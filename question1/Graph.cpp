#include "Graph.hpp"
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
    if(neighborsMatrix.size()!=numOfVertex){
        throw std::runtime_error("matrix size must be N x N");
    }
    for(int i=0;i<numOfVertex;i++){
        if(neighborsMatrix[i].size()!=numOfVertex){
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