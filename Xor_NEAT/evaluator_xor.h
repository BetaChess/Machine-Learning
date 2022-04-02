#ifndef EVALUATOR_XOR_H
#define EVALUATOR_XOR_H

#include "evaluator.h"

#include <vector>
#include <array>

class EvaluatorXor : public Evaluator
{
public:
	struct outputInfo
	{
		float fitness;
		float correctPercentage;
		size_t numberOfNodes;
		size_t numberOfConnections;
	};
	
public:
	explicit EvaluatorXor(size_t populationSize, const float compatibilityDistanceCutoff = 3.0f, const float excessConst = 1.0f, const float disjointConst = 1.0f, const float weightDiffConst = 0.4f);	
	
	std::array<outputInfo, 5> getTop5Info();
	
private:
	std::vector<bool> param1training = {0, 0, 1, 1};
	std::vector<bool> param2training = {0, 1, 0, 1};
	
	std::vector<bool> param1test;
	std::vector<bool> param2test;
	
protected:
	[[nodiscard]] float evaluateGenomeTraining(Genome& genome) override;
	[[nodiscard]] FitnessCorrectpercentagePair evaluateGenomeTest(Genome& genome) override;
};

#endif /* EVALUATOR_XOR_H */