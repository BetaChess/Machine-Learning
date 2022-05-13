#include "evaluator_xor.h"

#include "benchmarker.h"

#include <random>
#include <iostream>


EvaluatorXor::EvaluatorXor(
	size_t populationSize, 
	const float compatibilityDistanceCutoff, 
	const float excessConst, 
	const float disjointConst, 
	const float weightDiffConst)
	: Evaluator(populationSize, compatibilityDistanceCutoff, excessConst, disjointConst, weightDiffConst)
{
	Benchmarker bench{ "Evaluator Constructor" };
	
	// Create randomizer helpers
	std::random_device rd;
	std::mt19937 gen(rd());
	
	std::uniform_int_distribution<uint16_t> dist(0, 1);

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
	
	bench.stop();
}

EvaluatorXor::EvaluatorXor(std::vector<Genome>&& startingPopulation)
	: Evaluator(startingPopulation.size())
{
	genomes_ = std::move(startingPopulation);
}

float EvaluatorXor::getBestFitness()
{
	float best = 0.0f;

	for (auto& genome : genomes_)
	{
		float fitness = evaluateGenomeTraining(genome);
		if (fitness > best)
			best = fitness;
	}

	return best;
}

std::array<EvaluatorXor::outputInfo, 5> EvaluatorXor::getTop5Info()
{
	std::array<outputInfo, 5> top5{};

	calculateFitnessMap();
	
	for (const auto& [genome, fitness] : fitnessMap_)
	{
		if (fitness > top5[0].fitness)
		{
			top5[4] = top5[3];
			top5[3] = top5[2];
			top5[2] = top5[1];
			top5[1] = top5[0];
			top5[0] = { fitness, evaluateGenomeTest(*genome).correctpercentage, genome->numberOfNodes(), genome->numberOfConnections() };
		}
		else if (fitness > top5[1].fitness)
		{
			top5[4] = top5[3];
			top5[3] = top5[2];
			top5[2] = top5[1];
			top5[1] = { fitness, evaluateGenomeTest(*genome).correctpercentage, genome->numberOfNodes(), genome->numberOfConnections() };
		}
		else if (fitness > top5[2].fitness)
		{
			top5[4] = top5[3];
			top5[3] = top5[2];
			top5[2] = { fitness, evaluateGenomeTest(*genome).correctpercentage, genome->numberOfNodes(), genome->numberOfConnections() };
		}
		else if (fitness > top5[3].fitness)
		{
			top5[4] = top5[3];
			top5[3] = { fitness, evaluateGenomeTest(*genome).correctpercentage, genome->numberOfNodes(), genome->numberOfConnections() };
		}
		else if (fitness > top5[4].fitness)
		{
			top5[4] = { fitness, evaluateGenomeTest(*genome).correctpercentage, genome->numberOfNodes(), genome->numberOfConnections() };
		}
	}
	
	return top5;
}

float EvaluatorXor::evaluateGenomeTraining(Genome& genome)
{
	Benchmarker bench{ "Training Evalutor" };

	float fitness = 0;
	
	for (size_t i = 0; i < param1training.size(); i++)
	{
		auto numOfNodes = genome.numberOfNodes();
		genome.resetCache();
		genome.setInputValues({ static_cast<float>(param1training[i]), static_cast<float>(param2training[i]) });
		genome.evaluateOutputNodes();

		bool answer = param1training[i] ^ param2training[i];
		float distanceFromAnswer = std::abs(answer - genome.getOutputValue(0));
		
		fitness += 200.0f * (1.0f - distanceFromAnswer * distanceFromAnswer)/* - (genome.numberOfNodes() - 5) - (genome.numberOfConnections())*/;
	}
	
	return
	{
		fitness / param1training.size()
	};
}

EvaluatorXor::FitnessCorrectpercentagePair EvaluatorXor::evaluateGenomeTest(Genome& genome)
{
	Benchmarker bench{ "Test Evalutor" };

	float fitness = 0;
	uint32_t correct = 0;

	for (size_t i = 0; i < param1test.size(); i++)
	{
		genome.resetCache();
		genome.setInputValues({ static_cast<float>(param1test[i]), static_cast<float>(param2test[i]) });
		genome.evaluateOutputNodes();

		bool answer = param1test[i] ^ param2test[i];
		float distanceFromAnswer = std::abs(answer - genome.getOutputValue(0));
		
		if (distanceFromAnswer < 0.3f)
			correct++;

		fitness += 200.0f * (1.0f - distanceFromAnswer * distanceFromAnswer)/* - (genome.numberOfNodes() - 4) * 5.0f*/;
	}
	
	//std::cout << static_cast<float>(correct) / param1test.size() << '\n';
	
	return
	{
		fitness / param1test.size(),
		static_cast<float>(correct) / param1test.size()
	};
}
