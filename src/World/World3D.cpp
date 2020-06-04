#include "../include/World3D.h"

void World3D::insert(Neuron pneuron)
{
	auto x = std::make_unique<AbstractPos>(&pneuron);
	hash[pneuron] = &(*x);
}

Neuron* World3D::nearest(Neuron& pneuron)
{

}

std::vector<Neuron> World3D::nearest(Neuron& pneuron, int count)
{

}

std::vector<Neuron> World3D::nearest(Neuron& pneuron, float max_distance)
{

}