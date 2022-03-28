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
		size_t numberOfNodes;
		size_t numberOfConnections;
	};
	
public:
	explicit EvaluatorXor(size_t populationSize, const float compatibilityDistanceCutoff = 3.0f, const float excessConst = 1.0f, const float disjointConst = 1.0f, const float weightDiffConst = 0.4f);	
	
	std::array<outputInfo, 5> getTop5Info() const;
	
private:
	std::vector<bool> param1training;
	std::vector<bool> param2training;
	
	std::vector<bool> param1test;
	std::vector<bool> param2test;
	
protected:
	[[nodiscard]] GenomeFitnessPair evaluateGenomeTraining(Genome& genome) override;
	[[nodiscard]] GenomeFitnessPair evaluateGenomeTest(Genome& genome) override;
};

#endif /* EVALUATOR_XOR_H */