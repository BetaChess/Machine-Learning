#include "evaluator.h"

#include <random>
#include <cassert> 
#include <algorithm>

#include <iostream>


Evaluator::Evaluator(
	size_t populationSize,
	const float compatibilityDistanceCutoff, 
	const float excessConst, 
	const float disjointConst, 
	const float weightDiffConst)
	: compatibilityDistanceCutoff_(compatibilityDistanceCutoff), excessConst_(excessConst), disjointConst_(disjointConst), weightDiffConst_(weightDiffConst)
{
	genomes_.reserve(populationSize);
}

// TODO: Might need to rewrite this a bit. Networks aren't really improving that fast.
void Evaluator::evaluate_training()
{
	// Create random devices
	std::random_device rd;
	std::mt19937 gen(rd());
	
	// Place genomes into species
	speciesMap_.clear();
	for (auto& g : genomes_)
	{
		bool placed = false;
		for (auto& s : species_)
		{
			if (s.mascot.calculateCompatibilityDistance(g, excessConst_, disjointConst_, weightDiffConst_) < compatibilityDistanceCutoff_)
			{
				s.memberGenomes.push_back(&g);
				speciesMap_[&g] = &s;
				placed = true;
				break;
			}
		}
		if (!placed)
		{
			// Create a new species and put it in the species vector, 
			// while also adding the genome and species to the species map.
			speciesMap_[&g] = &species_.emplace_back(&g);
		}
	}
	
	// Check if every species has members, remove any that don't.
	for (size_t i = 0; i < species_.size(); i++)
	{
		if (species_[i].memberGenomes.size() == 0)
		{
			species_.erase(species_.begin() + i);
			i--;
		}
	}
	
	std::cout << species_.size() << '\n';
	
	// Evaluate genomes and assign fitness
	calculateFitnessMap();
	
	// Put the best genomes from each species (with 5 or more Genomes) into the next generation
	std::vector<Genome> nextGenGenomes;
	nextGenGenomes.reserve(genomes_.size());
	// Also, keep a pointer map to each new genome. This will be used for breeding the next generation.
	std::unordered_map<const Species*, std::vector<Genome*>> newGenomeSpeciesMap;
	
	for (auto& s : species_)
	{
		// Sort the species genomes by fitness
		std::sort(s.memberGenomes.begin(), s.memberGenomes.end(), [&](Genome* g1, Genome* g2)
			{
				return fitnessMap_[g1] > fitnessMap_[g2];
			});
		
		if (s.memberGenomes.size() > 4)
		{
			// todo: Maybe move here?
			auto& newGenome = nextGenGenomes.emplace_back(*s.memberGenomes[0]);
			newGenomeSpeciesMap[&s].push_back(&newGenome);
		}
	}
	
	// Put the 50% best of each species into the next generation and mutate them.
	for (auto& s : species_)
	{
		// They should already be sorted here.
		for (size_t i = 1; i < s.memberGenomes.size() / 2; i++)
		{
			auto& newGenome = nextGenGenomes.emplace_back(*s.memberGenomes[i]).mutate();
			newGenomeSpeciesMap[&s].push_back(&newGenome);
		}
	}
	
	// Breed the remaining genomes.
	std::uniform_real_distribution<float> dis0_1(0.0f, 1.0f);
	while (nextGenGenomes.size() < genomes_.size())
	{
		// 0.1% chance to cross-breed between species.
		if (dis0_1(gen) < 0.001f)
		{
			// Get two random species
			auto s1 = getRandomSpeciesBiasedAdjustedFitness();
			auto s2 = getRandomSpeciesBiasedAdjustedFitness();
			
			// Select a random genome from each
			//auto g1 = s1->memberGenomes[std::uniform_int_distribution<size_t>(0, s1->memberGenomes.size() - 1)(gen)];
			//auto g2 = s2->memberGenomes[std::uniform_int_distribution<size_t>(0, s2->memberGenomes.size() - 1)(gen)];
			auto& g1VectorRef = newGenomeSpeciesMap[s1];
			auto g1 = g1VectorRef[std::uniform_int_distribution<size_t>(0, g1VectorRef.size() - 1)(gen)];
			auto& g2VectorRef = newGenomeSpeciesMap[s2];
			auto g2 = g2VectorRef[std::uniform_int_distribution<size_t>(0, g2VectorRef.size() - 1)(gen)];
			
			// Crossover
			if (fitnessMap_[g1] > fitnessMap_[g2])
				nextGenGenomes.emplace_back(*g1, *g2).mutate();
			else
				nextGenGenomes.emplace_back(*g2, *g1).mutate();
		}
		//else if (dis0_1(gen) < 0.25f) 
		//{
		//	// Get a random species and take a genome from the top 25% (excluding the best if there are more than 4), then mutate it and add it to the next generation.
		//	std::uniform_int_distribution<size_t> dis;
		//	auto randomSpecies = getRandomSpeciesBiasedAdjustedFitness();
		//	
		//	if (randomSpecies->memberGenomes.size() > 4)
		//		dis = std::uniform_int_distribution<size_t>(1, static_cast<size_t>(1 + randomSpecies->memberGenomes.size() * 0.25f));
		//	else
		//		dis = std::uniform_int_distribution<size_t>(0, static_cast<size_t>((randomSpecies->memberGenomes.size() - 1) * 0.25f));
		//	
		//	nextGenGenomes.emplace_back(*randomSpecies->memberGenomes[dis(gen)]).mutate();
		//}
		else
		{
			// Breed two genomes from the same species and mutate the offspring.
			auto s = getRandomSpeciesBiasedAdjustedFitness();
			
			auto& memberVecRef = newGenomeSpeciesMap[s];
			std::uniform_int_distribution<size_t> dis(0, memberVecRef.size() - 1);
			auto g1 = memberVecRef[dis(gen)];
			auto g2 = memberVecRef[dis(gen)];
			
			//std::uniform_int_distribution<size_t> dis(0, s->memberGenomes.size() - 1);
			//auto g1 = s->memberGenomes[dis(gen)];
			//auto g2 = s->memberGenomes[dis(gen)];

			// The more fit parent should always be the first parameter.
			if (fitnessMap_[g1] > fitnessMap_[g2])
				nextGenGenomes.emplace_back(*g1, *g2).mutate();
			else
				nextGenGenomes.emplace_back(*g2, *g1).mutate();
		}
	}
	
	// Reset all species
	for (auto& species : species_)
	{
		species.reset();
	}

	genomes_ = std::move(nextGenGenomes);
	for (auto& g : genomes_)
		g.reconnectIncommingPointers();
}

void Evaluator::evaluate_testing()
{
}

void Evaluator::computeAdjustedFitnessSums()
{
	totalAdjustedFitness_ = 0;
	
	for (auto& s : species_)
	{
		assert(s.adjustedFitnessSum == 0 && "Adjusted fitness sum did not start at a value of 0!");
		
		s.generationsSinceLastImprovement++;
		
		for (auto& g : s.memberGenomes)
		{
			auto adjustedFitness = getAdjustedFitness(g);
			
			s.adjustedFitnessSum += adjustedFitness;
			totalAdjustedFitness_ += adjustedFitness;
			if (adjustedFitness > s.maxAdjustedFitness)
			{
				s.maxAdjustedFitness = adjustedFitness;
				s.generationsSinceLastImprovement = 0;
			}
		}
	}
}

const Evaluator::Species* Evaluator::getRandomSpeciesBiasedAdjustedFitness() const
{
	// Create randomizer helpers
	std::random_device rd;
	std::mt19937 gen(rd());
	
	std::uniform_real_distribution<float> dis(0, totalAdjustedFitness_);
	
	// Get random number
	float random = dis(gen);
	
	// Sum the total adjusted fitness of each species, until it is greater than or equal to the random number.
	float sum = 0;
	for (auto& s : species_)
	{
		sum += s.adjustedFitnessSum;
		
		if (sum >= random)
			return &s;
	}
	
	// If this point is reached, something went wrong.
	return nullptr;
}

void Evaluator::calculateFitnessMap()
{
	fitnessMap_.clear();
	for (auto& g : genomes_)
	{
		g.reconnectIncommingPointers();
		float fitness = evaluateGenomeTraining(g);

		fitnessMap_[&g] = fitness;
	}
}

Evaluator::Species::Species(Genome* mascot)
	: mascot{ *mascot }, memberGenomes{ mascot }
{
}

void Evaluator::Species::reset()
{
	// Create randomizer helpers
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<size_t> dis(0ULL, memberGenomes.size() - 1);
	
	// Randomly select a genome from the species and make it the mascot
	mascot = *memberGenomes[dis(gen)];
	
	// Clear the member genomes
	memberGenomes.clear();
	adjustedFitnessSum = 0;
}
