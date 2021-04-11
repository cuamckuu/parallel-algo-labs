#include <iostream>
#include <iomanip>
#include <random>
#include <future>

#include "sets.hpp"


template<typename SetType>
std::vector<std::chrono::milliseconds> run_tests(size_t number_of_tests, size_t iters_per_test, size_t threads_num) {
    const size_t iters_per_thread = iters_per_test / threads_num;
    volatile std::atomic_size_t cnt = {0};

    std::vector<std::chrono::milliseconds> runs_elapsed_time(number_of_tests);
    for (size_t i = 0; i < number_of_tests; ++i) {
        std::vector<std::future<void>> threads(threads_num);
        SetType set;

        const auto start_time = std::chrono::high_resolution_clock::now();
        for (size_t thread_i = 0; thread_i < threads_num; ++thread_i) {
            threads[thread_i] = std::async(std::launch::async, [&]() {
                for (size_t itter = 0; itter < iters_per_thread; ++itter) {
                    std::random_device rseed;
                    std::mt19937 rgen(rseed());
                    std::uniform_int_distribution<int> idist(0, 100);

                    int num = idist(rgen);
                    int op = idist(rgen);

                    set.contains(num);
                    if (op % 2 == 0) {
                        set.add(num);
                    } else {
                        set.remove(num);
                    }
                    set.contains(num);
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

template<typename SetType>
void display_elapsed_time(std::string name, size_t number_of_tests, size_t iters_per_test, size_t threads_num) {
    const auto& runs_elapsed_time = run_tests<SetType>(number_of_tests, iters_per_test, threads_num);

    double sum_elapsed_ms = 0;
    for (const auto &elapsed_time_ms : runs_elapsed_time) {
        sum_elapsed_ms += static_cast<double>(elapsed_time_ms.count());;
    }

    double mean_elapsed_ms = sum_elapsed_ms / runs_elapsed_time.size();

    std::cout << name << "  " << std::fixed << std::setprecision(3) << mean_elapsed_ms << "\n";
}

int main() {
    CoarseGrainedSet<int> set;

    for (size_t threads_num = 1; threads_num <= 3+std::thread::hardware_concurrency(); ++threads_num) {
        const size_t number_of_tests = 5;
        const size_t iters_per_test = 100000;

        std::cout << threads_num << " threads (ops per thread: " << iters_per_test / threads_num << ")\n";

        display_elapsed_time<CoarseGrainedSet<int>>("CoarseGrainedSet", number_of_tests, iters_per_test, threads_num);

        std::cout << "===\n";
    }


    return 0;
}
