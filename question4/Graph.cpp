#include "../question2/Graph.hpp"
using namespace std;
/**
 * @brief Constructor for Graph class that initializes a graph with a given number of vertices.
 * @param numOfVertex The number of vertices in the graph.
 * @throws std::runtime_error if numOfVertex is not positive.
 */
Graph:: Graph(int numOfVertex){
    if(numOfVertex<=0){
        throw std::runtime_error("you must have positive number of vertex");
    }
    this->numOfVertex=numOfVertex;
    neighborsMatrix.resize(numOfVertex,std::vector<bool>(numOfVertex,false));
}

/**
 * @brief Constructor for Graph class that initializes a graph with a given number of vertices and a neighbors matrix.
 * @param numOfVertex The number of vertices in the graph.
 * @param neighborsMatrix A 2D vector representing the adjacency matrix of the graph.
 * @throws std::runtime_error if numOfVertex is not positive, if the matrix size is not N x N, or if some vertex is connected to itself or if the graph is not undirected.
 */
Graph::Graph(int numOfVertex ,const std::vector<std::vector<bool>> neighborsMatrix){
     if(numOfVertex<=0){
        throw std::runtime_error("you must have positive number of vertex");
    }
    this->numOfVertex=numOfVertex;
    if((int)neighborsMatrix.size()!=numOfVertex){
        throw std::runtime_error("matrix size must be N x N");
    }
    for(int i=0;i<numOfVertex;i++){
        if((int)neighborsMatrix[i].size()!=numOfVertex){
            throw std::runtime_error("matrix size must be N x N");
        }
    }
    for(int i=0;i<numOfVertex;i++){
        if(neighborsMatrix[i][i]){
            throw std::runtime_error("vtx cant be connected to himself");
        }
        for(int j=0;j<i;j++){
            if(neighborsMatrix[i][j]!=neighborsMatrix[j][i]){
                throw std::runtime_error("the graph must be undirected");
            }
        }
    }
    this->neighborsMatrix = neighborsMatrix;
}

/**
 * @brief Adds an edge between two vertices in the graph.
 * @param src The source vertex.
 * @param dest The destination vertex.
 * @throws std::runtime_error if src or dest is out of bounds or if they are the same vertex or they are already connected.
 */
void Graph::addEdge(int src,int dest){
    if(src>=numOfVertex || src<0 || dest>=numOfVertex || dest<0){
        throw std::runtime_error("src and dest must be in {0,....,n-1}");
    }
    if(src == dest){
        throw std::runtime_error("src and dest cant be the same");
    }
    if(neighborsMatrix[src][dest]){
        throw std::runtime_error("this 2 vtx already connected");
    }
    neighborsMatrix[src][dest]=true;
    neighborsMatrix[dest][src]=true;
}

/**
 * @brief Checks if the graph is connected.
 * @return true if the graph is connected, false otherwise.
 * This function uses a fake breadth-first search (BFS) algorithm to traverse the graph and check
 * we build this function beacuse graph must be connected to have Euler cycle.
 */
bool Graph::isConnected(){
    vector<int> distance;
    for(int i=0;i<numOfVertex;i++){
        distance.push_back(1);
    }
    queue<int> bfs;
    bfs.push(0);
    while(!bfs.empty()){
        int v=bfs.front();
        bfs.pop();
        for(int i=0;i<numOfVertex;i++){
            if(neighborsMatrix[v][i] && distance[i] == 1){
                distance[i] =0;
                bfs.push(i);
            }
        }
    }
    for(int i=0;i<numOfVertex;i++){
        if(distance[i]==1){
            return false;
        }
    }
    return true;
}

/**
 * @brief Finds an Euler cycle in the graph.
 * if the is a Euler cycle in the graph, it returns a vector containing the vertices in the cycle by their order.
 * If the graph is not connected or if any vertex has an odd degree, it returns an empty vector.
 * @return A vector of integers representing the vertices in the Euler cycle, or an empty vector if no Euler cycle exists.
 */
vector<int> Graph::findEuler(){
    int counter=0;
    vector<int> EulerCycle;
    if(!this->isConnected()){
        cout<<"There cant be Euler cycle in unconnected graph"<<endl;
        return EulerCycle;
    }
    for(int i=0;i<numOfVertex;i++){
        counter=0;
        for(int j=0;j<numOfVertex;j++){
            if(neighborsMatrix[i][j]){
                counter++;
            }
        }
        if(counter%2==1){
            cout<<"This graph is not containing Euler cycle becase vtx "<<i<<" have odd degree"<<endl;
            return EulerCycle;
        }
    }
    vector<vector<bool>> copyMat = neighborsMatrix;
    stack<int> s;
    s.push(0);
    while(!s.empty()){
        int v= s.top();
        bool foundEdge = false;
        for(int i=0;i<numOfVertex;i++){
            if(copyMat[v][i]){
                foundEdge=true;
                copyMat[v][i]=false;
                copyMat[i][v]=false;
                s.push(i);
                break;
            }
        }
        if(!foundEdge){
            EulerCycle.push_back(v);
            s.pop();
        }
    }
    return EulerCycle;
}

/**
 * @brief this function print the neighbors of every vertex in the graph.
 */
void Graph::printGraph(){
   for(int i=0;i<numOfVertex;i++){
        cout<<"neighbors of : "<<i<<std::endl;
        for(int j=0;j<numOfVertex;j++){
            if(neighborsMatrix[i][j]){
                cout<<" " <<j ;
            }
        }
        cout<<std::endl;
   } 
}