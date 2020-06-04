#pragma once
#include "Neuron.h"
#include <vector>
#include <unordered_map>
#include <array>

class World3D
{
	struct AbstractPos
	{
		AbstractPos(Neuron* neuron) : point(neuron)
		{

		}
		float value = 0;
		int index = 0;
		Neuron* point = nullptr;
	};
	std::vector<AbstractPos> x_axis, y_axis, z_axis;
	std::unordered_map<Neuron, AbstractPos*> hash;

public:
	void insert(Neuron pneuron);
	Neuron* nearest(Neuron& pneuron);
	std::vector<Neuron> nearest(Neuron& pneuron, int count);
	std::vector<Neuron> nearest(Neuron& pneuron, float max_distance);
};