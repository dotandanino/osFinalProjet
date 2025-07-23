#ifndef GRAPH_HPP
#define GRAPH_HPP
#include <iostream>
#include <vector>
#include <stack>
#include <queue>

class Graph{

private:
    int numOfVertex;
    bool isConnected();
    std::vector<std::vector<int>> neighborsMatrix;
public:
    Graph(int numOfVertex);
    Graph(int numOfVertex ,const std::vector<std::vector<int>> neighborsMatrix);
    void addEdge(int src,int dest,int weight =0);
    std::vector<int> findEuler();
    void printGraph();
    void addDirectedEdge(int src,int dest,int weight =0);
    int getNumOfVertex() const { return numOfVertex; }
    const std::vector<std::vector<int>>& getNeighborsMatrix() const { return neighborsMatrix; }
};
#endif