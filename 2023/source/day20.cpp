#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/point.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>
#include <list>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

namespace bblp::aoc {
namespace {
enum class NodeType { BROADCASTER, FLIP_FLOP, CONJUNCTION };

enum class PulseType { LOW, HIGH };

std::ostream& operator<<(std::ostream& stream, const PulseType pulse) {
    switch (pulse) {
        case PulseType::LOW:
            stream << "low";
            break;
        case PulseType::HIGH:
            stream << "high";
            break;
        default:
            throw std::logic_error("Invalid pulse");
    }
    return stream;
}

struct Module;
struct PulseSender {
    virtual void sendPulse(PulseType pulse, Module* input, const std::vector<Module*>& output) = 0;
};

struct Module {
    Module(PulseSender& pulseSender, std::string name) : pulseSender(pulseSender), name(std::move(name)) {}
    virtual ~Module() = default;
    virtual void onPulse(PulseType pulse, Module* source) = 0;

    void sendPulse(PulseType pulse) { pulseSender.sendPulse(pulse, this, outputs); }

    PulseSender& pulseSender;
    std::string name;
    std::vector<Module*> outputs;
};

struct BroadcasterModule : public Module {
    BroadcasterModule(PulseSender& pulseSender, std::string name) : Module(pulseSender, std::move(name)) {}

    void onPulse(PulseType pulse, Module*) override { sendPulse(pulse); }
};

struct FlipFlopModule : public Module {
    FlipFlopModule(PulseSender& pulseSender, std::string name) : Module(pulseSender, std::move(name)), state(false) {}

    void onPulse(PulseType pulse, Module*) override {
        if (pulse == PulseType::LOW) {
            state = !state;
            if (state) {
                sendPulse(PulseType::HIGH);
            } else {
                sendPulse(PulseType::LOW);
            }
        }
    }

    void reset() { state = false; }

    bool state;
};

struct ConjunctionModule : public Module {
    ConjunctionModule(PulseSender& pulseSender, std::string name) : Module(pulseSender, std::move(name)) {}

    void onPulse(PulseType pulse, Module* source) override {
        inputs.at(source) = pulse;
        if (std::all_of(inputs.cbegin(), inputs.cend(), [](const std::pair<Module*, PulseType>& pair) -> bool {
                return pair.second == PulseType::HIGH;
            })) {
            sendPulse(PulseType::LOW);
        } else {
            sendPulse(PulseType::HIGH);
        }
    }

    void reset() {
        for (auto& element : inputs) {
            element.second = PulseType::LOW;
        }
    }

    std::unordered_map<Module*, PulseType> inputs;
};

struct ButtonModule : public Module {
    ButtonModule(PulseSender& pulseSender) : Module(pulseSender, "button") {}

    void onPulse(PulseType pulse, Module* source) override {}
};

struct OutputModule : public Module {
    OutputModule(PulseSender& pulseSender, std::string name) : Module(pulseSender, std::move(name)), input(nullptr) {}

    void onPulse(PulseType pulse, Module* source) override {}

    ConjunctionModule* input;
};

struct CommunicationArray : public PulseSender {
    struct PulseTransmission {
        PulseType type;
        Module* source;
        Module* destination;
    };

    CommunicationArray() : buttonModule(*this) {}

    void run() {
        sendPulse(PulseType::LOW, &buttonModule, {broadcasterModule.get()});
        while (!pulseQueue.empty()) {
            std::queue<PulseTransmission> transmissions{pulseQueue};
            while (!pulseQueue.empty()) {
                pulseQueue.pop();
            }

            while (!transmissions.empty()) {
                const auto& transmission = transmissions.front();
                if (transmission.type == PulseType::HIGH) {
                    ++numberOfHighPulses;
                } else {
                    ++numberOfLowPulses;
                }
                transmission.destination->onPulse(transmission.type, transmission.source);
                transmissions.pop();
            }
        }
    }

    void sendPulse(PulseType pulse, Module* input, const std::vector<Module*>& outputs) override {
        if (pulse == PulseType::HIGH) {
            if (outputRequirements.contains(input) && outputRequirements.at(input) == 0U) {
                outputRequirements.at(input) = buttonPresses;
                // std::cout << "Requirement " << input->name << " satisfied with " << buttonPresses << " button pressed" << '\n';
            }
        }

        for (const auto& output : outputs) {
            // std::cout << input->name << " -" << pulse << "-> " << output->name << '\n';
            pulseQueue.push({pulse, input, output});
        }
    }

    void reset() {
        buttonPresses = 0U;
        numberOfLowPulses = 0U;
        numberOfHighPulses = 0U;

        for (auto& req : outputRequirements) {
            req.second = 0U;
        }
        for (auto& module : flipFlopModules) {
            module->reset();
        }
        for (auto& module : conjunctionModules) {
            module->reset();
        }
    }

    bool areAllRequirementsFulfilled() const {
        for (const auto& req : outputRequirements) {
            if (req.second == 0U) {
                return false;
            }
        }
        return true;
    }

    ButtonModule buttonModule;
    std::unique_ptr<BroadcasterModule> broadcasterModule;
    std::unique_ptr<OutputModule> outputModule;
    std::vector<std::unique_ptr<FlipFlopModule>> flipFlopModules;
    std::vector<std::unique_ptr<ConjunctionModule>> conjunctionModules;
    std::queue<PulseTransmission> pulseQueue;
    uint64_t numberOfLowPulses{0U};
    uint64_t numberOfHighPulses{0U};
    uint64_t buttonPresses{0U};
    std::unordered_map<Module*, uint64_t> outputRequirements;
};

Module* findModule(CommunicationArray& commArray, const std::string& name) {
    if (name == "broadcaster") {
        return commArray.broadcasterModule.get();
    }

    auto flipFlopIter =
        std::find_if(commArray.flipFlopModules.cbegin(), commArray.flipFlopModules.cend(),
                     [&name](const std::unique_ptr<FlipFlopModule>& module) { return name == module->name; });
    if (flipFlopIter != commArray.flipFlopModules.cend()) {
        return flipFlopIter->get();
    }

    auto conjunctionIter =
        std::find_if(commArray.conjunctionModules.cbegin(), commArray.conjunctionModules.cend(),
                     [&name](const std::unique_ptr<ConjunctionModule>& module) { return name == module->name; });
    if (conjunctionIter != commArray.conjunctionModules.cend()) {
        return conjunctionIter->get();
    }

    return nullptr;
}

auto parse(const std::filesystem::path& filePath) {
    CommunicationArray commArray;
    std::unordered_map<std::string, std::vector<std::string>> connections;

    const auto lineCallback = [&commArray, &connections](const std::string& line) {
        const auto parts = split(line, " -> ");
        const auto outputs = split(parts.back(), ", ");
        const auto& input = parts.front();
        if (input.starts_with("broadcaster")) {
            commArray.broadcasterModule = std::make_unique<BroadcasterModule>(commArray, "broadcaster");
            connections["broadcaster"] = outputs;
        } else if (input.starts_with('%')) {
            const auto name = input.substr(1U);
            commArray.flipFlopModules.push_back(std::make_unique<FlipFlopModule>(commArray, name));
            connections[name] = outputs;

        } else if (input.starts_with('&')) {
            const auto name = input.substr(1U);
            commArray.conjunctionModules.push_back(std::make_unique<ConjunctionModule>(commArray, name));
            connections[name] = outputs;
        }
    };
    parseInput(filePath, lineCallback);
    for (const auto& connection : connections) {
        auto input = findModule(commArray, connection.first);
        for (const auto& outputName : connection.second) {
            auto output = findModule(commArray, outputName);
            if (output != nullptr) {
                if (dynamic_cast<ConjunctionModule*>(output)) {
                    dynamic_cast<ConjunctionModule*>(output)->inputs[input] = PulseType::LOW;
                }
            } else {
                commArray.outputModule = std::make_unique<OutputModule>(commArray, outputName);
                commArray.outputModule->input = dynamic_cast<ConjunctionModule*>(input);
                output = commArray.outputModule.get();
            }
            input->outputs.push_back(output);
        }
    }

    if (commArray.outputModule) {
        for (auto& input : commArray.outputModule->input->inputs) {
            commArray.outputRequirements[input.first] = 0U;
        }
    }

    return commArray;
}

uint64_t calculatePartOne(CommunicationArray& input) {
    for (int i = 0; i < 1000; ++i) {
        ++input.buttonPresses;
        input.run();
    }

    // std::cout << "Number of low pulses after 1000 button presses is " << input.numberOfLowPulses << '\n';
    // std::cout << "Number of high pulses after 1000 button presses is " << input.numberOfHighPulses << '\n';
    return input.numberOfLowPulses * input.numberOfHighPulses;
}

uint64_t calculatePartTwo(CommunicationArray& input) {
    input.reset();
    while (!input.areAllRequirementsFulfilled()) {
        ++input.buttonPresses;
        input.run();
    }

    uint64_t result{0U};
    for (auto iter = input.outputRequirements.cbegin(); iter != input.outputRequirements.cend(); ++iter) {
        if (result == 0U) {
            result = iter->second;
        } else {
            result = std::lcm(result, iter->second);
        }
    }

    return result;
}
}  // namespace

std::pair<std::string, std::string> day20() {
    auto input = parse("resources/day20.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
