#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "NEAT.h"

#include <unordered_map>
#include <memory>

class Evaluator
{
public:
	struct FitnessCorrectpercentagePair
	{
		float fitness;
		float correctpercentage;
	};
	
	struct Species
	{
		Genome mascot;
		std::vector<Genome*> memberGenomes{};
		
		float adjustedFitnessSum = 0;
		
		float maxAdjustedFitness = 0;
		size_t generationsSinceLastImprovement = 0;
		
		// Constructors
		Species(Genome* mascot);
		
		// Public methods
		void reset();
	};
	
public:
	explicit Evaluator(size_t populationSize, const float compatibilityDistanceCutoff = 3.0f, const float excessConst = 1.0f, const float disjointConst = 1.0f, const float weightDiffConst = 0.4f);

	// Public methods
	void evaluate_training();
	void evaluate_testing();

	// Getters
	[[nodiscard]] inline size_t populationSize() const { return genomes_.size(); };

private:
	const float compatibilityDistanceCutoff_;
	const float excessConst_;
	const float disjointConst_;
	const float weightDiffConst_;
	
	
	std::vector<Species> species_{};
	std::unordered_map<Genome*, Species*> speciesMap_{};
	
	float totalAdjustedFitness_ = 0;
	
	// Private methods
	void computeAdjustedFitnessSums();
	
	[[nodiscard]] inline float getAdjustedFitness(Genome* genome) const { return fitnessMap_.at(genome) / speciesMap_.at(genome)->memberGenomes.size(); };
	[[nodiscard]] const Species* getRandomSpeciesBiasedAdjustedFitness() const;
	
protected:
	std::vector<Genome> genomes_{};
	std::unordered_map<Genome*, float> fitnessMap_{};

	void calculateFitnessMap();
	
	[[nodiscard]] virtual float evaluateGenomeTraining(Genome& genome) = 0;
	[[nodiscard]] virtual FitnessCorrectpercentagePair evaluateGenomeTest(Genome& genome) = 0;
};



#endif /* EVALUATOR_H */
