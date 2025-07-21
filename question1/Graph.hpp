#ifndef GRAPH_HPP
#define GRAPH_HPP
#include <iostream>
#include <vector>

class Graph{
private:
    int numOfVertex;
    std::vector<std::vector<bool>> neighborsMatrix;
public:
    Graph(int numOfVertex);
    Graph(int numOfVertex ,const std::vector<std::vector<bool>> neighborsMatrix);
    void addEdge(int src,int dest);
    void printGraph();
};

#endif