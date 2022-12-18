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
#include <set>
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
    std::vector<Node*> findShortestPath(Node& start, Node& end);
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
    for (auto& start : nodes) {
        for (auto& end : nodes) {
            if (start.get() == end.get()) {
                continue;
            }

            start->paths[end.get()] = findShortestPath(*start, *end);
        }
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

std::vector<Node*> Graph::findShortestPath(Node& start, Node& end) {
    using p = std::pair<int32_t, const Node*>;

    std::unordered_map<Node*, bool> visited;
    std::unordered_map<Node*, int32_t> distances;
    std::unordered_map<Node*, Node*> parents;
    for (const auto& node : nodes) {
        visited[node.get()] = false;
        distances[node.get()] = INT32_MAX;
        parents[node.get()] = nullptr;
    }
    distances[&start] = 0;
    parents[&start] = nullptr;

    std::list<Node*> queue;
    queue.push_back(&start);

    while (!queue.empty()) {
        Node* currentNode = queue.front();
        queue.pop_front();

        for (auto& connection : currentNode->connections) {
            if (visited[connection]) {
                continue;
            }

            visited[connection] = true;
            distances[connection] = distances[currentNode];
            parents[connection] = currentNode;
            queue.push_back(connection);

            if (connection == &end) {
                queue.clear();
                break;
            }
        }
    }

    std::vector<Node*> path;
    Node* currentNode = &end;
    path.emplace_back(currentNode);
    while (currentNode != &start) {
        Node* parent = parents[currentNode];
        path.push_back(parent);
        currentNode = parent;
    }

    std::reverse(path.begin(), path.end());
    return path;
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

    std::set<const Node*> opened;
    const Node* currentNode = startingNode;
    int32_t currentMinute = 0;
    int32_t totalScore = 0;
    while (currentMinute < MINUTES) {

        std::cout << "Current node is " << currentNode->label << '\n';

        const auto timeLeft = MINUTES - currentMinute;

        std::vector<std::pair<Node*, int32_t>> evaluation;
        evaluation.reserve(graph.nodes.size() - 1);

        for (const auto& path : currentNode->paths) {
            const auto distance = path.second.size() - 1;
            const auto score = path.first->flowRate * (timeLeft - distance - 1);

            const auto iter = opened.find(path.first);
            if (iter == opened.end() && score != 0) {
                evaluation.emplace_back(std::make_pair(path.first, score));
                std::cout << "Evaluation for node " << path.first->label << " is " << score << '\n';
            }
        }

        std::sort(evaluation.begin(), evaluation.end(), [](const auto& first, const auto& second) { return first.second < second.second; });

        if (evaluation.empty()) {
            currentMinute = MINUTES;
        } else {
            auto maxIter =
                std::max_element(evaluation.cbegin(), evaluation.cend(),
                                 [](const auto& first, const auto& second) { return first.second < second.second; });

            auto& path = currentNode->paths.at(maxIter->first);
            const auto distance = path.size();
            if (distance <= timeLeft - 1) {
                totalScore += maxIter->first->flowRate * (timeLeft - distance - 1);
                opened.insert(maxIter->first);
            }

            currentMinute += distance + 1;
            currentNode = maxIter->first;
        }
    }

    return totalScore;
}
}  // namespace

std::pair<std::string, std::string> day16() {
    const auto input = parse("resources/day16.txt");
    const auto pressure = findMaxPossiblePressure(input);
    return {std::to_string(pressure), ""};
}
}  // namespace bblp::advent_of_code_2022
