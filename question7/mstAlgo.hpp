#ifndef MST_ALGORITHM
#define MST_ALGORITHM
#include "GraphAlgorithm.hpp"
#include <vector>             // std::vector
#include <queue>              // std::priority_queue
#include <limits.h>

class MSTAlgo :public GraphAlgorithm{
    public:
        virtual std::string execute(const Graph& g) const override;
};

#endif