#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "NEAT.h"

#include <unordered_set>

namespace neat
{
	class Calculator
	{
	public:
		Calculator() = delete;
		Calculator(const Genome& genome);
		
		[[nodiscard]] std::vector<float> calculate(const std::vector<float>& inputs) const;
		[[nodiscard]] float calculateIndex(size_t outputIndex, const std::vector<float>& inputs) const;

		[[nodiscard]] inline constexpr uint64_t inputCount() const { return inputCount_c; };
		[[nodiscard]] inline constexpr uint64_t outputCount() const { return outputCount_c; };
		[[nodiscard]] inline constexpr uint64_t hiddenCount() const { return hiddenCount_c; };
		// Does not include the bias "node".
		[[nodiscard]] inline constexpr uint64_t nodeCount() const { return inputCount_c + outputCount_c + hiddenCount_c; };
		[[nodiscard]] inline constexpr uint64_t connectionCount() const { return connectionCount_c; };
		
	private:
		const uint64_t inputCount_c;
		const uint64_t outputCount_c;
		const uint64_t hiddenCount_c;
		const uint64_t connectionCount_c;

		// The order that nodes should be calculated in (input to output) to calculate all outputs (inputs are not included).
		const std::vector<size_t> nodeCalculationOrderList_c;
		// The order that nodes should be calculated in (input to output) to calculate a specific output (nodeCalculationOrderListIndividualOutputs_c[i], gives the order for the i'th output).
		const std::vector<std::vector<size_t>> nodeCalculationOrderList_individualOutputs_c;
		// A list of each nodes input nodes and their associated weights.
		const std::vector<std::vector<std::pair<size_t, float>>> nodeInputs_c;

		// Only used for calculating the output. It's not part of the calculator itself (therefore mutable), but constructing the vector is very expensive, so it's much faster to only do it once, at object creation.
		mutable std::vector<float> values;

		/// <summary>
		/// Generates a list of all nodes that solely depend on the dependencies (as direct input).
		/// </summary>
		[[nodiscard]] static std::vector<size_t> getExclusivelyDependentNodes(const Genome& genome, const std::vector<size_t>& dependencies);

		/// <summary>
		/// Generates the nodeCalculationOrderList (The order that nodes should be calculated in (input to output) to calculate all outputs (inputs are not included).)
		/// </summary>
		[[nodiscard]] static std::vector<size_t> getNodeCalculationOrder(const Genome& genome);

		/// <summary>
		/// Generates the nodeCalculationOrderList_individualOutputs.
		/// </summary>
		[[nodiscard]] static std::vector<std::vector<size_t>> getOutnodeFilteredCalculationOrderLists(const Genome& genome, const std::vector<size_t>& nodeCalculationOrder);

		/// <summary>
		/// Get all parent node indices.
		/// </summary>
		[[nodiscard]] static std::unordered_set<size_t> getAllParentNodes(const Genome& genome, size_t nodeIndex);

		/// <summary>
		/// Generates the vector of each nodes inputs
		/// </summary>
		[[nodiscard]] static std::vector<std::vector<std::pair<size_t, float>>> getNodeInputs(const Genome& genome);
	};

}


#endif /* CALCULATOR_H */