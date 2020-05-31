#include "../../include/Neuron.h"
constexpr auto PULSE_RESISTANCE = 4.0f;



// Neuron

Neuron::NeuronState::NeuronState(float x, float y, float z, float p_pulse_resistance, ThreadPool* p_worker)
{
	pulse_resistance = p_pulse_resistance;
	position = { x, y, z };
	nconnections = 0;
	worker = p_worker;
}

Neuron::Neuron(float x, float y, float z, ThreadPool* p_worker)
{
		state = std::make_shared<NeuronState>(x, y, z, PULSE_RESISTANCE, p_worker);
		// Initialize appendages
		dendrite = std::make_shared<CollectiveDendrite>(state);
		axon = std::make_shared<Axon>(state);
}

void Neuron::update()
{
	// TODO: Implement
	dendrite->update();
	axon->update();
}

inline const std::shared_ptr<Neuron::NeuronState>& Neuron::get_state() { return state; }
inline const std::shared_ptr<Neuron::CollectiveDendrite>& Neuron::get_dendrite() { return dendrite; }
inline const std::shared_ptr<Neuron::Axon>& Neuron::get_axon() { return axon; }



// Axon



Neuron::Axon::Axon(const std::shared_ptr<NeuronState>& pneuron_state)
{
	neuron_state = pneuron_state;
	id = this;
}

void Neuron::Axon::send_pulse()
{
	for (auto const& synapse : targets) {
		neuron_state->worker->enqueue(
			[&]() { synapse.target->receive_pulse(id); });
	}
	neuron_state->hot = true;
}

void Neuron::Axon::update()
{
	// TODO: Implement
}

inline const std::shared_ptr<Neuron::NeuronState>& Neuron::Axon::get_state() { return neuron_state; }



// CollectiveDendrite



Neuron::CollectiveDendrite::CollectiveDendrite(const std::shared_ptr<NeuronState>& pneuron_state)
{
	neuron_state = pneuron_state;
}

void Neuron::CollectiveDendrite::form_link(Axon* axon, int length)
{
	auto formed_link = std::make_unique<Link>();
	formed_link->length = length;
	links[axon] = std::move(formed_link);
}

void Neuron::CollectiveDendrite::remove_link(Axon* axon)
{
	links.erase(axon);
}

void Neuron::CollectiveDendrite::receive_pulse(Axon* pulse_sender)
{
	links[pulse_sender]->pulses.push_back(links[pulse_sender]->length);
}

void Neuron::CollectiveDendrite::travel_pulses()
{
	for (const auto& link : links)
	{
		for (auto& pulse : link.second->pulses)
		{
			if (pulse > 1) { pulse--; }
			else
			{
				neuron_state->worker->enqueue
				(
					[=, &link]() { collect_pulse(link.first, link.second); }
				);
			}
		}
	}
}

void Neuron::CollectiveDendrite::collect_pulse(Axon* sender, const std::unique_ptr<Link>& link)
{
	neuron_state->polarization += link->weight;
	neuron_state->worker->enqueue(
		[=]() { update_link_strength(sender, sender->get_state()->hot); }
	);
}

// TODO: model a weight function to delimit increase
void Neuron::CollectiveDendrite::update_link_strength(Axon* sender, bool positive)
{
	if (sender)
	{
		// increase weight
	}
	else
	{
		// decrease weight
	}
}

void Neuron::CollectiveDendrite::update()
{
	// TODO: Implement
}

inline const std::shared_ptr<Neuron::NeuronState>& Neuron::CollectiveDendrite::get_state() { return neuron_state; }
