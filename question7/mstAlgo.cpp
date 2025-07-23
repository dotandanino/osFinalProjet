#include "mstAlgo.hpp"
/**
 * we are running the Prim's algorithm to find the minimum spanning tree of the graph.
 * The algorithm starts from an arbitrary vertex (in this case, vertex 0) and grows
 * the MST by adding the minimum weight edge that connects a vertex in the MST to a vertex
 * outside the MST until all vertices are included.
 * @param g the graph to find the minimum spanning tree of
 * @return a string representation of the total weight of the minimum spanning tree
 */
std::string MSTAlgo::execute(const Graph& g) const {
    int n = g.getNumOfVertex();
    std::vector<bool> inMST(n, false);//if the vertex is already in the MST
    std::vector<int> key(n, INT_MAX);// key[v] is the minimum weight edge connecting vertex v to the MST
    std::vector<int> parent(n, -1);// parent[v] is the vertex in the MST that connects to vertex v
    key[0] = 0;

    // min-heap: {weight, vertex}
    std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>, std::greater<>> pq;// priority queue to get the minimum weight edge every time
    pq.push({0, 0});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (inMST[u]) continue; // If u is already in the MST, skip it
        inMST[u] = true;

        const auto& neighbors = g.getNeighborsMatrix()[u];
        for (int v = 0; v < n; ++v) {//update the key and parent of the neighbors of u
            int weight = neighbors[v];
            if (weight > 0 && !inMST[v] && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
                pq.push({weight, v});//if its will be duplicate not realy matter because we will pop the minimum weight edge every time and check if the vertex is already in the MST
            }
        }
    }
    int weightSum = 0;
    for (int v = 1; v < n; ++v) {
        if (parent[v] != -1) {
            weightSum += g.getNeighborsMatrix()[parent[v]][v];//sum the weights of the edges in the MST
        }
    }
    return std::to_string(weightSum);
}
