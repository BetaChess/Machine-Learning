#include "evaluator_xor.h"

#include <random>


EvaluatorXor::EvaluatorXor(
	size_t populationSize, 
	const float compatibilityDistanceCutoff, 
	const float excessConst, 
	const float disjointConst, 
	const float weightDiffConst)
	: Evaluator(populationSize, compatibilityDistanceCutoff, excessConst, disjointConst, weightDiffConst)
{
	// Create randomizer helpers
	std::random_device rd;
	std::mt19937 gen(rd());
	
	std::uniform_int_distribution<uint16_t> dist(0, 1);
	
	// Create random parameters for training data
	param1training.resize(300);
	param2training.resize(300);
	
	for (auto& b : param1training)
		b = dist(gen);
	for (auto& b : param2training)
		b = dist(gen);
	
	// Create random parameters for testing data
	param1test.resize(10000);
	param2test.resize(10000);
	
	for (auto& b : param1test)
		b = dist(gen);
	for (auto& b : param2test)
		b = dist(gen);
	
	// Create the starting genomes
	Genome baseGenome{ 2, 1 };
	baseGenome.addConnectionGene(0, 3, 1.0f);
	baseGenome.addConnectionGene(1, 3, 1.0f);
	baseGenome.addConnectionGene(2, 3, 1.0f);
	
	for (size_t i = 0; i < populationSize; i++)
		genomes_.emplace_back(baseGenome).mutateConnectionGenes();
}

std::array<EvaluatorXor::outputInfo, 5> EvaluatorXor::getTop5Info() const
{
	std::array<outputInfo, 5> top5{};
	
	for (const auto& [genome, fitness] : fitnessMap_)
	{
		if (fitness > top5[0].fitness)
		{
			top5[4] = top5[3];
			top5[3] = top5[2];
			top5[2] = top5[1];
			top5[1] = top5[0];
			top5[0] = { fitness, genome->numberOfNodes(), genome->numberOfConnections() };
		}
		else if (fitness > top5[1].fitness)
		{
			top5[4] = top5[3];
			top5[3] = top5[2];
			top5[2] = top5[1];
			top5[1] = { fitness, genome->numberOfNodes(), genome->numberOfConnections() };
		}
		else if (fitness > top5[2].fitness)
		{
			top5[4] = top5[3];
			top5[3] = top5[2];
			top5[2] = { fitness, genome->numberOfNodes(), genome->numberOfConnections() };
		}
		else if (fitness > top5[3].fitness)
		{
			top5[4] = top5[3];
			top5[3] = { fitness, genome->numberOfNodes(), genome->numberOfConnections() };
		}
		else if (fitness > top5[4].fitness)
		{
			top5[4] = { fitness, genome->numberOfNodes(), genome->numberOfConnections() };
		}
	}
	
	return top5;
}

EvaluatorXor::GenomeFitnessPair EvaluatorXor::evaluateGenomeTraining(Genome& genome)
{
	float fitness = 0;
	
	for (size_t i = 0; i < param1training.size(); i++)
	{
		genome.resetCache();
		genome.setInputValues({ static_cast<float>(param1training[i]), static_cast<float>(param2training[i]) });
		genome.evaluateOutputNodes();

		bool answer = param1training[i] ^ param2training[i];
		float distanceFromAnswer = std::abs(answer - genome.getOutputValue(0));
		
		fitness += 100.0f * (1.0f - distanceFromAnswer * distanceFromAnswer) - (genome.numberOfNodes() - 4) * 10.0f;
	}

	return
	{
		&genome,
		fitness / param1training.size()
	};
}

EvaluatorXor::GenomeFitnessPair EvaluatorXor::evaluateGenomeTest(Genome& genome)
{
	return { &genome, 0.0f };
}
