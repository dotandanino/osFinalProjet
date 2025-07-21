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
    std::vector<std::vector<bool>> neighborsMatrix;
public:
    Graph(int numOfVertex);
    Graph(int numOfVertex ,const std::vector<std::vector<bool>> neighborsMatrix);
    void addEdge(int src,int dest);
    std::vector<int> findEuler();
    void printGraph();
};
#endif