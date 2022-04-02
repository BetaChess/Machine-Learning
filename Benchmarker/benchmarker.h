#ifndef BENCHMARKER_H
#define BENCHMARKER_H

#include <chrono>
#include <string>
#include <unordered_map>

class Benchmarker
{
public:
	Benchmarker(const std::string& benchName);
	~Benchmarker();

	void stop();

	static void printStats();

private:
	struct BenchmarkStats
	{
		size_t count;

		std::chrono::microseconds totalDuration;
		std::chrono::microseconds minDuration;
		std::chrono::microseconds maxDuration;

		inline std::chrono::duration<double, std::micro> getAverageDuration() const { return std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(totalDuration) / count; };
	};

	static std::unordered_map<std::string, BenchmarkStats> benchmarkStats_s;

	const std::string name_;
	bool stopped_ = false;

	std::chrono::steady_clock::time_point startTime_;
};


#endif /* BENCHMARKER_H */