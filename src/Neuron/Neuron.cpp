#include "../../include/Neuron.h"
constexpr auto PULSE_RESISTANCE = 4.0f;



// Neuron



Neuron::Neuron(float x, float y, float z, ThreadPool* p_worker, unsigned int id)
{
	state = std::make_shared<NeuronState>(x, y, z, PULSE_RESISTANCE, p_worker, id);
	dendrite = std::make_shared<CollectiveDendrite>(state);
	axon = std::make_shared<Axon>(state);
}

void Neuron::update()
{
	// TODO: Implement
	if (state->polarization >= state->pulse_resistance)
	{
		axon->send_pulse();
		decay();
		state->polarization -= state->pulse_resistance;
	}
	dendrite->update();
	axon->update();
	decay(); // allow two decays for excited neuron
	         // to compensate for the over or under polarization
		 // after sending a pulse.
}

void Neuron::decay() noexcept
{
	state->polarization *= (1.0f/3.0f);
}



// Axon



void Neuron::Axon::send_pulse()
{
	for (auto const& synapse : targets)
	{
		neuron_state->worker->enqueue(
			[&]() { synapse.target->receive_pulse(id); });
	}
	neuron_state->hot = true;
}

void Neuron::Axon::update()
{
	// TODO: Implement
}



// CollectiveDendrite



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
		unsigned int tracker = 0;
		pulse_del_buffer.clear();

		for (auto& pulse : link.second->pulses)
		{
			if (pulse > 1) { pulse--; }
			else
			{ 

				pulse_del_buffer.push_back(tracker);
				neuron_state->worker->enqueue(
					[=, &link]() { collect_pulse(link.first, *(link.second)); }
				);
			}
			++tracker;
		}

		unsigned int offset = 0;
		auto pulse_iter = link.second->pulses.begin();
		
		for (auto n : pulse_del_buffer)
		{
			link.second->pulses.erase(pulse_iter + n - offset);
			++offset;
			
		}
	}
}

void Neuron::CollectiveDendrite::collect_pulse(Axon* sender, const Link& link)
{
	neuron_state->polarization += link.weight;
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
	travel_pulses();
}
