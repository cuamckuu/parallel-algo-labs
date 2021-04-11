#include <functional>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <vector>
#include <thread>
#include <future>
#include <atomic>
#include <string>
#include <random>
#include <cmath>
#include <numeric>
#include <future>

#include "locks.h"

template<class T>
std::string get_lock_name() {
	return "Unknown Lock";
}

template<>
std::string get_lock_name<TestAndSetLock>() {
	return "TestAndSetLock";
}

template<>
std::string get_lock_name<TestAndTestAndSetLock>() {
	return "TestAndTestAndSetLock";
}

template<>
std::string get_lock_name<ExpBackoffLock>() {
	return "ExpBackoffLock";
}

template<typename LockType>
std::vector<std::chrono::milliseconds> run_lock_tests(size_t number_of_tests, size_t iters_per_test, size_t threads_num) {
    const size_t iters_per_thread = iters_per_test / threads_num;
    volatile std::atomic_size_t cnt = {0};

	std::vector<std::chrono::milliseconds> runs_elapsed_time(number_of_tests);
    for (size_t i = 0; i < number_of_tests; ++i) {
		std::vector<std::future<void>> threads(threads_num);
		LockType lock;

		const auto start_time = std::chrono::high_resolution_clock::now();
        for (size_t thread_i = 0; thread_i < threads_num; ++thread_i) {
            threads[thread_i] = std::async(std::launch::async, [&]() {
                for (size_t itter = 0; itter < iters_per_thread; ++itter) {
                    lock.lock();
					for (size_t payload = 0; payload < 16; ++payload) cnt++;
                    lock.unlock();
                }
            });
        }

		for (auto& thread : threads) {
			thread.wait();
		}

        const auto end_time = std::chrono::high_resolution_clock::now();
        runs_elapsed_time[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time);
    }

    return runs_elapsed_time;
}

template<typename LockType>
void display_elapsed_time(size_t number_of_tests, size_t iters_per_test, size_t threads_num) {
    const auto& runs_elapsed_time = run_lock_tests<LockType>(number_of_tests, iters_per_test, threads_num);
    
	double sum_elapsed_ms = 0;
    for (const auto &elapsed_time_ms : runs_elapsed_time) {
        sum_elapsed_ms += static_cast<double>(elapsed_time_ms.count());;
    }

	double mean_elapsed_ms = sum_elapsed_ms / runs_elapsed_time.size();

    std::cout << get_lock_name<LockType>() << "  " << std::fixed << std::setprecision(3) << mean_elapsed_ms << "\n";
}

int main() {
	for (size_t threads_num = 1; threads_num <= std::thread::hardware_concurrency(); ++threads_num) {
		const size_t number_of_tests = 5;
		const size_t iters_per_test = 1000000;

		std::cout << threads_num << " threads (ops per thread: " << iters_per_test / threads_num << ")\n";

		display_elapsed_time<TestAndSetLock>(number_of_tests, iters_per_test, threads_num);
		display_elapsed_time<TestAndTestAndSetLock>(number_of_tests, iters_per_test, threads_num);
		display_elapsed_time<ExpBackoffLock>(number_of_tests, iters_per_test, threads_num);

		std::cout << "===\n";
	}

	return 0;
}