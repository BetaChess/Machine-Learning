#ifndef BENCHMARKER_H
#define BENCHMARKER_H

#include <chrono>
#include <string>
#include <unordered_map>
#include <functional>

#define _STR2(x) #x
#define _STR1(x) _STR2(x)
 
// Macros
#ifdef BENCHMARK_ENABLED
#define BENCHMARK_START(name) Benchmarker name(#name "_in_" __FILE__ ":" _STR1(__LINE__));
#define BENCHMARK_END(name) name.stop();
#define BENCHMARK_END_GET_STATS(name) name.stop_getStats();
#else
#define BENCHMARK_START(name)
#define BENCHMARK_END(name)
#define BENCHMARK_END_GET_STATS(name)
#endif


class Benchmarker
{
public:
	struct BenchmarkStats
	{
		size_t count;

		std::chrono::microseconds totalDuration;
		std::chrono::microseconds minDuration;
		std::chrono::microseconds maxDuration;

		BenchmarkStats() : count(0), totalDuration(0), minDuration(0), maxDuration(0) {};
		BenchmarkStats(std::chrono::microseconds duration) : count(1), totalDuration(duration), minDuration(duration), maxDuration(duration) {};
		BenchmarkStats(size_t count, std::chrono::microseconds totalDuration, std::chrono::microseconds minDuration, std::chrono::microseconds maxDuration) : count(count), totalDuration(totalDuration), minDuration(minDuration), maxDuration(maxDuration) {};

		[[nodiscard]] inline std::chrono::duration<double, std::micro> getAverageDuration() const { return std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(totalDuration) / count; };
		
		BenchmarkStats& operator+=(const BenchmarkStats& other);
	};

	Benchmarker(const std::string& benchName);
	~Benchmarker();

	void stop();
	BenchmarkStats stop_getStats();

	[[nodiscard]] inline static std::unordered_map<std::string, BenchmarkStats> getAllStats() { return benchmarkStats_s; };

	/// <summary>
	/// Prints the stats to the console.
	/// </summary>
	static void printStats();
	/// <summary>
	/// Writes the stats to a file with the specified name.
	/// Format is: BenchmarkNAME, Samples, Total Runtime, Min, Max, Average
	/// </summary>
	static void writeStatsToFile(const std::string& fileName = "benchmark.csv");
	/// <summary>
	/// Runs a function to benchmark a specified number of times.
	/// Writes the result to a file with the specified name. The format is simply the number of samples, followed 
	/// by a newline character, followed by the runtime (in microseconds) of each sample seperated by a comma (no space padding).
	/// </summary>
	static void runNormalTestWriteToFile(size_t sampleCount, std::string fileName, std::function<void()> functionToBenchmark);

private:
	static std::unordered_map<std::string, BenchmarkStats> benchmarkStats_s;

	const std::string name_;
	bool stopped_ = false;

	std::chrono::steady_clock::time_point startTime_;
};


#endif /* BENCHMARKER_H */