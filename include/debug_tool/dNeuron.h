#pragma once
#define NEURON_DEBUG
#include <cmath>
#include "../Neuron.h"

class meta_neuron
{
public:
	meta_neuron() = delete;

	/*
	 * form_link:
	 *
	 * Force complete the linking of two given neurons. If max_weight
	 * is true, every pulse received from the formed link will fire 
	 * an impulse.
	 */
	static void form_link(Neuron& from, Neuron& to, bool max_weight)
	{
		float distance = std::sqrt(
			std::pow((to.state->position[0] - from.state->position[0]), 2) +
			std::pow((to.state->position[1] - from.state->position[1]), 2) +
			std::pow((to.state->position[2] - from.state->position[2]), 2));
		from.axon->targets.emplace_back(&*(to.dendrite), distance, distance);
		to.dendrite->form_link(&*(from.axon), static_cast<int>(std::floor(distance)));
		if (max_weight)
		{
			to.dendrite->links[&*(from.axon)]->weight = to.state->pulse_resistance;
		}
	}

	/*
	 * prepare_fire:
	 *
	 * Force a neuron to fire on next world tick
	 */
	static void prepare_fire(Neuron& neuron)
	{
		neuron.state->polarization = neuron.state->pulse_resistance;
	}
};