#include "VectorOperations.h"
#include <numeric>
#include <functional>
#include <thread>
#include <future>
#include <mutex>
#include <omp.h>

// Method (1) Simple summation
double SimpleVectorOperations::sum1(bool Time) const {
	Timer timeit(Time);
	double sum = 0;
	for (const auto& val : vec)
		sum += val;
	return sum;
}
// Method (2) std::acumulate summation
double SimpleVectorOperations::sum2(bool Time) const {
	Timer timeit(Time);
	double sum = 0;
	sum = std::accumulate(vec.begin(), vec.end(), 0.0);
	return sum;
}
// Method (3) compensated summation
double SimpleVectorOperations::KahanSummation(bool Time) const {
	Timer timeit(Time);
	double sum = 0.0;
	double c = 0.0; // A running compensation for lost low-order bits.
	for (const auto& num : vec) {
		double y = num - c;
		double t = sum + y;
		c = (t - sum) - y;
		sum = t;
	}
	return sum;
}

// Method (4) Simple product
double SimpleVectorOperations::product1(bool Time) const {
	Timer timeit(Time);
	double prod = 1.0;
	for (const auto& val : vec) {
		prod *= val;
	}
	return prod;
}
// Method (5) std::acumulate product
double SimpleVectorOperations::product2(bool Time) const {
	Timer timeit(Time);
	double prod = 0.0;
	prod = std::accumulate(vec.begin(), vec.end(), 1.0, std::multiplies<double>());
	return prod;
}
// Method (6) Simple adjacent difference 
void SimpleVectorOperations::adjacent_difference1(std::vector<double>& diff, bool Time) const {
	Timer timeit(Time);
	for (std::size_t i = 1; i < vec.size(); ++i) {
		diff.push_back(vec[i] - vec[i - 1]);
	}
}
// Method (7) std::adjacent difference 
void SimpleVectorOperations::adjacent_difference2(std::vector<double>& diff, bool Time) const {
	Timer timeit(Time);
	diff.resize(vec.size());
	std::adjacent_difference(vec.begin(), vec.end(), diff.begin());
}
// Method (8) Simple adjacent difference using array 
void SimpleVectorOperations::adjacent_difference3(std::unique_ptr<double[]>& diff, bool Time) const {
	Timer timeit(Time);
	for (std::size_t i = 1; i < vec.size(); ++i) {
		diff[i - 1] = vec[i] - vec[i - 1];
	}
}
// Method (9) Multi threaded summation using lambda function
double MultiThreadVectorOperations::ComputeSumMultiThreadSum1(bool Time) {
	Timer timeit(Time);
	size_t NumOfThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	std::vector<double> Partial_Sums(NumOfThreads);
	auto begin = vec.begin();
	auto step = vec.size() / NumOfThreads;


	for (size_t i = 0; i < NumOfThreads; i++)
	{
		auto thread_begin = begin + i * step;
		auto thread_end = i == NumOfThreads - 1 ? vec.end() : (begin + (i + 1) * step);
		threads.push_back(std::thread([&, thread_begin, thread_end, i]() {
			double sum = std::accumulate(thread_begin, thread_end, 0.0);
			Partial_Sums[i] = sum;
			}));
	}

	for (auto& t : threads) {
		if (t.joinable()) {
			t.join();
		}
	}
	const double total_sum = std::accumulate(Partial_Sums.begin(), Partial_Sums.end(), 0.0);
	return total_sum;
}

// Functor used for method (10)
struct SumHelper {
	std::vector<double>::const_iterator begin;
	std::vector<double>::const_iterator end;
	double& result;

	SumHelper(std::vector<double>::const_iterator begin, std::vector<double>::const_iterator end, double& result)
		: begin(begin), end(end), result(result) {}
	void operator()() {
		result = std::accumulate(begin, end, 0.0);
	}
};

// Method (10) Multi threaded summation using Functor
double MultiThreadVectorOperations::ComputeSumMultiThreadSum2(bool Time) {
	Timer timeit(Time);
	unsigned int NumOfThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads(NumOfThreads);
	std::vector<double> results(NumOfThreads);
	std::vector<std::unique_ptr<SumHelper>> helpers;
	auto begin = vec.begin();
	const auto step = vec.size() / NumOfThreads;

	for (unsigned int i = 0; i < NumOfThreads; ++i)
	{
		auto thread_begin = begin + i * step;
		auto thread_end = i == NumOfThreads - 1 ? vec.end() : (begin + (i + 1) * step);
		helpers.push_back(std::make_unique<SumHelper>(thread_begin, thread_end, results[i]));
		threads[i] = std::thread(std::ref(*helpers[i]));
	}

	for (auto& t : threads) {
		if (t.joinable()) {
			t.join();
		}
	}
	const double total_sum = std::accumulate(results.begin(), results.end(), 0.0);
	return total_sum;
}

// Helper function used for method (11)
void MultiThreadVectorOperations::CalculatePartialSum(unsigned int start, unsigned int end) {
	double partial_sum = std::accumulate(vec.begin() + start, vec.begin() + end, 0.0);
	for (double old_sum = sum.load(); !sum.compare_exchange_weak(old_sum, old_sum + partial_sum); ) {
		// Empty, everything is done in the condition
	}
}

// Method (11) Multi threaded summation using atomic variable
double MultiThreadVectorOperations::ComputeSumMultiThreadSum3(bool Time) {
	sum = 0.0;
	Timer timeit(Time);
	unsigned int NumOfThreads = std::thread::hardware_concurrency();
	std::vector<std::future<void>> futures(NumOfThreads);
	auto begin = vec.begin();
	const auto step = vec.size() / NumOfThreads;

	for (unsigned int i = 0; i < NumOfThreads; i++) {
		unsigned int start = i * (vec.size() / NumOfThreads);
		unsigned int end = (i + 1) == NumOfThreads ? vec.size() : (i + 1) * (vec.size() / NumOfThreads);
		futures[i] = std::async(std::launch::async, &MultiThreadVectorOperations::CalculatePartialSum, this, start, end);
	}

	for (auto& fut : futures) {
		fut.get();
	}

	return sum.load();  // load() is needed to safely read from an std::atomic
}

// Method (12) to compute the product of values in a vector using multithreading
double MultiThreadVectorOperations::ComputeSumMultiThreadProduct(bool Time) {
	Timer timeit(Time);
	std::mutex mtx;
	double product = 1.0;

	// Get number of supported threads
	unsigned int num_threads = std::thread::hardware_concurrency();

	// Vector of futures
	std::vector<std::future<void>> futures(num_threads);

	for (unsigned int i = 0; i < num_threads; ++i) {
		futures[i] = std::async(std::launch::async, [&](unsigned int start, unsigned int end) {
			double partial_product = std::accumulate(vec.begin() + start, vec.begin() + end, 1.0, std::multiplies<double>());
			std::lock_guard<std::mutex> lock(mtx);
			product *= partial_product;
			}, i * (vec.size() / num_threads), (i + 1) == num_threads ? vec.size() : (i + 1) * (vec.size() / num_threads));
	}

	// Wait for all threads to complete
	for (auto& fut : futures)
		fut.get();

	return product;
}

// Method (13) to compute the difference between adjacent values in a vector using async
void MultiThreadVectorOperations::ComputeAdjDiffMultiThread1(std::vector<double>& diff, bool Time) const {
	Timer timeit(Time);
	unsigned int NumOfThreads = std::thread::hardware_concurrency();
	std::vector<std::future<void>> futures(NumOfThreads);
	size_t vec_size = vec.size();
	size_t step = vec_size / NumOfThreads;
	size_t remaining = vec_size % NumOfThreads;

	size_t start = 0, end = 0;
	// Step 1: Compute adjacent differences in parallel, ignoring boundaries between subsets
	for (unsigned int i = 0; i < NumOfThreads; ++i) {
		start = end;
		end = start + step + (i < remaining ? 1 : 0); // Distribute remaining elements among the first few threads
		futures[i] = std::async(std::launch::async, [&diff, this, start, end, i] {
			std::adjacent_difference(this->vec.begin() + start, this->vec.begin() + end, diff.begin() + start);
			});
	}

	// Make sure all threads have finished before correcting the boundaries
	for (auto& f : futures) {
		f.get();
	}

	// Step 2: Correct the first difference in each subset, except for the first subset
	end = 0;
	for (unsigned int i = 1; i < NumOfThreads; ++i) {
		start = end;
		end = start + step + (i <= remaining ? 1 : 0); // Distribute remaining elements among the first few threads
		if (end < vec_size) {
			diff[end] = vec[end] - vec[end - 1];
		}
	}
}
// Method (14) to compute the difference between adjacent values in a vector using async
void MultiThreadVectorOperations::ComputeAdjDiffMultiThread2(std::vector<double>& diff, bool Time) const {
	Timer timeit(Time);
	unsigned int NumOfThreads = omp_get_max_threads();
	unsigned int step = vec.size() / NumOfThreads;

	// Step 1: Compute adjacent differences in parallel, ignoring boundaries between subsets
#pragma omp parallel for
	for (unsigned int i = 0; i < NumOfThreads; ++i) {
		auto thread_begin = vec.begin() + i * step;
		auto thread_end = (i + 1 == NumOfThreads) ? vec.end() : vec.begin() + (i + 1) * step;
		std::adjacent_difference(thread_begin, thread_end, diff.begin() + i * step);
	}

	// Step 2: Correct the first difference in each subset, except for the first subset
	for (unsigned int i = 1; i < NumOfThreads; ++i) {
		diff[i * step] = vec[i * step] - vec[i * step - 1];
	}
}
