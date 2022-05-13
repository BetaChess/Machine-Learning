#include <gtest/gtest.h>

#include <evaluator_xor.h>

TEST(XorVerifierTests, VerifySolutionGetsMaxFitness)
{
	// Construct the network
	Genome xorSolver{ 2, 1 };
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

	EvaluatorXor evaluator{ {xorSolver} };
	
	EXPECT_LE(198.0f, evaluator.getBestFitness());
}

