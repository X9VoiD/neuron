#include <iostream>
#include <random>
#include "../include/debug_tool/dNeuron.h"
#include "../include/Neuron.h"
#include "../include/ThreadPool.h"

/*
		TODO: Axon Growth; Dendrite Signal Convergence; Brain
		TODO: HIGH PRIO: Neuron Spatial Awareness
*/

class Brain
{
	std::random_device seed;
	std::mt19937 rng;
	std::vector<Neuron> neurons;
	std::uniform_real_distribution<float> distribution;
	std::unique_ptr<ThreadPool> worker;
	unsigned int neuron_count = 0;

	inline float gen_rand_position()
	{
		return distribution(rng);
	}

public:
	// TODO
	Brain(float brain_size)
	{
		worker = std::make_unique<ThreadPool>(this);
		constexpr float offset = 2.0;
		constexpr int mem_relax = 5;
		neurons.reserve((std::pow(brain_size, 3)) / mem_relax);
		distribution = std::uniform_real_distribution<float>(-(static_cast<float>(brain_size) / offset),
			(static_cast<float>(brain_size) / offset));
		rng = std::mt19937(seed());
	}

	void generate_neuron()
	{
		neurons.emplace_back(gen_rand_position(), gen_rand_position(), gen_rand_position(), &(*worker), neuron_count++);
		worker->register_neuron(&(neurons.back()));
		// TODO: Implement neuron spatial awareness.
	}

	void generate_neuron(float x, float y, float z)
	{
		neurons.emplace_back(x, y, z, &(*worker), neuron_count++);
		worker->register_neuron(&(neurons.back()));
	}

	std::vector<Neuron>& get_neurons() noexcept
	{
		return neurons;
	}

	void start()
	{
		// TODO: Tickle a random Neuron
		worker->get_barrier()->sync();
	}

	void shutdown()
	{
		worker->shutdown();
		worker->join();
	}
};


int main()
{
	try
	{
		uint64_t spawn_count = 0;
		float brain_size = 20;

		std::cout << "Amount of Neuron: ";
		std::cin >> spawn_count;

		std::cout << "Brain World Size (x, y, z) (default = 20): ";
		std::cin >> brain_size;
		std::cin.ignore(INT_MAX, '\n');

		Brain brain = Brain(brain_size);

		for (int i = 0; i != spawn_count; i++)
		{
			brain.generate_neuron();
		}

		std::cout << "Brain construction success.\n";

		auto brain_neurons = &(brain.get_neurons());
		std::cout << "Verified neuron count: " << brain.get_neurons().size() << std::endl;

		// Construct small circular network for debugging purposes
		for (uint64_t i = 0; i != spawn_count - 1; i++)
		{
			meta_neuron::form_link(brain_neurons->at(i), brain_neurons->at(i + 1), true);
		}

		meta_neuron::form_link(brain_neurons->at(spawn_count - 1), brain_neurons->at(0), true);
		meta_neuron::prepare_fire(brain_neurons->at(0));

		std::cout << "Simulation started. Press Enter to stop.\n";
		brain.start();
		std::cin.get();

		std::cout << "Attempting Brain Shutdown.\n";
		brain.shutdown();

		std::cout << "Brain Shutdown Successful.\n";
	}
	catch (...)
	{
		
	}
}
