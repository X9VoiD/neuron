#include "../include/World3D.h"
#include <utility>
#include <iostream>
#include <sstream>

void World3D::insert(Neuron* pneuron)
{
	y_axis[pneuron->get_state()->position] = pneuron;
	z_axis[pneuron->get_state()->position] = pneuron;
	x_axis[pneuron->get_state()->position] = pneuron;
}

float World3D::distance(NeuronPos3D& a, NeuronPos3D& b)
{
	return std::sqrt(std::pow((a.at(0) - b.at(0)), 2) +
	                 std::pow((a.at(1) - b.at(1)), 2) +
	                 std::pow((a.at(2) - b.at(2)), 2));
}

std::pair<Neuron*, float> World3D::compare(Neuron* base, Neuron* a, Neuron* b, float distance_p)
{
	float distance_a = distance(base->get_state()->position, b->get_state()->position);
	return (distance_a < distance_p) ? std::pair<Neuron*, float>(b, distance_a):
	                                   std::pair<Neuron*, float>(a, distance_p);
}

std::pair<Neuron*, float> World3D::compare(Neuron* base, Neuron* a)
{
	float distance_a = distance(base->get_state()->position, a->get_state()->position);
	return std::pair<Neuron*, float>(a, distance_a);
}

template <class T>
auto World3D::find_good_point(Neuron* base, typename T::iterator iter, T& axis, unsigned int raxis, bool positive) -> typename T::iterator
{
	unsigned int axis_a, axis_b;
	NeuronPos3D& bpos = base->get_state()->position;
	if (raxis == 0) { axis_a = 1; axis_b = 2; }
	else if (raxis == 1) { axis_a = 0; axis_b = 2; }
	else if (raxis == 2) { axis_a = 0; axis_b = 1; }
	if (positive)
	{
		while (true)
		{
			iter = std::next(iter);
			if (iter == axis.end())
			{
				std::stringstream e;
				e << "No valid point for axis " << raxis << " on direction " << positive << std::endl;
				throw e.str();
			}
			NeuronPos3D& pos = iter->second->get_state()->position;
			float max_diff = std::abs(pos[raxis] - bpos[raxis]);
			bool valid_a = !(std::abs(pos[axis_a] - bpos[axis_a]) > max_diff);
			bool valid_b = !(std::abs(pos[axis_b] - bpos[axis_b]) > max_diff);
			if (valid_a && valid_b)
			{
				return iter;
			}
		}
	}
	else
	{
		while (true)
		{
			iter = std::prev(iter);
			if (iter == axis.end())
			{
				throw "No valid point.";
			}
			NeuronPos3D& pos = iter->second->get_state()->position;
			float max_diff = std::abs(pos[raxis] - bpos[raxis]);
			bool valid_a = !(std::abs(pos[axis_a] - bpos[axis_a]) > max_diff);
			bool valid_b = !(std::abs(pos[axis_b] - bpos[axis_b]) > max_diff);
			if (valid_a && valid_b)
			{
				return iter;
			}
		}
	}
}

template <class T>
std::pair<Neuron*, float> World3D::subsearch(typename T::iterator iter, Neuron* base, T& axis, unsigned int raxis)
{
	float pdistance = 0.0f;
	bool point_found = false;
	typename T::iterator piter, niter;
	Neuron* pneuron; Neuron* nneuron; Neuron* snearest;
	if ((iter != --(axis.end())) && (iter != axis.begin()))
	{
		std::pair<Neuron*, float> x;
		try {
			piter = World3D::find_good_point<T>(base, iter, axis, raxis, true);
			pneuron = piter->second;
			x = compare(base, pneuron);
			snearest = x.first;
			pdistance = x.second;
			point_found = true;
		}
		catch(...) {}
		try {
			niter = World3D::find_good_point<T>(base, iter, axis, raxis, false);
			nneuron = niter->second;
			if (point_found) {
				x = compare(base, pneuron, nneuron, pdistance);
			}
			else {
				x = compare(base, nneuron);
			}
			snearest = x.first;
			pdistance = x.second;
			point_found = true;
		}
		catch(...) {}
	}
	else if (iter == axis.begin())
	{
		try {
			piter = World3D::find_good_point<T>(base, iter, axis, raxis, true);
			point_found = true;
			pneuron = piter->second;
			auto x = compare(base, pneuron);
			snearest = x.first;
			pdistance = x.second;
		}
		catch (...) {}
	}
	else if (iter == --(axis.end()))
	{
		try {
			niter = World3D::find_good_point<T>(base, iter, axis, raxis, false);
			point_found = true;
			nneuron = niter->second;
			auto x = compare(base, nneuron);
			snearest = x.first;
			pdistance = x.second;
		}
		catch (...) {}
	}
	if (!point_found)
	{
		throw "No point found.";
	}
	return std::pair<Neuron*, float>(snearest, pdistance);
}

template <class T>
std::pair<Neuron*, float> World3D::subsearch(typename T::iterator iter, Neuron* base, Neuron* curr_nearest, T& axis, float pdistance, unsigned int raxis)
{
	bool point_found = false;
	typename T::iterator piter, niter;
	Neuron* pneuron; Neuron* nneuron; Neuron* snearest;
	if ((iter != --(axis.end())) && (iter != axis.begin()))
	{
		try {
			piter = find_good_point(base, iter, axis, raxis, true);
			point_found = true;
			pneuron = piter->second;
			auto x = compare(base, curr_nearest, pneuron, pdistance);
			snearest = x.first;
			pdistance = x.second;
		}
		catch (...) {}
		try {
			niter = find_good_point(base, iter, axis, raxis, false);
			point_found = true;
			nneuron = niter->second;
			auto x = compare(base, curr_nearest, nneuron, pdistance);
			snearest = x.first;
			pdistance = x.second;
		}
		catch (...) {}
	}
	else if (iter == axis.begin())
	{
		try {
			piter = find_good_point(base, iter, axis, raxis, true);
			point_found = true;
			pneuron = piter->second;
			auto x = compare(base, curr_nearest, pneuron, pdistance);
			snearest = x.first;
			pdistance = x.second;
		}
		catch (...) {}
	}
	else if (iter == --(axis.end()))
	{
		try {
			niter = find_good_point(base, iter, axis, raxis, false);
			point_found = true;
			nneuron = niter->second;
			auto x = compare(base, curr_nearest, nneuron, pdistance);
			snearest = x.first;
			pdistance = x.second;
		}
		catch (...) {}
	}
	if (!point_found) 
	{
		throw "No point found.";
	}
	return std::pair<Neuron*, float>(snearest, pdistance);
}

Neuron* World3D::nearest(Neuron* pneuron)
{
	float pdistance     = 0.0f;
	bool point_found = false;
	auto pos = pneuron->get_state()->position;
	Neuron* nearest;
	std::pair<Neuron*, float> pair;

	try {
		pair = subsearch<World3D::AxisX>(x_axis.find(pos), pneuron, x_axis, 0);
		point_found = true;
		nearest = pair.first;
		pdistance = pair.second;
	}
	catch (...) { std::cout << "X-Axis invalid search.\n"; }

	try {
		if (point_found) {
			pair = subsearch<World3D::AxisY>(y_axis.find(pos), pneuron, nearest, y_axis, pdistance, 1);
		}
		else {
			pair = subsearch<World3D::AxisY>(y_axis.find(pos), pneuron, y_axis, 1);
		}
		point_found = true;
		nearest = pair.first;
		pdistance = pair.second;
	}
	catch (...) { std::cout << "Y-Axis invalid search.\n"; }

	try {
		if (point_found) {
			pair = subsearch<World3D::AxisZ>(z_axis.find(pos), pneuron, nearest, z_axis, pdistance, 2);
		}
		else {
			pair = subsearch<World3D::AxisZ>(z_axis.find(pos), pneuron, z_axis, 2);
		}
		point_found = true;
		nearest = pair.first;
		pdistance = pair.second;
	}
	catch (...) { std::cout << "Z-Axis invalid search.\n"; }

	if (!point_found)
	{
		throw "No other point found besides current point.";
	}
	return nearest;
}
/*
std::vector<Neuron> World3D::nearest(Neuron* pneuron, int count)
{

}

std::vector<Neuron> World3D::nearest(Neuron* pneuron, float max_distance)
{

}
*/