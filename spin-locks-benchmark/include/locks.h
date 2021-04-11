#ifndef LOCKS_H
#define LOCKS_H

#include <atomic>
#include <vector>

void nop() {
    do {
        (void)0;
    } while (false);
}

class TestAndSetLock {
  public:
    void lock() {
		while (is_locked.exchange(true, std::memory_order_acquire)) {
			// nop
		};
    }

    void unlock() { 
		is_locked.store(false, std::memory_order_release); 
	}

  private:
    std::atomic_bool is_locked = {false};
};

class TestAndTestAndSetLock {
  public:
    void lock() {
        do {
			while (is_locked.load(std::memory_order_relaxed)) {
				// nop
			}
		} while (is_locked.exchange(true, std::memory_order_acquire));
    }

    void unlock() { 
		is_locked.store(false, std::memory_order_release); 
	}

  private:
    std::atomic_bool is_locked = {false};
};

class ExpBackoffLock {
  public:
    void lock() {
        size_t curent_max_delay = MIN_BACKOFF_ITERS;

        while (true) {
            wait_untill_free();

			if (is_locked.exchange(true, std::memory_order_acquire)) {
				exponential_backoff(curent_max_delay);
			} else {
				break;
			}
                
        }
    }

    void unlock() { 
		is_locked.store(false, std::memory_order_release); 
	}

  private:
    void wait_untill_free() const {
        size_t num_iters = 0;

        while (is_locked.load(std::memory_order_relaxed)) {
            if (num_iters < MAX_WAIT_ITERS) {
                num_iters++;
                nop();
			} else {
				yield_sleep();
			}
        }
    }

	void exponential_backoff(size_t& curMaxIters) {
        static const size_t MAX_BACKOFF_ITERS = 1024;
		curMaxIters = std::min(2 * curMaxIters, MAX_BACKOFF_ITERS);

		thread_local std::uniform_int_distribution<size_t> dist;
		thread_local std::minstd_rand gen(std::random_device{}());

		const size_t itters_num = dist(gen, decltype(dist)::param_type{ 0, curMaxIters });
		for (size_t i = 0; i < itters_num; i++) {
			nop();
		}
	}

	void yield_sleep() const {
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(500us);
	}

  public:
    std::atomic_bool is_locked = {false};

  private:
    static const size_t MAX_WAIT_ITERS = 0x10000;
    static const size_t MIN_BACKOFF_ITERS = 32;
};
#endif // LOCKS_H
