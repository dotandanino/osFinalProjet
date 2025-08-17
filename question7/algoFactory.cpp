#include "algoFactory.hpp"

std::unique_ptr<GraphAlgorithm> createAlgorithm(AlgorithmType algoType) {
    switch(algoType) {
        case AlgorithmType::MaxFlow:
            return std::make_unique<maxFlowAlgo>();
        case AlgorithmType::MST:
            return std::make_unique<MSTAlgo>();
        case AlgorithmType::PathCover:
            return std::make_unique<PathCoverAlgo>();
        case AlgorithmType::SCC:
            return std::make_unique<SCCAlgo>();
        default:
            return nullptr; // there is no such algorithm
    }
}