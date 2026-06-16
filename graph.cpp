// ============================================================
//  graph.cpp — TourismGraph implementation
//  BFS, DFS, and Dijkstra's Shortest Path Algorithm
// ============================================================
#include "../include/graph.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

namespace Tourism {

// -------------------------------------------------------
//  Graph Building
// -------------------------------------------------------

void TourismGraph::addDestination(int id, const std::string& name,
                                   const std::string& city) {
    if (vertices_.count(id)) {
        std::cout << "[Graph] WARNING: Destination ID " << id
                  << " already exists. Updating.\n";
    }
    vertices_[id] = { id, name, city };
    if (!adjacency_.count(id)) adjacency_[id] = {};
    std::cout << "[Graph] Added destination: [" << id << "] "
              << name << " (" << city << ")\n";
}

void TourismGraph::addRoute(int srcId, int dstId, double distance) {
    if (!vertices_.count(srcId) || !vertices_.count(dstId)) {
        std::cerr << "[Graph] ERROR: One or both destination IDs not found.\n";
        return;
    }
    // Undirected: add both directions
    adjacency_[srcId].push_back({ dstId, distance });
    adjacency_[dstId].push_back({ srcId, distance });
    std::cout << "[Graph] Route added: " << vertices_[srcId].name
              << " <--> " << vertices_[dstId].name
              << " (dist: " << distance << " km)\n";
}

void TourismGraph::removeDestination(int id) {
    if (!vertices_.count(id)) {
        std::cerr << "[Graph] ERROR: Destination ID " << id << " not found.\n";
        return;
    }
    // Remove all edges pointing to this node
    for (auto& [vid, edges] : adjacency_) {
        edges.erase(std::remove_if(edges.begin(), edges.end(),
            [id](const Edge& e){ return e.destination == id; }),
            edges.end());
    }
    adjacency_.erase(id);
    std::cout << "[Graph] Removed destination: " << vertices_[id].name << "\n";
    vertices_.erase(id);
}

void TourismGraph::removeRoute(int srcId, int dstId) {
    auto removeEdge = [](std::vector<Edge>& edges, int target) {
        edges.erase(std::remove_if(edges.begin(), edges.end(),
            [target](const Edge& e){ return e.destination == target; }),
            edges.end());
    };
    removeEdge(adjacency_[srcId], dstId);
    removeEdge(adjacency_[dstId], srcId);
    std::cout << "[Graph] Route removed between "
              << vertices_[srcId].name << " and "
              << vertices_[dstId].name << "\n";
}

// -------------------------------------------------------
//  BFS — Breadth-First Search
//  Returns nodes in BFS visitation order.
// -------------------------------------------------------
std::vector<int> TourismGraph::bfs(int startId) const {
    if (!vertices_.count(startId)) {
        std::cerr << "[BFS] Start ID " << startId << " not found.\n";
        return {};
    }
    std::vector<int>          visited;
    std::unordered_map<int,bool> seen;
    std::queue<int>           q;

    q.push(startId);
    seen[startId] = true;

    std::cout << "\n╔══════════════════════════════╗\n";
    std::cout << "║   BFS Traversal              ║\n";
    std::cout << "╚══════════════════════════════╝\n";
    std::cout << "Starting from: " << vertices_.at(startId).name << "\n\n";

    while (!q.empty()) {
        int curr = q.front(); q.pop();
        visited.push_back(curr);

        std::cout << "  Visited → [" << curr << "] "
                  << vertices_.at(curr).name
                  << " (" << vertices_.at(curr).city << ")\n";

        // Enqueue unvisited neighbors
        for (const auto& edge : adjacency_.at(curr)) {
            if (!seen[edge.destination]) {
                seen[edge.destination] = true;
                q.push(edge.destination);
                std::cout << "    Enqueued neighbor: ["
                          << edge.destination << "] "
                          << vertices_.at(edge.destination).name << "\n";
            }
        }
    }
    return visited;
}

// -------------------------------------------------------
//  DFS — Depth-First Search (iterative using explicit stack)
// -------------------------------------------------------
std::vector<int> TourismGraph::dfs(int startId) const {
    if (!vertices_.count(startId)) {
        std::cerr << "[DFS] Start ID " << startId << " not found.\n";
        return {};
    }
    std::vector<int>          visited;
    std::unordered_map<int,bool> seen;
    std::stack<int>           stk;

    stk.push(startId);

    std::cout << "\n╔══════════════════════════════╗\n";
    std::cout << "║   DFS Traversal              ║\n";
    std::cout << "╚══════════════════════════════╝\n";
    std::cout << "Starting from: " << vertices_.at(startId).name << "\n\n";

    while (!stk.empty()) {
        int curr = stk.top(); stk.pop();

        if (seen[curr]) continue;
        seen[curr] = true;
        visited.push_back(curr);

        std::cout << "  Visited → [" << curr << "] "
                  << vertices_.at(curr).name
                  << " (" << vertices_.at(curr).city << ")\n";

        // Push neighbors in reverse to maintain left-to-right order
        const auto& edges = adjacency_.at(curr);
        for (auto it = edges.rbegin(); it != edges.rend(); ++it) {
            if (!seen[it->destination]) {
                stk.push(it->destination);
                std::cout << "    Pushed to stack: ["
                          << it->destination << "] "
                          << vertices_.at(it->destination).name << "\n";
            }
        }
    }
    return visited;
}

// -------------------------------------------------------
//  Dijkstra's Algorithm — Single-Source Shortest Path
//  Uses a min-heap (priority queue).
// -------------------------------------------------------
TourismGraph::DijkstraResult
TourismGraph::shortestPath(int srcId, int dstId) const {
    if (!vertices_.count(srcId) || !vertices_.count(dstId)) {
        std::cerr << "[Dijkstra] Invalid source or destination ID.\n";
        return { {}, std::numeric_limits<double>::infinity(), false };
    }

    const double INF = std::numeric_limits<double>::infinity();

    // dist[id] = best known distance from src
    std::map<int, double> dist;
    std::map<int, int>    prev;   // predecessor map

    // FIX: Using pair to avoid -Wunused-variable warning from structured bindings
    for (const auto& pair : vertices_) {
        dist[pair.first] = INF;
        prev[pair.first] = -1;
    }
    dist[srcId] = 0.0;

    // min-heap: (distance, nodeId)
    using pdi = std::pair<double, int>;
    std::priority_queue<pdi, std::vector<pdi>, std::greater<pdi>> pq;
    pq.push({ 0.0, srcId });

    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║   Dijkstra's Shortest Path Algorithm ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    std::cout << "  Source      : [" << srcId << "] "
              << vertices_.at(srcId).name << "\n";
    std::cout << "  Destination : [" << dstId << "] "
              << vertices_.at(dstId).name << "\n\n";

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();

        if (d > dist[u]) continue;   // stale entry

        std::cout << "  Processing: [" << u << "] "
                  << vertices_.at(u).name
                  << "  (dist from src: " << std::fixed
                  << std::setprecision(2) << dist[u] << " km)\n";

        if (u == dstId) break;       // early exit

        for (const auto& edge : adjacency_.at(u)) {
            double alt = dist[u] + edge.weight;
            if (alt < dist[edge.destination]) {
                dist[edge.destination] = alt;
                prev[edge.destination] = u;
                pq.push({ alt, edge.destination });
                std::cout << "    Updated: ["
                          << edge.destination << "] "
                          << vertices_.at(edge.destination).name
                          << " → " << std::fixed << std::setprecision(2)
                          << alt << " km\n";
            }
        }
    }

    bool reached = dist[dstId] < INF;
    std::vector<int> path = reached ? reconstructPath(prev, srcId, dstId)
                                    : std::vector<int>{};

    // Print result
    std::cout << "\n  ┌─ Result ────────────────────────────────────\n";
    if (reached) {
        std::cout << "  │  Shortest distance: "
                  << std::fixed << std::setprecision(2)
                  << dist[dstId] << " km\n";
        std::cout << "  │  Optimal Route:\n  │    ";
        for (std::size_t i = 0; i < path.size(); ++i) {
            std::cout << vertices_.at(path[i]).name;
            if (i + 1 < path.size()) std::cout << " → ";
        }
        std::cout << "\n";
    } else {
        std::cout << "  │  No path exists between the two destinations.\n";
    }
    std::cout << "  └────────────────────────────────────────────\n";

    return { path, dist[dstId], reached };
}

// -------------------------------------------------------
//  Path reconstruction (backtrack through prev map)
// -------------------------------------------------------
std::vector<int> TourismGraph::reconstructPath(
    const std::map<int, int>& prev, int src, int dst) const
{
    std::vector<int> path;
    for (int at = dst; at != -1; at = prev.at(at)) {
        path.push_back(at);
        if (at == src) break;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// -------------------------------------------------------
//  Display helpers
// -------------------------------------------------------
void TourismGraph::printGraph() const {
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║        Tourism Graph (Adj. List)     ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    for (const auto& [id, vertex] : vertices_) {
        std::cout << "  [" << id << "] " << vertex.name
                  << " (" << vertex.city << ")\n";
        for (const auto& edge : adjacency_.at(id)) {
            std::cout << "       └─→ [" << edge.destination << "] "
                      << vertices_.at(edge.destination).name
                      << "  (" << std::fixed << std::setprecision(1)
                      << edge.weight << " km)\n";
        }
    }
    std::cout << "  Total vertices : " << vertices_.size() << "\n";
    int edgeCount = 0;
    for (const auto& [id, edges] : adjacency_) edgeCount += edges.size();
    std::cout << "  Total edges    : " << edgeCount / 2 << "\n\n";
}

void TourismGraph::printDestinations() const {
    std::cout << "\n  ID   Name                        City\n";
    std::cout << "  ──── ─────────────────────────── ─────────────\n";
    for (const auto& [id, v] : vertices_) {
        std::cout << "  " << std::setw(4) << id
                  << " " << std::setw(27) << std::left << v.name
                  << " " << v.city << "\n";
    }
    std::cout << std::right << "\n";
}

std::string TourismGraph::getDestinationName(int id) const {
    auto it = vertices_.find(id);
    return (it != vertices_.end()) ? it->second.name : "Unknown";
}

} // namespace Tourism