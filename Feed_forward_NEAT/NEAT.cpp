#include "NEAT.h"

#include <unordered_set>
#include <utility>
#include <random>
#include <cassert>

Genome::NodeGene::NodeGene(NodeType type)
	: type_(type)
{
}

Genome::NodeGene::NodeGene(const NodeGene& other)
	: type_(other.type_)
{
}

inline void Genome::NodeGene::evaluate(std::vector<NodeGene>& nodes)
{
	value_ = 0; 
	for (auto conn : incomming_)
	{
		if (!conn->expressed_)
			continue;
		
		value_ += conn->weight_ * nodes[conn->inNode_].getValue(nodes);
	}
	
	value_ = 1.0f / (1.0f + std::exp(-4.9f * value_));
	
	cached_ = true;
}


Genome::ConnectionGene::ConnectionGene()
	: inNode_(0), outNode_(0), weight_(0), expressed_(false), innovationNumber_(0)
{
}

Genome::ConnectionGene::ConnectionGene(uint64_t inNode, uint64_t outNode, float weight, bool expressed, uint64_t innovationNumber)
	: inNode_(inNode), outNode_(outNode), weight_(weight), expressed_(expressed), innovationNumber_(innovationNumber)
{
}

uint64_t Genome::ConnectionGene::currentInnovationNumber_s = 0;



std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, hashPair> Genome::existingInnovations_s{};

Genome::Genome()
{
}

Genome::Genome(size_t inputCount, size_t outputCount)
{
	for (size_t i = 0; i < inputCount; i++)
	{
		nodeGenes_.emplace_back(Genome::NodeGene::NodeType::INPUT);
		inputNodes_.push_back(&nodeGenes_[nodeGenes_.size() - 1]);
	}
	nodeGenes_.emplace_back(Genome::NodeGene::NodeType::INPUT);
	inputNodes_.push_back(&nodeGenes_[nodeGenes_.size() - 1]);
	
	
	for (size_t i = 0; i < outputCount; i++)
	{
		nodeGenes_.emplace_back(Genome::NodeGene::NodeType::OUTPUT);
		outputNodes_.push_back(&nodeGenes_[nodeGenes_.size() - 1]);
	}
}

Genome::Genome(const Genome& genomeToCopy)
{
	// Copy the node genes
	for (const auto& node : genomeToCopy.nodeGenes_)
	{
		nodeGenes_.emplace_back(node);
	}
	
	for (auto& node : nodeGenes_)
	{
		if (node.type_ == NodeGene::NodeType::INPUT)
			inputNodes_.push_back(&node);
		else if (node.type_ == NodeGene::NodeType::OUTPUT)
			outputNodes_.push_back(&node);
	}

	// Copy the connection genes
	for (const auto&[innovationNum, connection] : genomeToCopy.connectionGenes_)
	{
		addConnectionGene_assumeSafe(connection);
	}
}

Genome::Genome(const Genome& parent1, const Genome& parent2)
{
	// Create the random number generator
	std::random_device rd;
	std::mt19937 gen(rd());
	
	// Add all nodes from parent1
	for (const auto& node : parent1.nodeGenes_)
	{
		nodeGenes_.push_back(node);
		
		if (node.type_ == NodeGene::NodeType::INPUT)
		{
			inputNodes_.push_back(&nodeGenes_[nodeGenes_.size() - 1]);
		}
		else if (node.type_ == NodeGene::NodeType::OUTPUT)
		{
			outputNodes_.push_back(&nodeGenes_[nodeGenes_.size() - 1]);
		}
	}
	
	// Go through each connection gene in parent1 and see if it also exists in parent2.
	// If it does, add it to the genome.
	// If it doesn't, add the excess/disjoint gene from parent1 (the more fit parent).
	for (const auto&[innovationNum, connection] : parent1.connectionGenes_)
	{
		if (parent2.connectionGenes_.find(innovationNum) != parent2.connectionGenes_.end())
		{
			if (std::uniform_int_distribution(0, 1)(gen))
			{
				connectionGenes_[innovationNum] = connection;
			}
			else
			{
				connectionGenes_[innovationNum] = parent2.connectionGenes_.at(innovationNum);
			}
			
			// If the gene is disabled in either parent, there is a 75% chance that it will also be disabled in the child.
			if (!parent2.connectionGenes_.at(innovationNum).expressed_ || !connection.expressed_)
			{
				if (std::uniform_real_distribution<float>(0.0f, 1.0f)(gen) < 0.75f)
					connectionGenes_[innovationNum].disable();
			}
		}
		else
		{
			// Add the excess/disjoint gene from parent1
			connectionGenes_[innovationNum] = connection;
		}
	}
}

/// <summary>
/// Mutates the network according to values found in: http://nn.cs.utexas.edu/downloads/papers/stanley.ec02.pdf
/// </summary>
void Genome::mutate(float mutateWeightChance, float mutateAddNodeChance, float mutateAddConnectionChance)
{
	// Create the random number generator
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<float> realDist(0.0f, 1.0f);
	
	// There is an 80% chance (by default) that the weights are mutated
	if (realDist(gen) < mutateWeightChance)
	{
		mutateConnectionGenes();
	}
	// A 3% chance (by default) of adding a new node
	if (realDist(gen) < mutateAddNodeChance)
	{
		addNodeMutation();
	}
	// A 5% chance (by default) of adding a new connection
	if (realDist(gen) < mutateAddConnectionChance)
	{
		addConnectionMutation();
	}
}

void Genome::addConnectionMutation()
{
	assert(nodeGenes_.size() > 0 && "There are no node genes to add connections to!");
	assert(inputNodes_.size() > 0 && "There are no input nodes!");
	assert(outputNodes_.size() > 0 && "There are no output nodes!");
	
	// Create the randomizer helpers
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<uint64_t> randomGen{ 0, nodeGenes_.size() - 1 };
	std::uniform_real_distribution<float> randomFloatGen{ -2.0f, 2.0f };

	// Determine in/out nodes
	uint64_t node1 = randomGen(gen);
	uint64_t node2 = randomGen(gen);
	while (
		node1 == node2 ||
		(nodeGenes_[node1].type() == NodeGene::NodeType::INPUT && nodeGenes_[node2].type() == NodeGene::NodeType::INPUT) ||
		(nodeGenes_[node1].type() == NodeGene::NodeType::OUTPUT && nodeGenes_[node2].type() == NodeGene::NodeType::OUTPUT)
		)
	{
		node2 = randomGen(gen);
	}
	
	// Reverse connection if needed
	{
		bool reversed = false; // connection goes from node 1 to 2 by default, but this might not be correct.
		if (nodeGenes_[node2].type() == NodeGene::NodeType::INPUT)
			reversed = true;
		else if (nodeGenes_[node1].type() == NodeGene::NodeType::OUTPUT)
			reversed = true;

		if (reversed)
		{
			uint64_t temp = node1;
			node1 = node2;
			node2 = temp;
		}
	}
	
	// Check if the connection already exists
	auto existsIt = existingInnovations_s.find({ node1, node2 });
	if (existsIt == existingInnovations_s.end())
	{
		// Connection does not exist
		addConnectionGene(node1, node2, randomFloatGen(gen));
	}
	else
	{
		// Connection exists in the innovation space. Check if it also exists withing this genome.
		if (connectionGenes_.find((*existsIt).second) != connectionGenes_.end())
		{
			// Connection exists in this genome.
			return;
		}
		else
		{
			// Connection exists in the innovation space, but not in this genome.
			// Add the connection to this genome.
			connectionGenes_.emplace((*existsIt).second, ConnectionGene(node1, node2, randomFloatGen(gen), true, (*existsIt).second));
			return;
		}
			
	}
}

void Genome::addNodeMutation()
{
	assert(connectionGenes_.size() != 0 && "There are no connection genes to split!");
	
	// Create the randomizer helpers
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<uint64_t> randomGen{ 0, connectionGenes_.size() - 1 };

	// Determine the connection to split
	size_t geneOffset = randomGen(gen);
	auto it = connectionGenes_.begin();
	for (size_t i = 0; i < geneOffset; i++)
		it++;
	ConnectionGene& connection = (*it).second;
	
	// Split the connection
	connection.disable();

	uint64_t node1 = connection.inNode();
	uint64_t node2 = connection.outNode();
	
	// Create the new node
	nodeGenes_.emplace_back(NodeGene::NodeType::HIDDEN);
	
	addConnectionGene_assumeSafe(node1, nodeGenes_.size() - 1, 1.0f);
	addConnectionGene_assumeSafe(nodeGenes_.size() - 1, node2, connection.weight_);
}

/// <summary>
/// Mutates every connection in the genome.
/// </summary>
void Genome::mutateConnectionGenes()
{
	// Create the randomizer helpers
	std::random_device rd;
	std::mt19937 gen(rd());
	
	std::uniform_real_distribution<float> randomFloatGen{ -2.0f, 2.0f };
	
	// Mutate all connection genes
	for (auto& connection : connectionGenes_)
	{
		// 10% chance to reset the weight
		if (randomFloatGen(gen) < 0.1f)
		{
			connection.second.weight_ = randomFloatGen(gen);
		}
		else // 90% change to perturb the weight
		{
			connection.second.weight_ *= randomFloatGen(gen);
		}
	}
}

void Genome::addHiddenNode()
{
	nodeGenes_.push_back(NodeGene::NodeType::HIDDEN);
}

/// <summary>
/// Adds a connection gene to the network, but checks whether the gene will create an infinite loop (this should be a feed-forward network. No looping connections).
/// </summary>
/// <returns>True if the connection was added. False if the connection was not added, due to it creating a loop. </returns>
bool Genome::addConnectionGene(uint64_t inNode, uint64_t outNode, float weight, bool expressed)
{
	{
		std::unordered_set<uint64_t> traversedNodes { inNode };
		std::unordered_set<uint64_t> newNodes = { inNode };
		
		while (newNodes.size())
		{
			std::unordered_set<uint64_t> temp;
			
			for (uint64_t node : newNodes)
			{
				for (const auto& connGene : nodeGenes_[node].incomming_)
				{
					traversedNodes.insert(connGene->inNode_);
					temp.insert(connGene->inNode_);
				}
			}
			
			newNodes = temp;
		}
		
		if (traversedNodes.find(outNode) != traversedNodes.end())
			return false;
	}
	
	addConnectionGene_assumeSafe(inNode, outNode, weight, expressed);
	
	return true;
}

/// <summary>
/// Adds a connection gene to the network. This method assumes that the connection gene won't create an infinite loop. 
/// </summary>
void Genome::addConnectionGene_assumeSafe(uint64_t inNode, uint64_t outNode, float weight, bool expressed)
{
	uint64_t connectionInnovationNumber;
	
	if (existingInnovations_s.find({ inNode, outNode }) != existingInnovations_s.end())
		connectionInnovationNumber = existingInnovations_s[{inNode, outNode}];
	else
		connectionInnovationNumber = ConnectionGene::currentInnovationNumber_s++;
	
	connectionGenes_[connectionInnovationNumber] = { inNode, outNode, weight, expressed, connectionInnovationNumber };
	nodeGenes_[outNode].incomming_.push_back(&connectionGenes_[connectionInnovationNumber]);
}

void Genome::addConnectionGene_assumeSafe(const ConnectionGene& gene)
{
	connectionGenes_[gene.innovationNumber_] = { gene.inNode_, gene.outNode_, gene.weight_, gene.expressed_, gene.innovationNumber_ };
	nodeGenes_[gene.outNode_].incomming_.push_back(&connectionGenes_[gene.innovationNumber_]);
}

/// <summary>
/// Calculates the compatibility distance (delta) between this Genome and another
/// </summary>
float Genome::calculateCompatibilityDistance(const Genome& other, const float excessConst, const float disjointConst, const float weightDiffConst) const
{
	uint64_t largestInnovationNum = 0;
	
	// Find the largest innovation number in this genome
	for (const auto& [innovationNum, gene] : connectionGenes_)
		largestInnovationNum = std::max(innovationNum, largestInnovationNum);
	
	float weightDifference = 0;
	size_t matchingGeneCount = 0, disjointGeneCount = 0, excessGeneCount = 0;
	// Go through each gene in this genome, and find any disjoint and matching genes.
	for (const auto& [innovationNum, gene] : connectionGenes_)
	{
		if (other.connectionGenes_.find(innovationNum) != other.connectionGenes_.end()) // Matching gene
		{
			weightDifference += std::abs(gene.weight_ - other.connectionGenes_.at(innovationNum).weight_);
			matchingGeneCount++;
		}
		else // Disjoint gene
		{
			disjointGeneCount++;
		}
	}
	
	// Go through each gene in the other genome, and find any excess genes
	for (const auto& [innovationNum, gene] : other.connectionGenes_)
	{
		if (connectionGenes_.find(innovationNum) == connectionGenes_.end()) // not a matching gene
		{
			if (innovationNum < largestInnovationNum)
				disjointGeneCount++;
			else
				excessGeneCount++;
		}
	}
	
	// Calculate the compatibility distance
	size_t N = std::max(connectionGenes_.size(), other.connectionGenes_.size());
	return
		(excessConst	* excessGeneCount) / N +
		(disjointConst	* disjointGeneCount) / N +
		weightDiffConst * (weightDifference / matchingGeneCount);
}

/// <summary>
/// Determines whether a connection gene is present in the Genome
/// </summary>
/// <returns>A boolean to indicate whether it was found and a reference to the gene, if it was found. If the gene was not found, the reference simply points to the input parameter. </returns>
std::pair<bool, const Genome::ConnectionGene&> Genome::hasConnection_get(const ConnectionGene& gene) const
{
	auto geneIt = connectionGenes_.find(gene.innovationNumber_);
	if (geneIt == connectionGenes_.end())
		return { false, gene };
	return { true, geneIt->second };
}

/// <summary>
/// Determines whether a connection gene is present in the Genome
/// </summary>
bool Genome::hasConnection(const ConnectionGene& gene) const
{
	auto geneIt = connectionGenes_.find(gene.innovationNumber_);
	return geneIt != connectionGenes_.end();
}

/// <summary>
/// Resets the network cache. SHOULD ALMOST ALWAYS BE CALLED BEFORE "evaluateOutputNodes"!
/// </summary>
void Genome::resetCache()
{
	for (auto& node : nodeGenes_)
		node.resetCache();
}

void Genome::setInputValues(const std::vector<float>& values)
{
	// Remember, one input node is the bias (the last one)
	assert(values.size() == inputNodes_.size() - 1 && "Number of input nodes doesn't match number of values given!");
	
	for (size_t i = 0; i < values.size(); i++)
	{
		inputNodes_[i]->value_ = values[i];
		inputNodes_[i]->cached_ = true;
	}
	
	// Set the bias node's value
	inputNodes_[inputNodes_.size() - 1]->value_ = 1.0f;
	inputNodes_[inputNodes_.size() - 1]->cached_ = true;
}

void Genome::evaluateOutputNodes()
{
	for (auto& node : outputNodes_)
		node->evaluate(nodeGenes_);
}

/// <summary>
/// Gets a specific output value. evaluateOutputNodes MUST be called first!
/// </summary>
float Genome::getOutputValue(size_t outputIndex)
{
	assert(outputIndex < outputNodes_.size() && "Tried retrieving output that doesn't exist!");
	assert(outputNodes_[outputIndex]->cached_ && "Tried retrieving output that hasn't been calculated yet!");
	
	return outputNodes_[outputIndex]->value_;
}

/// <summary>
/// Gets all output values. evaluateOutputNodes MUST be called first!
/// </summary>
std::vector<float> Genome::getOutputValues()
{
	std::vector<float> returnValues;
	returnValues.reserve(outputNodes_.size());
	
	for (auto& node : outputNodes_)
	{
		assert(node->cached_ && "Tried retrieving output that hasn't been calculated yet!");
		
		returnValues.push_back(node->value_);
	}
	
	
	return returnValues;
}


