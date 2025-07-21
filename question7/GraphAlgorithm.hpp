#ifndef GRAPH_ALGORITHM
#define GRAPH_ALGORITHM
#include "Graph.hpp"

class GraphAlgorithm{
    public:
        virtual std::string execute(const Graph& g) = 0;
        virtual ~GraphAlgorithm()=default;
};

#endif