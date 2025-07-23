#ifndef PATHCOVER_ALGORITHM
#define PATHCOVER_ALGORITHM
#include "GraphAlgorithm.hpp"
#include <vector>
#include <string>
#include <functional>

class PathCoverAlgo :public GraphAlgorithm{
    public:
        virtual std::string execute(const Graph& g) const override;
};

#endif