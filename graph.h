// ============================================================
//  Smart Tourism System — Graph Module (Header)
//  Represents tourist destinations and travel routes.
//  Algorithms: BFS, DFS, Dijkstra's Shortest Path
// ============================================================
#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>
#include <stack>
#include <limits>
#include <string>
#include <functional>
#include <algorithm>
#include <utility> // FIX: Added for std::pair compatibility

namespace Tourism {

// A weighted, undirected graph edge
struct Edge {
    int   destination;   // destination node ID
    double weight;       // distance / cost between nodes
};

// A single tourist destination vertex
struct Vertex {
    int         id;
    std::string name;
    std::string city;
};

// -------------------------------------------------------
//  TourismGraph
//  Adjacency-list weighted graph.
// -------------------------------------------------------
class TourismGraph {
public:
    // ---------- Graph Building ----------
    void addDestination(int id, const std::string& name, const std::string& city);
    void addRoute(int srcId, int dstId, double distance);
    void removeDestination(int id);
    void removeRoute(int srcId, int dstId);

    // ---------- Traversals ----------
    std::vector<int> bfs(int startId) const;
    std::vector<int> dfs(int startId) const;

    // ---------- Shortest Path (Dijkstra) ----------
    struct DijkstraResult {
        std::vector<int>    path;      // ordered list of node IDs
        double              distance;  // total distance
        bool                found;     // reachable?
    };
    DijkstraResult shortestPath(int srcId, int dstId) const;

    // ---------- Display ----------
    void printGraph()         const;
    void printDestinations()  const;
    std::string getDestinationName(int id) const;

private:
    std::map<int, Vertex>            vertices_;
    std::map<int, std::vector<Edge>> adjacency_;

    // Helper: reconstruct path from predecessor map
    std::vector<int> reconstructPath(
        const std::map<int, int>& prev, int src, int dst) const;
};

} // namespace Tourism