#include "benchmarker.h"

#include <iostream>


std::unordered_map<std::string, Benchmarker::BenchmarkStats> Benchmarker::benchmarkStats_s{};

Benchmarker::Benchmarker(const std::string& benchName)
	: name_(benchName)
{
	// This is only done here to guarentee that this is the last thing that is defined.
	// Basically to protect against any reordering of variables in the class.
	startTime_ = std::chrono::steady_clock::now();
}

Benchmarker::~Benchmarker()
{
	stop();
}

void Benchmarker::stop()
{
	auto stopTime = std::chrono::steady_clock::now();
	
	if (stopped_)
		return;
	
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime_);

	if (benchmarkStats_s.find(name_) == benchmarkStats_s.end())
		benchmarkStats_s[name_] = BenchmarkStats{ 1, duration, duration, duration };
	else
	{
		auto& stats = benchmarkStats_s[name_];
		stats.count++;
		stats.totalDuration += duration;
		stats.minDuration = std::min(stats.minDuration, duration);
		stats.maxDuration = std::max(stats.maxDuration, duration);
	}

	stopped_ = true;
}

void Benchmarker::printStats()
{
	std::cout << "BENCHMARKER STATS:\n";
	for (auto& [name, stats] : benchmarkStats_s)
	{
		std::cout << "#####################################\n";
		std::cout << "Benchmark name:     " << name << '\n';
		std::cout << "Number of samples:  " << stats.count << '\n';
		std::cout << "Total runtime:      " << stats.totalDuration.count() << '\n';
		std::cout << "Lowest exec. time:  \033[32m" << stats.minDuration.count() << "\033[0m\n";
		std::cout << "Highest exec. time: \033[31m" << stats.maxDuration.count() << "\033[0m\n";
		std::cout << "Avg. exec. time:    \033[35m" << stats.getAverageDuration().count() << "\033[0m\n";
	}
}
