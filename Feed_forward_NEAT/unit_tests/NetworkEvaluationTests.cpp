#include <gtest/gtest.h>

#include <vector>

#include <NEAT.h>


TEST(NetworkEvaluationTests, XorEvaluationTest) {
	// Construct the network
	neat::Genome xorSolver{2, 1};
	xorSolver.addHiddenNode().addHiddenNode();
	ASSERT_TRUE(xorSolver.addConnectionGene(2, 4, 10.0676f));
	ASSERT_TRUE(xorSolver.addConnectionGene(2, 3, -4.6458f));
	ASSERT_TRUE(xorSolver.addConnectionGene(2, 5, 2.8261f));
	ASSERT_TRUE(xorSolver.addConnectionGene(0, 4, -6.6619f));
	ASSERT_TRUE(xorSolver.addConnectionGene(4, 3, 9.461f));
	ASSERT_TRUE(xorSolver.addConnectionGene(0, 5, -5.9874f));
	ASSERT_TRUE(xorSolver.addConnectionGene(1, 4, -6.3597f));
	ASSERT_TRUE(xorSolver.addConnectionGene(5, 3, -9.9307f));
	ASSERT_TRUE(xorSolver.addConnectionGene(1, 5, -9.9025f));

	

	// Evaluate the tests
	std::vector<std::pair<bool, bool>> inputs{ {false, false}, {false, true}, {true, false}, {true, true} };

	for (const auto& input : inputs)
	{
		const bool expected = input.first ^ input.second;

		xorSolver.resetCache();
		xorSolver.setInputValues({ 
			static_cast<float>(input.first), 
			static_cast<float>(input.second) 
			});
		xorSolver.evaluateOutputNodes();
		
		float distance = std::abs(xorSolver.getOutputValue(0) - static_cast<float>(expected));
		bool success = distance < 0.001f;
		EXPECT_TRUE(success) << "Network failed to evaluate correctly (distance: " << distance << ") with input : " << input.first << ", " << input.second;
	}
}

