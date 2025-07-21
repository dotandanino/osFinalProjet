#ifndef SCC_ALGORITHM
#define SCC_ALGORITHM
#include "GraphAlgorithm.hpp"

class SCCAlgo : GraphAlgorithm{
    public:
        virtual std::string execute(const Graph& g) override;
};

#endif