#ifndef MST_ALGORITHM
#define MST_ALGORITHM
#include "GraphAlgorithm.hpp"

class MSTAlgo : GraphAlgorithm{
    public:
        virtual std::string execute(const Graph& g) override;
};

#endif