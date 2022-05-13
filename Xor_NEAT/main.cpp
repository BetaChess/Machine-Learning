#include "evaluator_xor.h"

#include <benchmarker.h>

#include <iostream>


int main()
{
	EvaluatorXor evaluator{ 150 };

	
	for (size_t i = 0; i < 100; i++)
	{
		evaluator.evaluate_training();
		
		std::cout << "Generation: " << i << '\n';
		auto top5 = evaluator.getTop5Info();
		for (size_t i = 0; i < top5.size(); i++)
		{
			std::cout << "Network " << i << ": fitness=" << top5[i].fitness << ", nodes=" << top5[i].numberOfNodes << ", connections=" << top5[i].numberOfConnections << ", correct rate=" << top5[i].correctPercentage << '\n';
		}
		std::cout << "\n\n";
	}
	
	auto top5 = evaluator.getTop5Info();
	for (size_t i = 0; i < top5.size(); i++)
	{
		std::cout << "Network " << i << ": fitness=" << top5[i].fitness << ", nodes=" << top5[i].numberOfNodes << ", connections=" << top5[i].numberOfConnections << ", correct rate=" << top5[i].correctPercentage << '\n';
	}
	
	Benchmarker::printStats();

	return 0;
}