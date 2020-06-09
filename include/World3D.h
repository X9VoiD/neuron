#pragma once
#include "Neuron.h"
#include <map>
#include <unordered_map>
#include <array>

class World3D
{
	struct AbstractPos
	{
		Neuron* point;
		bool ignore = false;

		AbstractPos(Neuron* neuron)
		{
			point = neuron;
		}
	};

	template <int axis>
	struct ABPComp
	{
		bool operator()(const NeuronPos3D& a, const NeuronPos3D& b) const { return a[axis] < b[axis]; }
	};

	using AxisX = std::map < NeuronPos3D, std::unique_ptr<AbstractPos>, ABPComp<0> >;
	using AxisY = std::map < NeuronPos3D, AbstractPos*, ABPComp<1> >;
	using AxisZ = std::map < NeuronPos3D, AbstractPos*, ABPComp<2> >;
	AxisX x_axis;
	AxisY y_axis;
	AxisZ z_axis;
	float distance(NeuronPos3D&, NeuronPos3D&);
	std::pair<Neuron*, float> compare(Neuron* base, Neuron* a, Neuron* b, float distance_p);
	std::pair<Neuron*, float> compare(Neuron* base, Neuron* a);

	template <class T>
	std::pair<Neuron*, float> subsearch(typename T::iterator iter, Neuron* base, T& axis);
	template <class T>
	std::pair<Neuron*, float> subsearch(typename T::iterator iter, Neuron* base, Neuron* curr_nearest, T& axis, float pdistance);

public:
	void insert(Neuron* pneuron);
	Neuron* nearest(Neuron* pneuron);
	//std::vector<Neuron> nearest(Neuron* pneuron, int count);
	//std::vector<Neuron> nearest(Neuron* pneuron, float max_distance);
};