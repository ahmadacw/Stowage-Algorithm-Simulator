// AlgorithmRegistration.cpp
#include "../Common/AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"

AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithm_factory)
{
	// Register algorithm factory into the AlgorithmRegistrar singleton instance
	AlgorithmRegistrar::getInstance().registerFactory(algorithm_factory);
}
