#include <array>
#include "../Neuron.h"
constexpr auto ACTIVATIONBIAS = 0;



struct Neuron::NeuronState {
	bool hot;
	std::array<float, 3> position;
	int activationbias;
	int nconnections;
};




// Neuron




Neuron::Neuron(float x, float y, float z) {
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



inline const std::shared_ptr<Neuron::NeuronState>& Neuron::get_state() {
	return state;
}



void Neuron::update() {
	// TODO: Implement
	dendrite->update();
	axon->update();
}



inline const std::shared_ptr<Neuron::CollectiveDendrite>& Neuron::get_dendrite() { return dendrite; }
inline const std::shared_ptr<Neuron::Axon>& Neuron::get_axon() { return axon; }




// Axon




struct Neuron::Axon::AxonTarget {
	CollectiveDendrite* target;
};



Neuron::Axon::Axon(const std::shared_ptr<NeuronState>& pneuron_state) {
	neuron_state = pneuron_state;
	id = this;
}



void Neuron::Axon::send_pulse() {
	for (auto const& synapse : targets) {
		synapse.target->receive_pulse(id);
	}
	neuron_state->hot = true;
}



void Neuron::Axon::update() {
	// TODO: Implement
}




// CollectiveDendrite




struct Neuron::CollectiveDendrite::Link{
			std::vector<unsigned int> pulses;
			int length = 0;
};



Neuron::CollectiveDendrite::CollectiveDendrite(const std::shared_ptr<NeuronState>& pneuron_state) {
	neuron_state = pneuron_state;
}



void Neuron::CollectiveDendrite::form_link(Axon* axon, int length) {
	auto formed_link = std::make_unique<Link>();
	formed_link->length = length;
	links[axon] = std::move(formed_link);
}



void Neuron::CollectiveDendrite::remove_link(Axon* axon) {
	links.erase(axon);
}



void Neuron::CollectiveDendrite::receive_pulse(Axon* pulse_sender) {
	links[pulse_sender]->pulses.push_back(links[pulse_sender]->length);
}



void Neuron::CollectiveDendrite::update() {
	// TODO: IMPLEMENT /////////////////////////////
}