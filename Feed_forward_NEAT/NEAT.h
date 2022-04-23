#ifndef NEAT_H
#define NEAT_H

#include <vector>
#include <unordered_map>

struct hashPair
{
	inline size_t operator()(const std::pair<uint64_t, uint64_t>& pair) const
	{
		return std::hash<uint64_t>()(pair.first) ^ std::hash<uint64_t>()(pair.second);
	}
};

namespace neat
{
	[[nodiscard]] inline float sigmoid(const float x, const float modifier = -4.9) { return 1.0f / (1.0f + std::exp(modifier * x)); };

	class Calculator;

	/// <summary>
	/// A feed-forward NEAT neural network.
	/// </summary>
	class Genome
	{
	public:
		class ConnectionGene;
		class NodeGene
		{
		public:
			enum class NodeType
			{
				INPUT,
				HIDDEN,
				OUTPUT
			};

			NodeGene(NodeType type);
			NodeGene(const NodeGene& other);

			// public methods
			inline NodeType type() const { return type_; };

			inline void evaluate(std::vector<NodeGene>& nodes);
			[[nodiscard]] inline float getValue(std::vector<NodeGene>& nodes) { if (cached_) return value_; else evaluate(nodes); return value_; }

			inline void resetCache() { cached_ = false; };

		private:
			NodeType type_;

			float value_ = 0;
			bool cached_ = false;

			// Only used for evaluating the genome.
			std::vector<ConnectionGene*> incomming_;

			// Friends
			friend Genome;
			friend Calculator;
		};

		class ConnectionGene
		{
		public:
			ConnectionGene();
			ConnectionGene(uint64_t inNode, uint64_t outNode, float weight, bool expressed, uint64_t innovationNumber);

			[[nodiscard]] inline bool isExpressed() const { return expressed_; };
			[[nodiscard]] inline uint64_t inNode() const { return inNode_; };
			[[nodiscard]] inline uint64_t outNode() const { return outNode_; };
			[[nodiscard]] inline float weight() const { return weight_; };

			inline void setExpressed(bool expressed) { expressed_ = expressed; };
			inline void disable() { expressed_ = false; };
			inline void enable() { expressed_ = true; };

			[[nodiscard]] inline bool operator==(const ConnectionGene& other) const
			{
				return innovationNumber_ == other.innovationNumber_;
			}

		private:
			static uint64_t currentInnovationNumber_s;

			uint64_t inNode_;
			uint64_t outNode_;
			float weight_;
			bool expressed_;
			uint64_t innovationNumber_;

			// Friends
			friend Genome;
		};

	public:

		// Constructors
		Genome();
		Genome(size_t inputCount, size_t outputCount);
		Genome(const Genome& genomeToCopy);
		Genome(const Genome& parent1, const Genome& parent2);

		// Public methods
		Genome& mutate(float mutateWeightChance = 0.8f, float mutateAddNodeChance = 0.03f, float mutateAddConnectionChance = 0.05f);
		Genome& addConnectionMutation();
		Genome& addNodeMutation();
		Genome& mutateConnectionGenes();

		Genome& addHiddenNode();
		bool addConnectionGene(uint64_t inNode, uint64_t outNode, float weight, bool expressed = true);

		[[nodiscard]] float calculateCompatibilityDistance(const Genome& other, const float excessConst, const float disjointConst, const float weightDiffConst) const;

		[[nodiscard]] std::pair<bool, const ConnectionGene&> hasConnection_get(const ConnectionGene& gene) const;
		[[nodiscard]] bool hasConnection(const ConnectionGene& gene) const;

		void resetCache();
		void setInputValues(const std::vector<float>& values);
		void evaluateOutputNodes();
		float getOutputValue(size_t outputIndex);
		std::vector<float> getOutputValues();

		// Does not include the bias "node".
		[[nodiscard]] inline uint64_t numberOfNodes() const { return nodeGenes_.size() - 1; };
		[[nodiscard]] inline uint64_t numberOfInputNodes() const { return inputCount_; };
		[[nodiscard]] inline uint64_t numberOfOutputNodes() const { return outputCount_; };
		[[nodiscard]] inline uint64_t numberOfHiddenNodes() const { return numberOfNodes() - numberOfInputNodes() - numberOfOutputNodes(); };
		[[nodiscard]] inline uint64_t numberOfConnections() const { return connectionGenes_.size(); };

		void reconnectIncommingPointers();

	private:
		static std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, hashPair> existingInnovations_s;

		std::vector<NodeGene> nodeGenes_{};
		uint64_t inputCount_ = 0;
		uint64_t outputCount_ = 0;
		//std::vector<NodeGene*> inputNodes_{};
		//std::vector<NodeGene*> outputNodes_{};

		std::unordered_map<uint64_t, ConnectionGene> connectionGenes_{};

		// Private methods
		void addConnectionGene_assumeSafe(uint64_t inNode, uint64_t outNode, float weight, bool expressed = true);
		void addConnectionGene_assumeSafe(const ConnectionGene& gene);

		friend Calculator;
	};

};

#endif /* NEAT_H */