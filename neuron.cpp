#include <iostream>
#include <thread>
#include <exception>
#include <algorithm>
#include <functional>
#include <vector>
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
		TODO: Axon Growth; Dendrite Signal Convergence; Brain; Queue
*/

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

class ThreadPool {

public:

	ThreadPool() {
		int i = 0;
		main_barrier = std::make_shared<Barrier>(THREADS);
		queue_array.reserve(THREADS);
		for (auto& child : pool) {
			std::shared_ptr<Barrier> ppbarrier = main_barrier;
			std::unique_ptr<ThreadState> tstate = std::make_unique<ThreadState>(ppbarrier, i);
			queue_array.at(i) = &(*tstate);
			child = std::make_unique<std::thread>(&ThreadPool::threadFunc, this, std::move(tstate));
			++i;
		}
	}
	~ThreadPool() {
		for (auto& child : pool) {
			child->join();
		}
	}

	void shutdown() {
		running = 0;
	}

private:

	struct ThreadState;
	std::array<std::unique_ptr<std::thread>, THREADS> pool;
	std::atomic<int> running = 1;
	std::vector<ThreadState*> queue_array;

	class Barrier {
	private:
		int count;
		int num_threads;
		int left;
		std::mutex mtx;
		std::condition_variable cv;
	public:
		Barrier(int pnum_threads) {
			count = pnum_threads;
			num_threads = pnum_threads;
			left = 0;
		}
		void sync() {
			std::unique_lock<std::mutex> lck(mtx);
			if ((--count == 0) && (left == (num_threads - 1))) {
				cv.notify_all();
				count = num_threads;
				left = 0;
			}
			else {
				cv.wait(lck, [this]() { return count == 0; });
			}
			left++;
		}
	};

	std::shared_ptr<Barrier> main_barrier;

	struct ThreadState {
	public:
		std::shared_ptr<Barrier> barrier;
		bool waiting = false;
		bool leaved = false;
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

	void enqueue(std::function<void()>& work) {
		auto t = std::min_element(queue_array.begin(), queue_array.end(), [] (ThreadState *a, ThreadState *b) {
			return a->get_future_command_array().size() < b->get_future_command_array().size();
		});
		std::unique_lock<std::mutex> lck((*t)->tqueue_lock);
		(*t)->get_future_command_array().push(work);
	}

private:

	void threadFunc(const std::unique_ptr<ThreadState>& state) {
		// TODO: Implement
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

public:
	void generate_neuron() {
		neurons.emplace_back(gen_rand_position(), gen_rand_position(), gen_rand_position());
	}
	std::vector<Neuron>* get_neurons() {
		return &neurons;
	}
};

int main() {
	try {
		constexpr auto test_rand = 10000;
		auto brain = std::make_unique<Brain>();
		std::cout << "Alpha Boot Success.\n";
		for (int i = 0; i != test_rand; i++) {
			brain->generate_neuron();
		}
	}
	catch(...) { }
}