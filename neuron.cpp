#include <iostream>
#include <cstdio>
#include <thread>
#include <exception>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <array>
constexpr auto ACTIVATIONBIAS = 0;

/*
		TODO: Axon-Dendrite Linking; Axon Reach; Brain
*/

class Neuron
{
public:
	Neuron(float x, float y, float z) {
		state = std::make_shared<NeuronState>();
		state->activationbias = ACTIVATIONBIAS;
		state->nconnections = 0;
		state->position[0] = x;
		state->position[1] = y;
		state->position[2] = z;
		// Initialize appendages
		dendrite = std::make_shared<CollectiveDendrite>(state);
		axon = std::make_shared<Axon>(state);
	}

	void update() { }

private:
	struct NeuronState {
		unsigned int hot;
		std::array<float, 3> position;
		int activationbias;
		int nconnections;
	};

	// CollectiveDendrite Start
	class CollectiveDendrite {
	public:
		CollectiveDendrite(std::shared_ptr<NeuronState>& pneuron_state) {
			neuron_state = pneuron_state;
		}
	private:
		struct 
		std::shared_ptr<NeuronState> neuron_state;
	};
	// CollectiveDendrite End

	// Axon Start
	class Axon {
	public:
		Axon(std::shared_ptr<NeuronState>& pneuron_state) {
			neuron_state = pneuron_state;
		}

		void send_pulse() {
			std::for_each(targets.begin(), targets.end(), [](AxonTarget synapse) {
				// TODO
			});
		}

	private:
		struct AxonTarget {
			CollectiveDendrite *target;
		};

		std::vector<AxonTarget> targets;
		std::vector<AxonTarget> candidates;
		std::shared_ptr<NeuronState> neuron_state;
	};
	// Axon End
	
	// Neuron Class Members
	std::shared_ptr<Axon> axon;
	std::shared_ptr<CollectiveDendrite> dendrite;
	std::shared_ptr<NeuronState> state;
	// End Neuron Class Members

public:
	inline std::shared_ptr<CollectiveDendrite>& get_dendrite() { return dendrite; }
	inline std::shared_ptr<Axon>& get_axon() { return axon; }
};

int main() {
	try {
		std::unique_ptr<Neuron> n1 = std::make_unique<Neuron>(0.0, 0.0, 0.0);
		n1->update();
		std::cout << "Alpha Boot Success." << "12";
	}
	catch(...) { }
}