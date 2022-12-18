#include "days.hpp"

#include "common/file_utils.hpp"
#include "common/string_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>

using FlowRate = int32_t;

struct Node {
    constexpr auto operator<=>(const Node& other) const noexcept { return label <=> other.label; }

    std::string label;
    FlowRate flowRate;
    std::vector<std::string> connectionsLabels;
    std::vector<Node*> connections;
    std::unordered_map<Node*, std::vector<Node*>> paths;
};

class Graph {
  public:
    void buildConnections();
    void buildShortestPaths();

    Node* findNode(const std::string& label);
    const Node* findNode(const std::string& label) const;

    int32_t calcValveScore(const Node& currentNode, const Node& target) {}

    std::vector<std::unique_ptr<Node>> nodes;

  private:
    void buildShortestPaths(Node& node);
};

void Graph::buildConnections() {
    for (auto& node : nodes) {
        for (const auto& label : node->connectionsLabels) {
            auto foundNode = findNode(label);
            if (foundNode != nullptr) {
                node->connections.emplace_back(foundNode);
            }
        }
    }
}

void Graph::buildShortestPaths() {
    for (auto& node : nodes) {
        buildShortestPaths(*node);
    }
}

Node* Graph::findNode(const std::string& label) {
    const auto iter =
        std::find_if(nodes.begin(), nodes.end(), [&label](const auto& node) { return node->label == label; });
    return (iter != nodes.end() ? iter->get() : nullptr);
}

const Node* Graph::findNode(const std::string& label) const {
    const auto iter =
        std::find_if(nodes.begin(), nodes.end(), [&label](const auto& node) { return node->label == label; });
    return (iter != nodes.end() ? iter->get() : nullptr);
}

void Graph::buildShortestPaths(Node& start) {
    using p = std::pair<int32_t, const Node*>;

    std::unordered_map<Node*, bool> added;
    std::unordered_map<Node*, int32_t> shortestDistances;
    for (const auto& node : nodes) {
        added[node.get()] = false;
        shortestDistances[node.get()] = INT32_MAX;
    }
    shortestDistances[&start] = 0;

    std::unordered_map<Node*, Node*> parents;
    parents[&start] = nullptr;

    for (const auto& n : nodes) {
        Node* nearestNode = nullptr;
        int shortestDistance = INT_MAX;
        for (const auto& node : nodes) {
            const auto nodePtr = node.get();

            if (added[nodePtr]) {
                continue;
            }

            if (shortestDistances[node.get()] < shortestDistance) {
                nearestNode = node.get();
                shortestDistance = shortestDistances[node.get()];
            }
        }

        added[nearestNode] = true;

        for (auto& m : nodes) {
            Node* connection = m.get();
            const auto iter = std::find(n->connections.cbegin(), n->connections.cend(), connection);
            if (iter == n->connections.cend()) {
                continue;
            }

            int edgeDistance = 1;

            if (edgeDistance > 0 && ((shortestDistance + edgeDistance) < shortestDistances[connection])) {
                parents[connection] = nearestNode;
                shortestDistances[connection] = shortestDistance + edgeDistance;
            }
        }
    }

    for (const auto& destinationNode : nodes) {
        if (destinationNode.get() == &start) {
            continue;
        }

        Node* currentNode = destinationNode.get();
        std::vector<Node*> path;
        path.emplace_back(currentNode);
        while (currentNode != &start) {
            Node* parent = parents[currentNode];
            path.push_back(parent);
            currentNode = parent;
        }

        std::reverse(path.begin(), path.end());

        start.paths[destinationNode.get()] = path;
    }
}

namespace bblp::advent_of_code_2022 {
namespace {

std::vector<std::string> parseConnections(const std::string& string) {
    static constexpr std::string_view VALVE = "valve";

    auto valvePos = string.find_last_of("valve");
    if (string.at(valvePos + 1) == 's') {
        ++valvePos;
    }

    return split(string.substr(valvePos + 2), ", ");
}

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    Graph input;
    const auto lineCallback = [&input](const std::string& line) {
        if (line.empty()) {
            return;
        }

        const auto parts = split(line, ";");
        const auto equalPos = parts[0].find_last_of("=");

        const auto label = parts[0].substr(6, 2);
        const auto flowRate = std::stol(parts[0].substr(equalPos + 1));
        const auto connections = parseConnections(parts[1]);

        input.nodes.emplace_back(std::make_unique<Node>(label, flowRate, connections));
    };
    parseInput(filePath, lineCallback);
    input.buildConnections();
    input.buildShortestPaths();
    return input;
}

static constexpr int32_t MINUTES = 30;

enum Action { MOVE, OPEN };

struct Step {
    Step(Action action, const Node* node, std::list<const Node*> unvisited)
        : action(action), node(node), unvisited(std::move(unvisited)) {}

    Action action;
    const Node* node;
    std::list<const Node*> unvisited;
};

int32_t calculateValveGain(const FlowRate flowRate, const int32_t minute) {
    return flowRate * (MINUTES - minute);
}

void printPath(std::ostream& stream, const std::list<Step>& path) {
    stream << "Path: ";
    for (const auto& step : path) {
        stream << " " << step.node->label;
    }
    stream << '\n';
}

bool isOpen(const Node* node, const std::list<Step>& path) {
    return std::find_if(path.cbegin(), path.cend(), [&node](const Step& step) {
               return (step.node == node && step.action == Action::OPEN);
           }) != path.cend();
}

int32_t findMaxPossiblePressure(const Graph& graph) {
    std::ofstream output("resources/day16_1.txt");

    const Node* startingNode = graph.findNode("AA");
    std::list<Step> path;
    path.push_back(Step(Action::MOVE, startingNode,
                        std::list<const Node*>(startingNode->connections.cbegin(), startingNode->connections.cend())));

    return 0;
    while (!path.empty()) {
        printPath(std::cout, path);
        Step& currentStep = path.back();

        if (path.size() >= MINUTES) {
            path.pop_back();
        }

        if (currentStep.unvisited.empty()) {
            path.pop_back();
        }

        auto nextNode = currentStep.unvisited.front();
        if (!isOpen(currentStep.node, path)) {
            std::vector<std::pair<const Node*, int32_t>> gains;
            gains.reserve(currentStep.unvisited.size());
            gains.emplace_back(currentStep.node, calculateValveGain(currentStep.node->flowRate, path.size()));
            for (const auto& unvisited : currentStep.unvisited) {
                gains.emplace_back(unvisited, calculateValveGain(unvisited->flowRate, path.size() + 1));
            }

            const auto max = std::max_element(gains.cbegin(), gains.cend(), [](const auto& first, const auto& second) {
                return first.second < second.second;
            });

            nextNode = max->first;
        }

        if (nextNode == currentStep.node) {
            path.push_back(Step(Action::OPEN, nextNode,
                                std::list<const Node*>(nextNode->connections.cbegin(), nextNode->connections.cend())));
        } else {
            path.push_back(Step(Action::MOVE, nextNode,
                                std::list<const Node*>(nextNode->connections.cbegin(), nextNode->connections.cend())));
            currentStep.unvisited.clear();
        }
    }

    for (auto currentMinute = 1; currentMinute <= MINUTES; ++currentMinute) {
    }

    return 0;
}
}  // namespace

std::pair<std::string, std::string> day16() {
    const auto input = parse("resources/day16.txt");
    const auto pressure = findMaxPossiblePressure(input);
    return {"", ""};
}
}  // namespace bblp::advent_of_code_2022
