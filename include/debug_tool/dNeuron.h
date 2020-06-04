#pragma once
#include <cmath>
#include "../Neuron.h"

class meta_neuron
{
public:
	meta_neuron() = delete;

	/*
		form_link:
			Force complete the linking of two given neurons. If max_weight
			is true, every pulse received from the formed link will fire 
			an impulse.
	 */
	static void form_link(const Neuron& from, const Neuron& to, bool max_weight)
	{
		auto to_p = to.state->position;
		auto from_p = from.state->position;
		float distance = std::sqrt(
			std::pow((to_p.at(0) - from_p.at(0)), 2) +
			std::pow((to_p.at(1) - from_p.at(1)), 2) +
			std::pow((to_p.at(2) - from_p.at(2)), 2));
		from.axon->targets.emplace_back(&*(to.dendrite), distance, distance);
		to.dendrite->form_link(&*(from.axon), static_cast<int>(std::floor(distance)));
		if (max_weight)
		{
			to.dendrite->links[&*(from.axon)]->weight = to.state->pulse_resistance;
		}
	}

	/*
		prepare_fire:
			Force a neuron to fire on next world tick
	 */
	static void prepare_fire(const Neuron& neuron) noexcept
	{
		neuron.state->polarization = neuron.state->pulse_resistance;
	}
};
