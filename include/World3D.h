#pragma once
#include "Neuron.h"
#include <map>
#include <unordered_map>
#include <array>

class World3D
{
	struct AbstractPos
	{
		std::shared_ptr<Neuron> point;
		bool ignore = false;

		AbstractPos(std::shared_ptr<Neuron> neuron)
		{
			point = neuron;
		}
	};

	template <int axis>
	struct ABPComp
	{
		bool operator()(const NeuronPos3D& a, const NeuronPos3D& b) const { return a[axis] < b[axis]; }
	};

	using AxisX = std::map < NeuronPos3D, std::shared_ptr<AbstractPos>, ABPComp<0> >;
	using AxisY = std::map < NeuronPos3D, std::shared_ptr<AbstractPos>, ABPComp<1> >;
	using AxisZ = std::map < NeuronPos3D, std::shared_ptr<AbstractPos>, ABPComp<2> >;
	AxisX x_axis;
	AxisY y_axis;
	AxisZ z_axis;
	float distance(NeuronPos3D&, NeuronPos3D&);
	std::pair<std::shared_ptr<Neuron>, float> compare(std::shared_ptr<Neuron>& base, std::shared_ptr<Neuron>& a, std::shared_ptr<Neuron>& b, float distance_p);
	std::pair<std::shared_ptr<Neuron>, float> compare(std::shared_ptr<Neuron>& base, std::shared_ptr<Neuron>& a);

	template <class T>
	std::pair<std::shared_ptr<Neuron>, float> subsearch(typename T::iterator iter, std::shared_ptr<Neuron>& base, T& axis);
	template <class T>
	std::pair<std::shared_ptr<Neuron>, float> subsearch(typename T::iterator iter, std::shared_ptr<Neuron>& base, std::shared_ptr<Neuron>& curr_nearest, T& axis, float pdistance);

public:
	void insert(std::shared_ptr<Neuron>& pneuron);
	std::shared_ptr<Neuron> nearest(std::shared_ptr<Neuron>& pneuron);
	//std::vector<Neuron> nearest(Neuron* pneuron, int count);
	//std::vector<Neuron> nearest(Neuron* pneuron, float max_distance);
};