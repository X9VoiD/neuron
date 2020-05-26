#include "../ThreadPool.h"

const unsigned int THREADS = std::thread::hardware_concurrency();



// ThreadPool



ThreadPool::ThreadPool()
{
	int i = 0;
	pool.resize(THREADS);
	main_barrier = std::make_shared<Barrier>(THREADS);
	pool_barrier = std::make_unique<Barrier>(THREADS + 1);
	queue_array.reserve(THREADS);
	for (auto& child : pool) {
		std::shared_ptr<Barrier> ppbarrier = main_barrier;
		std::shared_ptr<ThreadState> tstate = std::make_shared<ThreadState>(ppbarrier, i);
		queue_array.push_back(tstate);
		child = std::make_unique<std::thread>(&ThreadPool::threadFunc, this, std::move(tstate));
		++i;
	}
}

ThreadPool::~ThreadPool()
{
	try
	{
		if (running)
		{
			for (auto& child : pool)
			{
				child->join();
			}
		}
	}
	catch (...) {};
}

void ThreadPool::shutdown()
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
}

void ThreadPool::enqueue(std::function<void()>& work)
{
	auto t = std::min_element(queue_array.begin(), queue_array.end(),
		[](const std::shared_ptr<ThreadState>& a, const std::shared_ptr<ThreadState>& b)
		{
			return a->get_future_command_array().size() < b->get_future_command_array().size();
		});
	std::unique_lock<std::mutex> lck((*t)->tqueue_lock);
	(*t)->get_future_command_array().push(work);
}

void ThreadPool::threadFunc(const std::shared_ptr<ThreadState>& state)
{
	pool_barrier->sync();
	while (running == 1)
	{
		while (!(state->get_command_array().empty()))
		{
			state->get_command_array().front()();
			state->get_command_array().pop();
		}
		state->prepare_next_tick();
		state->barrier->sync();
	}
}

ThreadPool::Barrier* ThreadPool::get_barrier()
{
	return &(*pool_barrier);
}



// Barrier



ThreadPool::Barrier::Barrier(int pnum_threads)
{
	count = pnum_threads;
	num_threads = pnum_threads;
	left = 0;
}

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
		else if (bypass) { cv.notify_all(); }
	}
}

void ThreadPool::Barrier::invalidate()
{
	bypass = true;
	cv.notify_all();
}



// ThreadState



ThreadPool::ThreadState::ThreadState(std::shared_ptr<Barrier>& pbarrier, int pid)
{
	barrier = std::move(pbarrier);
	id = pid;
	tick_observer = 0;
}

inline std::queue<std::function<void()>>&
ThreadPool::ThreadState::get_command_array()
{
	if (tick_observer == 1) { return c2; }
	else { return c1; }
}

inline std::queue<std::function<void()>>&
ThreadPool::ThreadState::get_future_command_array()
{
	if (tick_observer == 1) { return c1; }
	else { return c2; }
}

inline void ThreadPool::ThreadState::prepare_next_tick()
{
	if (tick_observer == 1) { tick_observer = 0; }
	else { tick_observer = 1; }
}