#pragma once
#include <memory>
#include <array>
#include <queue>
#include <atomic>
#include <thread>
#include <mutex>
#include <functional>
#include "Neuron.h"

class Neuron;
class Brain;

class ThreadPool
{
	struct ThreadState;
	class Barrier;
	std::atomic<int> running = 1;
	Brain* brain;

	std::vector<std::unique_ptr<std::thread>> pool;
	std::unique_ptr<std::thread> choreographer;
	std::vector<std::shared_ptr<ThreadState>> queue_array;

	std::unique_ptr<Barrier> pool_barrier;
	std::unique_ptr<Barrier> main_barrier;

	void threadFunc(const std::shared_ptr<ThreadState>&);
	void choreoFunc();

public:
	ThreadPool(Brain*);
	~ThreadPool();

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;

	void shutdown();
	void join();

	void enqueue(std::function<void()>);
	void register_neuron(Neuron*);
	Barrier* get_barrier();
};

struct ThreadPool::ThreadState
{
	unsigned int id;
	unsigned int tick_observer;
	static unsigned int neuron_count;
	std::queue<std::function<void()>> c1;
	std::queue<std::function<void()>> c2;
	std::vector<Neuron*> n;
	std::mutex tqueue_lock;
	ThreadState(int);

	inline std::queue<std::function<void()>>& get_command_array();
	inline std::queue<std::function<void()>>& get_future_command_array();
	inline void prepare_next_tick();
};

class ThreadPool::Barrier
{
	std::atomic<int> count;
	unsigned int num_threads;
	unsigned int left;
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
