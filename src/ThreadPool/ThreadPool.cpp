#include <chrono>
#include "../../include/ThreadPool.h"

const unsigned int THREADS = std::thread::hardware_concurrency();



// ThreadPool

class Brain;

ThreadPool::ThreadPool(Brain* pbrain): brain(pbrain)
{
	int i = 0;
	pool.resize(THREADS);
	main_barrier = std::make_unique<Barrier>(THREADS);
	pool_barrier = std::make_unique<Barrier>(THREADS + 1);
	//main_barrier = std::make_unique<Barrier>(THREADS + 1);
	//pool_barrier = std::make_unique<Barrier>(THREADS + 2);
	queue_array.reserve(THREADS);
	//choreographer = std::make_unique<std::thread>(&ThreadPool::choreoFunc, this);
	for (auto& child : pool)
	{
		queue_array.push_back(std::move(std::make_shared<ThreadState>(i)));
		child = std::make_unique<std::thread>(&ThreadPool::threadFunc, this, &(*queue_array[i]));
		++i;
	}
}

void ThreadPool::shutdown() noexcept
{
	main_barrier->invalidate();
	running = 0;
}

void ThreadPool::join()
{
	for (auto& child : pool)
	{
		child->join();
	}
	//choreographer->join();
}

void ThreadPool::enqueue(std::function<void()> work)
{
	auto t = std::min_element(queue_array.begin(), queue_array.end(),
		[](const std::shared_ptr<ThreadState>& a, const std::shared_ptr<ThreadState>& b)
		{
			return a->get_future_command_array().size() < b->get_future_command_array().size();
		});
	std::unique_lock<std::mutex> lck((*t)->tqueue_lock);
	(*t)->get_future_command_array().push(work);
}

void ThreadPool::register_neuron(Neuron* pn)
{
	auto t = std::min_element(queue_array.begin(), queue_array.end(),
			[](const std::shared_ptr<ThreadState>& a, const std::shared_ptr<ThreadState>& b)
			noexcept { return a->n.size() < b->n.size(); });
	(*t)->n.push_back(pn);
}

void ThreadPool::threadFunc(ThreadState* state)
{
	pool_barrier->sync();
	while (running == 1)
	{
		while (!(state->get_command_array().empty()))
		{
			state->get_command_array().front()();
			state->get_command_array().pop();
		}
		for (auto& neuron : state->n)
		{
			neuron->update();
		}
		state->prepare_next_tick();
		main_barrier->sync();
	}
}

/*
void ThreadPool::choreoFunc()
{
	using namespace std::chrono_literals;
	pool_barrier->sync();
	while (running == 1)
	{
		//std::this_thread::sleep_for(1us);
		main_barrier->sync();
	}
}
*/



// Barrier



void ThreadPool::Barrier::sync()
{
	std::unique_lock<std::mutex> lck(mtx);
	--count;
	if (count == 0)
	{
		cv.notify_all();
		count = num_threads;
	}
	else
	{
		if (!bypass) { cv.wait(lck); }
		else { cv.notify_all(); }
	}
}

void ThreadPool::Barrier::invalidate() noexcept
{
	bypass = true;
	cv.notify_all();
}



// ThreadState



ThreadPool::ThreadState::ThreadState(int pid)
{
	id = pid;
	tick_observer = 0;
}

inline std::queue<std::function<void()>>&
ThreadPool::ThreadState::get_command_array() noexcept
{
	if (tick_observer == 1) { return c2; }
	return c1;
}

inline std::queue<std::function<void()>>&
ThreadPool::ThreadState::get_future_command_array() noexcept
{
	if (tick_observer == 1) { return c1; }
	return c2;
}

inline void ThreadPool::ThreadState::prepare_next_tick() noexcept
{
	if (tick_observer == 1)
	{
		tick_observer = 0;
		return;
	}
	tick_observer = 1;
}
