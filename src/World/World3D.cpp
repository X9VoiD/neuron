#include "../include/World3D.h"
#include <utility>

void World3D::insert(std::shared_ptr<Neuron>& pneuron)
{
	auto ab = std::make_shared<AbstractPos>(pneuron);
	x_axis[pneuron->get_state()->position] = ab;
	y_axis[pneuron->get_state()->position] = ab;
	z_axis[pneuron->get_state()->position] = ab;
}

float World3D::distance(NeuronPos3D& a, NeuronPos3D& b)
{
	return std::sqrt(std::pow((a.at(0) - b.at(0)), 2) +
	                 std::pow((a.at(1) - b.at(1)), 2) +
	                 std::pow((a.at(2) - b.at(2)), 2));
}

std::pair<std::shared_ptr<Neuron>, float> World3D::compare(std::shared_ptr<Neuron>& base,
	std::shared_ptr<Neuron>& a, std::shared_ptr<Neuron>& b, float distance_p)
{
	float distance_a = distance(base->get_state()->position, b->get_state()->position);
	return (distance_a < distance_p) ? std::pair<std::shared_ptr<Neuron>, float>(b,  distance_a):
	                                   std::pair<std::shared_ptr<Neuron>, float>(a, distance_p);
}

std::pair<std::shared_ptr<Neuron>, float> World3D::compare(std::shared_ptr<Neuron>& base, std::shared_ptr<Neuron>& a)
{
	float distance_a = distance(base->get_state()->position, a->get_state()->position);
	return std::pair<std::shared_ptr<Neuron>, float>(a, distance_a);
}

template <class T>
std::pair<std::shared_ptr<Neuron>, float> World3D::subsearch(typename T::iterator iter, std::shared_ptr<Neuron>& base, T& axis)
{
	float pdistance = 0.0f;
	World3D::AxisX::iterator piter, niter;
	std::shared_ptr<Neuron>  pneuron, nneuron, snearest;
	if ((iter != --(axis.end())) && (iter != axis.begin()))
	{
		piter = iter; piter++;
		pneuron = piter->second->point;
		auto x = compare(base, pneuron);
		snearest = x.first;
		pdistance = x.second;
		niter = iter; niter--;
		nneuron = niter->second->point;
		x = compare(base, pneuron, nneuron, pdistance);
		snearest = x.first;
		pdistance = x.second;
	}
	else if (iter == axis.begin())
	{
		piter = iter; piter++;
		pneuron = piter->second->point;
		auto x = compare(base, pneuron);
		snearest = x.first;
		pdistance = x.second;
	}
	else if (iter == --(axis.end()))
	{
		niter = iter; niter--;
		nneuron = niter->second->point;
		auto x = compare(base, nneuron);
		snearest = x.first;
		pdistance = x.second;
	}
	return std::pair<std::shared_ptr<Neuron>, float>(snearest, pdistance);
}

template <class T>
std::pair<std::shared_ptr<Neuron>, float> World3D::subsearch(typename T::iterator iter, std::shared_ptr<Neuron>& base, std::shared_ptr<Neuron>& curr_nearest, T& axis, float pdistance)
{
	World3D::AxisX::iterator piter, niter;
	std::shared_ptr<Neuron>  pneuron, nneuron, snearest;
	if ((iter != --(axis.end())) && (iter != axis.begin()))
	{
		piter = iter; piter++;
		pneuron = piter->second->point;
		auto x = compare(base, curr_nearest, pneuron, pdistance);
		snearest = x.first;
		pdistance = x.second;
		niter = iter; niter--;
		nneuron = niter->second->point;
		x = compare(base, curr_nearest, nneuron, pdistance);
		snearest = x.first;
		pdistance = x.second;
	}
	else if (iter == axis.begin())
	{
		piter = iter; piter++;
		pneuron = piter->second->point;
		auto x = compare(base, curr_nearest, pneuron, pdistance);
		snearest = x.first;
		pdistance = x.second;
	}
	else if (iter == --(axis.end()))
	{
		niter = iter; niter--;
		nneuron = niter->second->point;
		auto x = compare(base, curr_nearest, nneuron, pdistance);
		snearest = x.first;
		pdistance = x.second;
	}
	return std::pair<std::shared_ptr<Neuron>, float>(snearest, pdistance);
}

std::shared_ptr<Neuron> World3D::nearest(std::shared_ptr<Neuron>& pneuron)
{
	float pdistance     = 0.0f;
	auto pos = pneuron->get_state()->position;
	std::shared_ptr<Neuron> nearest;

	auto pair = subsearch<World3D::AxisX>(x_axis.find(pos), pneuron, x_axis);
	nearest   = pair.first;
	pdistance = pair.second;

	pair      = subsearch<World3D::AxisY>(y_axis.find(pos), pneuron, nearest, y_axis, pdistance);
	nearest   = pair.first;
	pdistance = pair.second;

	pair      = subsearch<World3D::AxisZ>(z_axis.find(pos), pneuron, nearest, z_axis, pdistance);
	nearest   = pair.first;
	pdistance = pair.second;
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