#include "pathCoverAlgo.hpp"
using namespace std;
enum State { UNVISITED, VISITING, VISITED };

bool dfs(int node, const vector<vector<int>>& graph, vector<State>& state) {
    state[node] = VISITING;
    for (int i=0; i < graph[node].size(); ++i) {
        if(graph[node][i] < 0) continue; // skip if no edge
        if (state[i] == VISITING) {//if we comeback to the same vtx we have cycle
            return true;
        }
        if (state[i] == UNVISITED) {
            if (dfs(i, graph, state)) return true;
        }
    }
    state[node] = VISITED;
    return false;
}

bool hasCycle(const vector<vector<int>>& graph) {
    int n = graph.size();
    vector<State> state(n, UNVISITED);
    for (int i = 0; i < n; ++i) {
        if (state[i] == UNVISITED) {
            if (dfs(i, graph, state)) return true;
        }
    }
    return false;
}

std::string PathCoverAlgo::execute(const Graph& g) const {
    int n = g.getNumOfVertex();
    const auto& graph = g.getNeighborsMatrix();
    // Step 1: check if DAG
    if (hasCycle(graph)) {
        return "Graph contains a cycle";
    }
    // Step 2: build bipartite graph from DAG
    vector<vector<int>> bipartiteGraph(n); // u_out -> list of v_in
    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (graph[u][v] >= 0) {
                bipartiteGraph[u].push_back(v); // from u_out to v_in
            }
        }
    }
    // Step 3: maximum bipartite matching (using DFS)
    vector<int> matchTo(n, -1); // v_in matched to u_out
    function<bool(int, vector<bool>&)> bpm = [&](int u, vector<bool>& visited) {
        for (int v : bipartiteGraph[u]) {
            if (!visited[v]) {
                visited[v] = true;
                if (matchTo[v] == -1 || bpm(matchTo[v], visited)) {
                    matchTo[v] = u;
                    return true;
                }
            }
        }
        return false;
    };
    for (int u = 0; u < n; ++u) {
        vector<bool> visited(n, false);
        bpm(u, visited);
    }
    // matchTo[v] = u means v is matched to u
    // matchedU[i] = true means u is matched to some v
    vector<bool> matchedU(n, false);
    for (int v = 0; v < n; ++v) {
        if (matchTo[v] != -1) {
            matchedU[matchTo[v]] = true;
        }
    }
    for(int u = 0; u < n; ++u) {
        cout<< "u: " << u << " matched to v: " << matchTo[u] << endl;
    }
    //Step 4: searching for paths
    vector<vector<int>> paths;
    for (int u = 0; u < n; ++u) {
        if (!matchedU[u]) {//start from points that wasnt in any path before
            vector<int> path;
            int current = u;
            path.push_back(current);
            cout<<"AAAA"<<endl;
            // building the path
            while (true) {
                // find the v that corresponds to current
                int nextV = -1;
                cout<< current <<endl;
                for (int v =0 ; v< n; ++v) {
                    if (matchTo[current] == v) {
                        nextV = v;
                        break;
                    }
                }
                cout<< nextV <<endl;
                if (nextV == -1) break;// no more edges from current to any v_in
                
                //move to the next vertex in the path
                current = nextV;
                path.insert(path.begin(),current);//add the next vertex to the path
            }
            // add the path to the list of paths
            paths.push_back(path);
        }
    }

    // Step 5: format the result
    std::string result = "Minimal Path Cover: " + std::to_string(paths.size()) + "\n";
    for (size_t i = 0; i < paths.size(); ++i) {
        result += "Path " + std::to_string(i + 1) + ": ";
        for (size_t j = 0; j < paths[i].size(); ++j) {
            result += std::to_string(paths[i][j]);
            if (j + 1 < paths[i].size()) result += " -> ";
        }
        result += "\n";
    }
    return result;
}
