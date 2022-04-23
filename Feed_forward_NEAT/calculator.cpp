#include "calculator.h"

#include <numeric>
#include <benchmarker.h>


neat::Calculator::Calculator(const Genome& genome) : 
	inputCount_c(genome.inputCount_), outputCount_c(genome.outputCount_), hiddenCount_c(genome.numberOfHiddenNodes()), connectionCount_c(genome.connectionGenes_.size()),
	nodeCalculationOrderList_c(getNodeCalculationOrder(genome)),
	nodeCalculationOrderList_individualOutputs_c(getOutnodeFilteredCalculationOrderLists(genome, nodeCalculationOrderList_c)),
	nodeInputs_c(getNodeInputs(genome)),
	values(nodeCount() + 1, -1)
{
	/*const size_t inputBegin = 0;
	const size_t inputEnd = inputCount_c;
	const size_t biasIndex = inputCount_c;
	const size_t outputBegin = biasIndex + 1;
	const size_t outputEnd = outputBegin + outputCount_c;
	const size_t hiddenBegin = outputEnd;
	const size_t hiddenEnd = hiddenBegin + hiddenCount_c;*/
}

std::vector<float> neat::Calculator::calculate(const std::vector<float>& inputs) const
{
	std::vector<float> values = inputs;
	values.resize(nodeCount() + 1);
	const size_t biasIndex = inputCount_c;
	values[biasIndex] = 1.0f;

	for (auto node : nodeCalculationOrderList_c)
	{
		float val = 0;
		for (auto [index, weight] : nodeInputs_c[node])
			val += values[index] * weight;

		values[node] = sigmoid(val);
	}

	const size_t outputBegin = biasIndex + 1;
	return std::vector<float>(values.begin() + outputBegin, values.end());
}

float neat::Calculator::calculateIndex(size_t outputIndex, const std::vector<float>& inputs) const
{
	BENCHMARK_START(CalculateIndex);
	
	std::copy(inputs.begin(), inputs.end(), values.begin());
	const size_t biasIndex = inputCount_c;
	values[biasIndex] = 1.0f;

	for (auto node : nodeCalculationOrderList_individualOutputs_c[outputIndex])
	{
		float val = 0;
		for (auto [index, weight] : nodeInputs_c[node])
			val += values[index] * weight;

		values[node] = sigmoid(val);
	}

	const size_t outputBegin = biasIndex + 1;
	return values[outputBegin + outputIndex];
}

std::vector<size_t> neat::Calculator::getExclusivelyDependentNodes(const Genome& genome, const std::vector<size_t>& dependencies)
{
	std::vector<size_t> retVec;

	for (const auto& node : genome.nodeGenes_)
	{
		if (node.type_ == Genome::NodeGene::NodeType::INPUT)
			continue;
		
		const size_t nodeIndex = node.incomming_[0]->outNode();
		
		[&]()
		{
			for (const auto& connection : node.incomming_)
			{
				const bool found = std::find(dependencies.begin(), dependencies.end(), connection->inNode()) != dependencies.end();
				if (!found)
					return; // Do not add this node
			}

			retVec.push_back(nodeIndex);
		}();
	}

	return retVec;
}

std::vector<size_t> neat::Calculator::getNodeCalculationOrder(const Genome& genome)
{
	std::vector<size_t> retVec;
	
	const size_t finalReturnSize = genome.numberOfNodes() - genome.inputCount_;
	retVec.reserve(finalReturnSize);
	
	// Get the initial input nodes
	std::vector<size_t> dependencies;
	dependencies.resize(genome.inputCount_ + 1);
	std::iota(dependencies.begin(), dependencies.end(), 0);

	// Add nodes to the return vector, until all nodes are added.
	while (retVec.size() < finalReturnSize)
	{
		auto newDependencies = getExclusivelyDependentNodes(genome, dependencies);
		newDependencies.erase(
			std::remove_if(
				newDependencies.begin(), newDependencies.end(), 
				[&](const size_t& nodeIndex)
				{
					return std::find(retVec.begin(), retVec.end(), nodeIndex) != retVec.end();
				}),
				newDependencies.end()
		);

		dependencies.insert(dependencies.end(), newDependencies.begin(), newDependencies.end());
		retVec.insert(retVec.end(), newDependencies.begin(), newDependencies.end());
	}

	return retVec;
}

std::vector<std::vector<size_t>> neat::Calculator::getOutnodeFilteredCalculationOrderLists(const Genome& genome, const std::vector<size_t>& nodeCalculationOrder)
{
	std::vector<std::vector<size_t>> retVec;
	retVec.reserve(genome.outputCount_);

	const size_t inputBegin = 0;
	const size_t inputEnd = genome.inputCount_;
	const size_t biasIndex = genome.inputCount_;
	const size_t outputBegin = biasIndex + 1;
	const size_t outputEnd = outputBegin + genome.outputCount_;

	for (size_t i = outputBegin; i < outputEnd; i++)
	{
		auto initialCalculationOrder = nodeCalculationOrder;
		auto parentNodes = getAllParentNodes(genome, i);
		parentNodes.insert(i);
		
		initialCalculationOrder.erase(
			std::remove_if(
				initialCalculationOrder.begin(), initialCalculationOrder.end(),
				[&](const size_t& nodeIndex)
				{
					return parentNodes.find(nodeIndex) == parentNodes.end();
				}),
			initialCalculationOrder.end()
		);

		retVec.emplace_back(std::move(initialCalculationOrder));
	}

	return retVec;
}

std::unordered_set<size_t> neat::Calculator::getAllParentNodes(const Genome& genome, size_t nodeIndex)
{
	std::unordered_set<size_t> retSet;

	std::unordered_set<size_t> notChecked;
	for (const auto& connection : genome.nodeGenes_[nodeIndex].incomming_)
	{
		notChecked.insert(connection->inNode());
		retSet.insert(connection->inNode());
	}

	while (notChecked.size())
	{
		std::unordered_set<size_t> newNotChecked;

		for (auto index : notChecked)
		{
			for (const auto& connection : genome.nodeGenes_[index].incomming_)
			{
				newNotChecked.insert(connection->inNode());
				retSet.insert(connection->inNode());
			}
		}

		notChecked = std::move(newNotChecked);
	}

	return retSet;
}

std::vector<std::vector<std::pair<size_t, float>>> neat::Calculator::getNodeInputs(const Genome& genome)
{
	std::vector<std::vector<std::pair<size_t, float>>> retVec;
	retVec.resize(genome.numberOfNodes() + 1);

	for (size_t i = genome.numberOfInputNodes() + 1; i < genome.numberOfNodes() + 1; i++)
	{
		retVec.reserve(genome.nodeGenes_[i].incomming_.size());
		for (auto connection : genome.nodeGenes_[i].incomming_)
		{
			retVec[i].push_back({ connection->inNode(), connection->weight() });
		}
	}

	return retVec;
}
