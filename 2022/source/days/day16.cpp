#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/string_utils.hpp"

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
    std::vector<Node*> findShortestPath(Node& start, Node& end);

    Node* findNode(const std::string& label);
    const Node* findNode(const std::string& label) const;

    int32_t calcValveScore(const Node& currentNode, const Node& target) {}

    std::vector<std::unique_ptr<Node>> nodes;

  private:
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

namespace bblp::aoc {
namespace {

constexpr int32_t MINUTES = 30;

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

struct Evalution {
    constexpr auto operator<=>(const Evalution& other) noexcept { return value <=> other.value; }

    Node* node;
    int32_t value;
};

struct Step {
    Step(const Node* node, std::vector<Evalution> unvisited) : node(node), unvisited(std::move(unvisited)) {}

    const Node* node;
    std::vector<Evalution> unvisited;
};

int32_t evaluateAtDepth(const Graph& graph, std::set<const Node*>& opened, const Node* currentNode, const int32_t currentMinute) {
    const auto timeLeft = MINUTES - currentMinute;
    int32_t bestScore = 0;

    std::vector<Evalution> evaluations;
    evaluations.reserve(graph.nodes.size() - 1);

    for (const auto& path : currentNode->paths) {
        const auto distance = path.second.size() - 1;
        const auto score = path.first->flowRate * (timeLeft - distance - 1);

        const auto iter = opened.find(path.first);
        if (iter == opened.end() && score != 0) {
            const auto& eval = evaluations.emplace_back(path.first, score);
        }
    }

    for (const auto& eval : evaluations) {
        int32_t evalScore = 0;
        const auto& path = currentNode->paths.at(eval.node);
        const auto distance = path.size() - 1;
        if (distance <= timeLeft - 1) {
            opened.insert(eval.node);
            const int32_t evalScore = eval.value + evaluateAtDepth(graph, opened, eval.node, currentMinute + distance + 1);
            opened.erase(eval.node);

            if (bestScore < evalScore) {
                bestScore = evalScore;
            }
        }
    }

    return bestScore;
}

int32_t findMaxPossiblePressure(const Graph& graph) {
    const Node* startingNode = graph.findNode("AA");


    std::set<const Node*> opened;
    int32_t currentMinute = 0;
    int32_t totalScore = 0;

    return evaluateAtDepth(graph, opened, startingNode, 0);
}
}  // namespace

std::pair<std::string, std::string> day16() {
    const auto input = parse("resources/day16.txt");
    const auto pressure = findMaxPossiblePressure(input);
    return {std::to_string(pressure), ""};
}
}  // namespace bblp::aoc
