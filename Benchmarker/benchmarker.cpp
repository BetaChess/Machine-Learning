#include "benchmarker.h"

#include <iostream>
#include <fstream>
#include <cassert>


Benchmarker::BenchmarkStats& Benchmarker::BenchmarkStats::operator+=(const Benchmarker::BenchmarkStats& other)
{
	count += other.count;
	totalDuration += other.totalDuration;
	minDuration = std::min(minDuration, other.minDuration);
	maxDuration = std::max(maxDuration, other.maxDuration);
	return *this;
}

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

Benchmarker::BenchmarkStats Benchmarker::stop_getStats()
{
	auto stopTime = std::chrono::steady_clock::now();
	stopped_ = true;

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime_);
	return { duration };

	
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

void Benchmarker::writeStatsToFile(const std::string& fileName)
{
	std::ofstream outFile{ fileName };
	if (!outFile.is_open())
	{
		std::cerr << "Failed to open file (in benchmarker.cpp): " << fileName << '\n';
		return;
	}
	
	for (auto& [name, stats] : benchmarkStats_s)
	{
		outFile 
			<< name << ','
			<< stats.count << ','
			<< stats.totalDuration.count() << ','
			<< stats.minDuration.count() << ','
			<< stats.maxDuration.count() << ','
			<< stats.getAverageDuration().count() << '\n';
	}

	return;
}

void Benchmarker::runNormalTestWriteToFile(size_t sampleCount, std::string fileName, std::function<void()> functionToBenchmark)
{
	assert(sampleCount > 0 && "Sample count must be greater than 0!");
	assert(fileName != "" && "File name cannot be an empty string!");
	assert(functionToBenchmark != nullptr && "Function to benchmark must not be null!");

	std::ofstream outFile{ fileName };
	if (!outFile.is_open())
	{
		std::cerr << "Failed to open file (in benchmarker.cpp): " << fileName << '\n';
		return;
	}	

	auto bench = Benchmarker{ "NormalTest" };
	functionToBenchmark();
	auto stats = bench.stop_getStats();
	outFile << stats.totalDuration.count();
	for (size_t i = 1; i < sampleCount; i++)
	{
		auto bench = Benchmarker{ "NormalTest" };
		functionToBenchmark();
		auto stats = bench.stop_getStats();
		outFile << ',' << stats.totalDuration.count();
	}
}

