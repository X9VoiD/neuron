#pragma once
#include <memory>
#include <array>
#include <queue>
#include <atomic>
#include <thread>
#include <mutex>
#include <functional>
constexpr auto THREADS = 4;

class ThreadPool
{
	struct ThreadState;
	class Barrier;
	std::atomic<int> running = 1;

	std::array<std::unique_ptr<std::thread>, THREADS> pool;
	std::vector<std::shared_ptr<ThreadState>> queue_array;

	std::unique_ptr<Barrier> pool_barrier;
	std::shared_ptr<Barrier> main_barrier;

	void threadFunc(const std::shared_ptr<ThreadState>&);

public:
	ThreadPool();
	~ThreadPool();

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;

	void shutdown();
	void join();

	void enqueue(std::function<void()>&);
	Barrier* get_barrier();
};

struct ThreadPool::ThreadState
{
	std::shared_ptr<Barrier> barrier;
	int id;
	int tick_observer;
	std::queue<std::function<void()>> c1;
	std::queue<std::function<void()>> c2;
	std::mutex tqueue_lock;
	ThreadState(std::shared_ptr<Barrier>&, int);

	inline std::queue<std::function<void()>>& get_command_array();
	inline std::queue<std::function<void()>>& get_future_command_array();
	inline void prepare_next_tick();
};

class ThreadPool::Barrier
{
	std::atomic<int> count;
	int num_threads;
	int left;
	std::mutex mtx;
	std::condition_variable cv;
	std::atomic<bool> bypass = false;

public:
	Barrier(int);

	~Barrier() = default;
	Barrier& operator=(const Barrier&) = delete;
	Barrier(const Barrier&) = delete;
	Barrier& operator=(Barrier&&) = delete;
	Barrier(Barrier&&) = delete;

	void sync();
	void invalidate();
};