#pragma once
#include "Neuron.h"
#include <map>
#include <unordered_map>
#include <array>

class World3D
{
	struct ABPCompX
	{
		bool operator()(const NeuronPos3D& a, const NeuronPos3D& b) const
		{
			if (!(a[0] < b[0]) && !(a[0] > b[0]))
			{
				if (!(a[1] < b[1]) && !(a[1] > b[1]))
				{
					return a[2] < b[2];
				}
				return a[1] < b[1];
			}
			return a[0] < b[0];
		}
	};

	struct ABPCompY
	{
		bool operator()(const NeuronPos3D& a, const NeuronPos3D& b) const
		{
			if (!(a[1] < b[1]) && !(a[1] > b[1]))
			{
				if (!(a[0] < b[0]) && !(a[0] > b[0]))
				{
					return a[2] < b[2];
				}
				return a[0] < b[0];
			}
			return a[1] < b[1];
		}
	};

	struct ABPCompZ
	{
		bool operator()(const NeuronPos3D& a, const NeuronPos3D& b) const
		{
			if (!(a[2] < b[2]) && !(a[2] > b[2]))
			{
				if (!(a[0] < b[0]) && !(a[0] > b[0]))
				{
					return a[1] < b[1];
				}
				return a[0] < b[0];
			}
			return a[2] < b[2];
		}
	};

	using AxisX = std::map < NeuronPos3D, Neuron*, ABPCompX >;
	using AxisY = std::map < NeuronPos3D, Neuron*, ABPCompY >;
	using AxisZ = std::map < NeuronPos3D, Neuron*, ABPCompZ >;
	AxisX x_axis;
	AxisY y_axis;
	AxisZ z_axis;
	float distance(NeuronPos3D&, NeuronPos3D&);
	std::pair<Neuron*, float> compare(Neuron* base, Neuron* a, Neuron* b, float distance_p);
	std::pair<Neuron*, float> compare(Neuron* base, Neuron* a);

	template <class T>
	auto find_good_point(Neuron* base, typename T::iterator, T& axis, unsigned int raxis, bool positive) -> typename T::iterator;

	template <class T>
	std::pair<Neuron*, float> subsearch(typename T::iterator iter, Neuron* base, T& axis, unsigned int raxis);
	template <class T>
	std::pair<Neuron*, float> subsearch(typename T::iterator iter, Neuron* base, Neuron* curr_nearest, T& axis, float pdistance, unsigned int raxis);

public:
	void insert(Neuron* pneuron);
	Neuron* nearest(Neuron* pneuron);
	//std::vector<Neuron> nearest(Neuron* pneuron, int count);
	//std::vector<Neuron> nearest(Neuron* pneuron, float max_distance);
};
