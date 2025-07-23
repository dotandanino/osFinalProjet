#ifndef MAXFLOW_ALGORITHM
#define MAXFLOW_ALGORITHM
#include "GraphAlgorithm.hpp"
#include <climits>
#include <queue>
#include <vector>

class maxFlowAlgo :public GraphAlgorithm{
    public:
        virtual std::string execute(const Graph& g) const override;
};

#endif