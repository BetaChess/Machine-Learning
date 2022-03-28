#include "evaluator_xor.h"

#include <iostream>


int main()
{
	EvaluatorXor evaluator{ 150 };
	
	for (size_t i = 0; i < 1000; i++)
	{
		evaluator.evaluate_training();
	}
	
	auto top5 = evaluator.getTop5Info();
	
	for (size_t i = 0; i < top5.size(); i++)
	{
		std::cout << "Network " << i << ": fitness=" << top5[i].fitness << ", nodes=" << top5[i].numberOfNodes << ", connections=" << top5[i].numberOfConnections << '\n';
	}
	
	return 0;
}