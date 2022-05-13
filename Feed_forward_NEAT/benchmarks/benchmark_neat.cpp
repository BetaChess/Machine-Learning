
#ifdef BENCHMARK_ENABLED
#define MESSAGE "Warning: Running benchmarks inside benchmark tests can skew the results by a significant margin. Consider disabling benchmarking in any benchmarked library. "

#if _MSC_VER
#pragma message (MESSAGE)
#elif   __GNUC__
#warning ("Warning: Running benchmarks inside benchmark tests can skew the results by a significant margin. Consider disabling benchmarking in any benchmarked library. ")
#endif
#endif

#define BENCHMARK_ENABLED

#include <benchmarker.h>
#include <NEAT.h>
#include <calculator.h>
#include <random>
#include <iostream>
#include <array>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __linux__    // Linux only
#include <sched.h>  // sched_setaffinity
#endif

int main(int argc, char** argv)
{
#ifdef _WIN32 
	// Set thread affinity and priority for platform windows
	HANDLE process = GetCurrentProcess();
	DWORD_PTR processAffinityMask = 1<<9; // Use CPU 0

	if (!SetProcessAffinityMask(process, processAffinityMask))
	{
		std::cerr << "Failed to set process affinity mask!" << std::endl;
		return -1;
	}
	
	if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
	{
		std::cerr << "Failed to set process priority!" << std::endl;
	}
#endif

#ifdef __linux__
	int cpuAffinity = argc > 1 ? atoi(argv[1]) : -1;

	if (cpuAffinity > -1)
	{
		cpu_set_t mask;
		int status;

		CPU_ZERO(&mask);
		CPU_SET(cpuAffinity, &mask);
		status = sched_setaffinity(0, sizeof(mask), &mask);
		if (status != 0)
		{
			perror("sched_setaffinity");
		}
	}
#endif

	// Create the randomizer helpers
	std::random_device rd;
	std::mt19937 gen(rd());	

	

	// Test network evaluation speed for known solution to xor.
	{
		std::uniform_int_distribution inputRnd(0, 1);
		volatile bool resultStore; // Prevents compiler from optimizing out the result.

		neat::Genome xorSolver{ 2, 1 };
		xorSolver.addHiddenNode().addHiddenNode();
		xorSolver.addConnectionGene(2, 4, 10.0676f);
		xorSolver.addConnectionGene(2, 3, -4.6458f);
		xorSolver.addConnectionGene(2, 5, 2.8261f);
		xorSolver.addConnectionGene(0, 4, -6.6619f);
		xorSolver.addConnectionGene(4, 3, 9.461f);
		xorSolver.addConnectionGene(0, 5, -5.9874f);
		xorSolver.addConnectionGene(1, 4, -6.3597f);
		xorSolver.addConnectionGene(5, 3, -9.9307f);
		xorSolver.addConnectionGene(1, 5, -9.9025f);

		// Shouldn't be nececary, but just in case.
		xorSolver.resetCache();

		// Create test data
		const uint64_t SAMPLES_PER_RUN = 1000;

		std::vector<std::pair<float, float>> testData;
		testData.resize(SAMPLES_PER_RUN);
		std::for_each(testData.begin(), testData.end(), [&](std::pair<float, float>& data)
		{
			data.first = static_cast<float>(inputRnd(gen));
			data.second = static_cast<float>(inputRnd(gen));
		});

		// Normal evaluation.
		{
			BENCHMARK_START(XOR_evaluation);

			Benchmarker::runNormalTestWriteToFile(200000, "xor_normal.csv", [&]() {
				for (size_t i = 0; i < SAMPLES_PER_RUN; i++)
				{
					xorSolver.setInputValues({ testData[i].first, testData[i].second });
					xorSolver.evaluateOutputNodes();
					resultStore = xorSolver.getOutputValue(0);
					xorSolver.resetCache();
				}
				});
		}

		std::cout << "XOR Normal evaluation complete." << std::endl;

		// Optimized evaluation
		{
			neat::Calculator calculator{ xorSolver };
			BENCHMARK_START(XOR_evaluation_calculator);

			Benchmarker::runNormalTestWriteToFile(200000, "xor_calculatorOptimized.csv", [&]() {
					for (size_t i = 0; i < SAMPLES_PER_RUN; i++)
					{
						resultStore = calculator.calculateIndex(0, { testData[i].first, testData[i].second });
					}
				});
		}

		std::cout << "XOR Calculator evaluation complete." << std::endl;
	}
	
	// Test network evaluation speed for a large randomly generated network.
	{
		std::uniform_real_distribution<float> inputRnd(0.0f, 1.0f);
		const uint64_t inputNodes = 100;
		const uint64_t outputNodes = 20;

		volatile float resultStore; // Prevents compiler from optimizing out the result.

		neat::Genome randomNetwork{ inputNodes, outputNodes };
		while (randomNetwork.numberOfConnections() < 40)
			randomNetwork.addConnectionMutation();
		while (randomNetwork.numberOfHiddenNodes() < 10)
			randomNetwork.addNodeMutation();
		while (randomNetwork.numberOfConnections() < 100)
			randomNetwork.addConnectionMutation();
		while (randomNetwork.numberOfHiddenNodes() < 40)
			randomNetwork.addNodeMutation();
		while (randomNetwork.numberOfConnections() < 400)
			randomNetwork.addConnectionMutation();
		while (randomNetwork.numberOfHiddenNodes() < 100)
			randomNetwork.addNodeMutation();
		while (randomNetwork.numberOfConnections() < 1000)
			randomNetwork.addConnectionMutation();

		// Shouldn't be nececary, but just in case.
		randomNetwork.resetCache();
		
		// Create test data
		const uint64_t SAMPLES_PER_RUN = 1000;
		std::vector<std::array<float, inputNodes>> testData;
		testData.resize(SAMPLES_PER_RUN);
		std::for_each(testData.begin(), testData.end(), [&](std::array<float, inputNodes>& data)
			{
				for (auto& value : data)
					value = static_cast<float>(inputRnd(gen));
			});
		
		// Normal evaluation.
		{
			BENCHMARK_START(Large_random_network_evaluation);

			Benchmarker::runNormalTestWriteToFile(50000, "Large_random_network_evaluation.csv", [&]() {
				for (size_t i = 0; i < SAMPLES_PER_RUN; i++)
				{
					randomNetwork.setInputValues({ testData[i].begin(), testData[i].end() });
					randomNetwork.evaluateOutputNodes();
					auto result = randomNetwork.getOutputValues();
					for (size_t j = 0; j < outputNodes; j++)
						resultStore = result[j];
					randomNetwork.resetCache();
				}
				});
		}

		std::cout << "Large random network evaluation complete." << std::endl;

		// Optimized evaluation
		{
			neat::Calculator calculator{ randomNetwork };
			BENCHMARK_START(Large_random_network_evaluation_calculator);

			Benchmarker::runNormalTestWriteToFile(50000, "Large_random_network_evaluation_calculator.csv", [&]() {
				for (size_t i = 0; i < SAMPLES_PER_RUN; i++)
				{
					auto result = calculator.calculate({ testData[i].begin(), testData[i].end() });
					for (size_t j = 0; j < outputNodes; j++)
						resultStore = result[j];
				}
				});
		}

		std::cout << "Large random network calculator evaluation complete." << std::endl;
	}
	
	Benchmarker::printStats();

	return 0;
}
