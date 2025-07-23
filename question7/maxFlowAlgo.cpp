#include "maxFlowAlgo.hpp"
/**
 * This function implements the Edmonds-Karp algorithm to find the maximum flow in a flow network.
 * It uses BFS to find augmenting paths in the residual graph.
 * @param resGraph The residual graph represented as an adjacency matrix.
 * @param s The source vertex.
 * @param t The sink vertex.
 * @param parent A vector to store the path from source to sink.
 * @return True if there is an augmenting path from source to sink, false otherwise.
 */
bool bfs(const std::vector<std::vector<int>>& resGraph,int s, int t, std::vector<int>& parent) {
    int n = resGraph.size();
    std::vector<bool> visited(n, false);
    std::queue<int> q;

    q.push(s);
    visited[s] = true;
    parent[s] = -1;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v = 0; v < n; ++v) {
            if (!visited[v] && resGraph[u][v] > 0) {
                parent[v] = u;
                visited[v] = true;
                q.push(v);
                if (v == t) return true;// we found an augmenting path
            }
        }
    }
    return false;  //we didnt found
}
/**
 * This function executes the Edmonds-Karp algorithm to find the maximum flow in a flow network.
 * It builds the residual graph from the original graph and uses BFS to find augmenting paths.
 * It calculates the maximum flow by updating the residual graph and summing the path flows.
 * @param g The graph represented as an adjacency matrix.
 * @return A string representation of the maximum flow value.
 */
std::string maxFlowAlgo::execute(const Graph& g) const {
    int n = g.getNumOfVertex();
    int s = 0, t = n - 1;

    std::vector<std::vector<int>> resGraph = g.getNeighborsMatrix();  // building the residual graph from the original graph

    std::vector<int> parent(n);
    int maxFlow = 0;

    while (bfs(resGraph, s, t, parent)) {
        int pathFlow = INT_MAX;

        // find the lowest capacity in the path.
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            pathFlow = std::min(pathFlow, resGraph[u][v]);
        }

        //update the residual graph
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            resGraph[u][v] -= pathFlow;
            resGraph[v][u] += pathFlow;
        }

        maxFlow += pathFlow;
    }

    return std::to_string(maxFlow);
}
