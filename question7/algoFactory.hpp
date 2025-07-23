#ifndef ALGORITHM_FACTORY_HPP
#define ALGORITHM_FACTORY_HPP

#include <memory>
#include <string>
#include "GraphAlgorithm.hpp"
enum class AlgorithmType {
    MaxFlow=1,
    MST,
    PathCover,
    SCC};
/**
 * @brief Factory function to create graph algorithms based on the algorithm name.
 * @param algoName The name of the algorithm to create.
 * @return A unique pointer to the created GraphAlgorithm object, or nullptr if the algorithm is not recognized.
 */
std::unique_ptr<GraphAlgorithm> createAlgorithm(AlgorithmType algoType);

#endif