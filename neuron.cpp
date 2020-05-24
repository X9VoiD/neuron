#include <iostream>
#include <thread>
#include <exception>
#include <algorithm>
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <atomic>
#include <unordered_map>
#include <random>
#include <array>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <chrono>
constexpr auto ACTIVATIONBIAS = 0;
constexpr auto BRAINSIZE = 10;
constexpr auto THREADS = 4;

/*
		TODO: Axon Growth; Dendrite Signal Convergence; Brain
		TODO: HIGH PRIO: Neuron Spatial Awareness, Synchronize ThreadPool Init to MainThread
*/

//////////////////
// Neuron Start //
//////////////////

class Neuron {
private:
	// Declare appendages
	class Axon;
	class CollectiveDendrite;
	// End Declaration

	struct NeuronState {
		bool hot;
		std::array<float, 3> position;
		int activationbias;
		int nconnections;
	};

public:
	Neuron(float x, float y, float z) {
		state = std::make_shared<NeuronState>();
		state->activationbias = ACTIVATIONBIAS;
		state->nconnections = 0;
		state->position.at(0) = x;
		state->position.at(1) = y;
		state->position.at(2) = z;
		// Initialize appendages
		dendrite = std::make_shared<CollectiveDendrite>(state);
		axon = std::make_shared<Axon>(state);
	}

	~Neuron() = default;
	Neuron& operator=(const Neuron&) = delete;
	Neuron(const Neuron&) = delete;
	Neuron& operator=(Neuron&&) = default;
	Neuron(Neuron&&) = default;

	const std::shared_ptr<NeuronState>& get_state() {
		return state;
	}

	void update() {
		// TODO: Implement /////////////////////////////////////////
		dendrite->update();
		axon->update();
	}

private:
	////////////////
	// Axon START //
	////////////////
	class Axon {
	public:
		Axon(const std::shared_ptr<NeuronState>& pneuron_state) {
			neuron_state = pneuron_state;
			id = this;
		}

		~Axon() = default;
		Axon& operator=(const Axon&) = delete;
		Axon(const Axon&) = delete;
		Axon& operator=(Axon&&) = delete;
		Axon(Axon&&) = delete;

		void send_pulse() {
			for (auto const& synapse : targets) {
				synapse.target->receive_pulse(id);
			}
			neuron_state->hot = true;
		}

		void update() {
			// TODO: IMPLEMENT ///////////////////////////
		}

	private:
		struct AxonTarget {
			CollectiveDendrite *target;
		};
		Axon* id;
		std::vector<AxonTarget> targets;
		std::vector<AxonTarget> candidates;
		std::shared_ptr<NeuronState> neuron_state;
	};
	//////////////
	// Axon END //
	//////////////

	//////////////////////////////
	// CollectiveDendrite Start //
	//////////////////////////////
	class CollectiveDendrite {
	public:
		CollectiveDendrite(const std::shared_ptr<NeuronState>& pneuron_state) {
			neuron_state = pneuron_state;
		}

		~CollectiveDendrite() = default;
		CollectiveDendrite& operator=(const CollectiveDendrite&) = delete;
		CollectiveDendrite(const CollectiveDendrite&) = delete;
		CollectiveDendrite& operator=(CollectiveDendrite&&) = delete;
		CollectiveDendrite(CollectiveDendrite&&) = delete;

		void form_link(Axon* axon, int length) {
			auto formed_link = std::make_unique<Link>();
			formed_link->length = length;
			links[axon] = std::move(formed_link);
		}
		void remove_link(Axon* axon) {
			links.erase(axon);
		}
		void receive_pulse(Axon* pulse_sender) {
			links[pulse_sender]->pulses.push_back(links[pulse_sender]->length);
		}
		void update() {
			// TODO: IMPLEMENT /////////////////////////////
		}
	private:
		struct Link {
			std::vector<unsigned int> pulses;
			int length = 0;
		};
		std::unordered_map<Axon*, std::unique_ptr<Link>> links;
		std::shared_ptr<NeuronState> neuron_state;
	};
	////////////////////////////
	// CollectiveDendrite End //
	////////////////////////////
	
	// Neuron Class Members
	std::shared_ptr<Axon> axon;
	std::shared_ptr<CollectiveDendrite> dendrite;
	std::shared_ptr<NeuronState> state;
	// End Neuron Class Members

public:
	inline const std::shared_ptr<CollectiveDendrite>& get_dendrite() { return dendrite; }
	inline const std::shared_ptr<Axon>& get_axon() { return axon; }
};

////////////////
// Neuron End //
////////////////

class ThreadPool {

public:

	ThreadPool() {
		int i = 0;
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
	~ThreadPool() {
		try {
			if (running) {
				for (auto& child : pool) {
					child->join();
				}
			}
		}
		catch (...) {};
	}

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;

	void shutdown() {
		main_barrier->invalidate();
		running = 0;
	}

	void join() {
		for (auto& child : pool) {
			child->join();
		}
	}

private:

	struct ThreadState;
	std::array<std::unique_ptr<std::thread>, THREADS> pool;
	std::atomic<int> running = 1;
	std::vector<std::shared_ptr<ThreadState>> queue_array;

	// START Barrier
	class Barrier {
	private:
		std::atomic<int> count;
		int num_threads;
		int left;
		std::mutex mtx;
		std::condition_variable cv;
		std::atomic<bool> bypass = false;
	public:
		Barrier(int pnum_threads) {
			count = pnum_threads;
			num_threads = pnum_threads;
			left = 0;
		}

		~Barrier() = default;
		Barrier& operator=(const Barrier&) = delete;
		Barrier(const Barrier&) = delete;
		Barrier& operator=(Barrier&&) = delete;
		Barrier(Barrier&&) = delete;

		void sync() {
			std::unique_lock<std::mutex> lck(mtx);
			--count;
			if (count == 0) {
				cv.notify_all();
				count = num_threads;
			}
			else {
				if (!bypass) { cv.wait(lck); }
				else if (bypass) { cv.notify_all(); }
			}
		}

		void invalidate() {
			bypass = true;
			cv.notify_all();
		}
	};
	// END Barrier

	std::unique_ptr<Barrier> pool_barrier;
	std::shared_ptr<Barrier> main_barrier;

	// START ThreadState
	struct ThreadState {
	public:
		std::shared_ptr<Barrier> barrier;
		int id;
		int tick_observer;
		std::queue<std::function<void()>> c1;
		std::queue<std::function<void()>> c2;
		std::mutex tqueue_lock;
		ThreadState(std::shared_ptr<Barrier>& pbarrier, int pid) {
			barrier = std::move(pbarrier);
			id = pid;
			tick_observer = 0;
		}

		inline std::queue<std::function<void()>>& get_command_array() {
			if (tick_observer == 1) {
				return c2;
			}
			else {
				return c1;
			}
		}

		inline std::queue<std::function<void()>>& get_future_command_array() {
			if (tick_observer == 1) {
				return c1;
			}
			else {
				return c2;
			}
		}
		inline void prepare_next_tick() {
			if    (tick_observer == 1) { tick_observer = 0; }
			else                       { tick_observer = 1; }
		}
	};
	// END ThreadState

public:

	void enqueue(std::function<void()>& work) {
		auto t = std::min_element(queue_array.begin(), queue_array.end(), [] (const std::shared_ptr<ThreadState>& a, const std::shared_ptr<ThreadState>& b) {
			return a->get_future_command_array().size() < b->get_future_command_array().size();
		});
		std::unique_lock<std::mutex> lck((*t)->tqueue_lock);
		(*t)->get_future_command_array().push(work);
	}

	Barrier* get_barrier() {
		return &(*pool_barrier);
	}

private:

	void threadFunc(const std::shared_ptr<ThreadState>& state) {
		pool_barrier->sync();
		while (running == 1) {
			while (!(state->get_command_array().empty())) {
				state->get_command_array().front()();
				state->get_command_array().pop();
			}
			state->prepare_next_tick();
			state->barrier->sync();
		}
	}
};

class Brain {
public:
	// TODO
	Brain() {
		worker = std::make_unique<ThreadPool>();
		constexpr float brain_world_center = 2.0;
		constexpr int mem_relax = 5;
		neurons.reserve((BRAINSIZE ^ 3) / mem_relax);
		distribution = std::uniform_real_distribution<float>( -(static_cast<float>(BRAINSIZE) / brain_world_center),
			                                                   (static_cast<float>(BRAINSIZE) / brain_world_center) );
		rng = std::mt19937(seed());
	}
	inline float gen_rand_position() {
		return distribution(rng);
	}

private:
	std::random_device seed;
	std::mt19937 rng;
	std::vector<Neuron> neurons;
	std::uniform_real_distribution<float> distribution;
	std::unique_ptr<ThreadPool> worker;

public:
	void generate_neuron() {
		neurons.emplace_back(gen_rand_position(), gen_rand_position(), gen_rand_position());
		// TODO: Implement neuron spatial awareness.
	}
	std::vector<Neuron>& get_neurons() {
		return neurons;
	}
	void start() {
		// TODO: Tickle a random Neuron
		worker->get_barrier()->sync();
	}

	void shutdown() {
		worker->shutdown();
		worker->join();
	}
};

int main() {
	try {
		constexpr auto test_rand = 10000;

		auto brain = std::make_unique<Brain>();
		std::cout << "Alpha Boot Success.\n";

		// Start seeding the Brain with random Neurons
		for (int i = 0; i != test_rand; i++) {
			brain->generate_neuron();
		}
		std::cout << "Brain Test Run\n";
		std::cout << "Press Enter to run simulation\n";
		std::cin.get();
		brain->start();
		std::cout << "Processing... Press Enter to shutdown the <Brain>";
		std::cin.get();
		std::cout << "Attempting Brain Shutdown.\n";
		brain->shutdown();
		std::cout << "Brain Shutdown Successful.\n";
	}
	catch (...) { }
}