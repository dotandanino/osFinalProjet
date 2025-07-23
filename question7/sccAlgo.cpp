#include "sccAlgo.hpp"
/**
 * a function to perform DFS on the original graph in use to order the vertices by their finishing times.
 * @param v the current vertex
 * @param g the graph to perform DFS on
 * @param visited a vector to keep track of visited vertices
 * @param finished a stack to store the vertices in the order of their finishing times
 */
void dfsOriginal(int v, const Graph& g, std::vector<bool>& visited, std::stack<int>& finished) {
    visited[v] = true;
    const auto& matrix = g.getNeighborsMatrix();
    for (int i = 0; i < g.getNumOfVertex(); ++i) {
        if (matrix[v][i] >= 0 && !visited[i]) {
            dfsOriginal(i, g, visited, finished);
        }
    }
    finished.push(v);
}

/**
 * a function to get the transpose of the graph.
 * @param g the original graph
 * @return a new graph that is the transpose of the original graph
 */
Graph getTranspose(const Graph& g) {
    int n = g.getNumOfVertex();
    Graph transposed(n);
    const auto& matrix = g.getNeighborsMatrix();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (matrix[i][j] >= 0) {
                transposed.addDirectedEdge(j, i, matrix[i][j]);
            }
        }
    }
    return transposed;
}

/**
 * a function to perform DFS on the transposed graph to find strongly connected components.
 * @param v the current vertex
 * @param g the transposed graph
 * @param visited a vector to keep track of visited vertices
 * @param component a vector to store the current strongly connected component
 */
void dfsTranspose(int v, const Graph& g, std::vector<bool>& visited, std::vector<int>& component) {
    visited[v] = true;
    component.push_back(v);
    const auto& matrix = g.getNeighborsMatrix();
    for (int i = 0; i < g.getNumOfVertex(); ++i) {
        if (matrix[v][i] >= 0 && !visited[i]) {
            dfsTranspose(i, g, visited, component);
        }
    }
}

/**
 * a function to execute the Kosaraju's algorithm to find strongly connected components in a directed graph.
 * @param g the directed graph
 * @return a string representation of the strongly connected components found in the graph
 */
std::string SCCAlgo::execute(const Graph& g) const{
    int n = g.getNumOfVertex();
    std::vector<bool> visited(n, false);
    std::stack<int> finished;
    std::cout<<"starting step 1"<<std::endl;
    // Step 1: Perform DFS on the original graph and fill the stack with vertices in finishing order
    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            dfsOriginal(i, g, visited, finished);
        }
    }
    // Step 2: Get the transpose of the graph
    Graph transposed = getTranspose(g);
    std::fill(visited.begin(), visited.end(), false);
    std::ostringstream oss;
    int count = 0;
    // step 3: Perform DFS on the transposed graph in the order of finishing times
    while (!finished.empty()) {
        int v = finished.top();
        finished.pop();
        if (!visited[v]) {
            std::vector<int> component;
            dfsTranspose(v, transposed, visited, component);
            // Step 4: save the strongly connected component in the output string
            oss << "SCC #" << ++count << ": ";
            for (int u : component) {
                oss << u << " ";
            }
            oss << "\n";
        }
    }
    return oss.str();
}