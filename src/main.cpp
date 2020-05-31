#include <iostream>
#include <random>
#include "../include/debug_tool/dNeuron.h"
#include "../include/Neuron.h"
#include "../include/ThreadPool.h"
constexpr auto BRAINSIZE = 10;

/*
		TODO: Axon Growth; Dendrite Signal Convergence; Brain
		TODO: HIGH PRIO: Neuron Spatial Awareness
*/

class Brain
{
public:
	// TODO
	Brain()
	{
		worker = std::make_unique<ThreadPool>();
		constexpr float brain_world_center = 2.0;
		constexpr int mem_relax = 5;
		neurons.reserve((pow(BRAINSIZE, 3)) / mem_relax);
		distribution = std::uniform_real_distribution<float>(-(static_cast<float>(BRAINSIZE) / brain_world_center),
			(static_cast<float>(BRAINSIZE) / brain_world_center));
		rng = std::mt19937(seed());
	}
	inline float gen_rand_position()
	{
		return distribution(rng);
	}

private:
	std::random_device seed;
	std::mt19937 rng;
	std::vector<Neuron> neurons;
	std::uniform_real_distribution<float> distribution;
	std::unique_ptr<ThreadPool> worker;

public:
	void generate_neuron()
	{
		neurons.emplace_back(gen_rand_position(), gen_rand_position(), gen_rand_position(), &(*worker));
		// TODO: Implement neuron spatial awareness.
	}

	void generate_neuron(float x, float y, float z)
	{
		neurons.emplace_back(x, y, z, &(*worker));
	}

	std::vector<Neuron>& get_neurons()
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
		constexpr auto test_rand = 4;

		auto brain = std::make_unique<Brain>();
		std::cout << "Brain construction success.\n";

		// Start seeding the Brain with random Neurons
		for (int i = 0; i != test_rand; i++)
		{
			brain->generate_neuron();
		}

		std::cout << "Brain Test Run...\n";

		#ifdef NEURON_DEBUG
		std::cout << "Preparing...";
		// Construct small circular network for debugging purposes
		meta_neuron::form_link(brain->get_neurons()[0], brain->get_neurons()[1], true);
		meta_neuron::form_link(brain->get_neurons()[1], brain->get_neurons()[2], true);
		meta_neuron::form_link(brain->get_neurons()[2], brain->get_neurons()[3], true);
		meta_neuron::form_link(brain->get_neurons()[3], brain->get_neurons()[0], true);
		#endif

		std::cout << "Press Enter to run simulation\n";

		std::cin.get();

		brain->start();

		std::cout << "Processing... Press Enter to shutdown the <Brain>";

		std::cin.get();

		std::cout << "Attempting Brain Shutdown.\n";

		brain->shutdown();

		std::cout << "Brain Shutdown Successful.\n";
	}
	catch (...) {}
}