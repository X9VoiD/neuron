#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include "../include/debug_tool/dNeuron.h"
#include "../include/Neuron.h"
#include "../include/ThreadPool.h"
#include "../include/World3D.h"

/*
		TODO: Axon Growth; Dendrite Signal Convergence; Brain
		TODO: HIGH PRIO: Neuron Spatial Awareness
*/

void circular_network_test(Brain& brain);
void nearest_neuron_search(Brain& brain);

class Brain
{
	std::random_device seed;
	std::mt19937 rng;
	std::vector<std::shared_ptr<Neuron>> neurons;
	std::uniform_real_distribution<float> distribution;
	std::unique_ptr<ThreadPool> worker;
	World3D world;
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
		auto neuron = std::make_shared<Neuron>(gen_rand_position(), gen_rand_position(), gen_rand_position(), &(*worker), neuron_count++);
		//neurons.emplace_back();
		neurons.push_back(neuron);
		worker->register_neuron(&(*neurons.back()));
		world.insert(neurons.back().get());
	}

	void generate_neuron(float x, float y, float z)
	{
		auto neuron = std::make_shared<Neuron>(x, y, z, &(*worker), neuron_count++);
		//neurons.emplace_back();
		neurons.push_back(neuron);
		worker->register_neuron(&(*neurons.back()));
		world.insert(neurons.back().get());
	}

	std::vector<std::shared_ptr<Neuron>>& get_neurons() noexcept
	{
		return neurons;
	}

	World3D& get_world() noexcept
	{
		return world;
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
		float brain_size = 20;

		{
			std::cout << "Brain World Size (x, y, z) (default = 20): ";
			std::string input;
			std::getline(std::cin, input);
			if (!input.empty()) {
				std::istringstream stream(input);
				stream >> brain_size;
			}
		}

		Brain brain = Brain(brain_size);

		std::cout << "Choose test:\n"
			<< "    1.) Circular network.\n"
			<< "    2.) Nearest Neuron Search.\n";

		int iinput;
		{
			std::string input;
			std::getline(std::cin, input);
			if (!input.empty())
			{
				std::istringstream stream(input);
				stream >> iinput;
			}
		}

		switch (iinput)
		{
		case 1:
			circular_network_test(brain);
			break;
		case 2:
			nearest_neuron_search(brain);
			break;
		}
	}
	catch (...)
	{
		
	}
}

void circular_network_test(Brain& brain)
{
	std::cout << "\ncircular_network_test() initiated.\n";
	// Construct small circular network for debugging purposes

	uint64_t spawn_count = 0;

	std::cout << "Amount of Neuron: ";
	std::cin >> spawn_count;
	std::cin.ignore(INT_MAX, '\n');

	for (int i = 0; i != spawn_count; i++)
	{
		brain.generate_neuron();
	}

	std::cout << "Brain construction success.\n";

	auto brain_neurons = &(brain.get_neurons());
	std::cout << "Verified neuron count: " << brain.get_neurons().size() << std::endl;


	for (uint64_t i = 0; i != spawn_count - 1; i++)
	{
		meta_neuron::form_link(*brain_neurons->at(i), *brain_neurons->at(i + 1), true);
	}

	meta_neuron::form_link(*brain_neurons->at(spawn_count - 1), *brain_neurons->at(0), true);
	meta_neuron::prepare_fire(*brain_neurons->at(0));

	std::cout << "Simulation started. Press Enter to stop.\n";
	brain.start();
	std::cin.get();

	std::cout << "Attempting Brain Shutdown.\n";
	brain.shutdown();

	std::cout << "Brain Shutdown Successful.\n";
}

void nearest_neuron_search(Brain& brain)
{
	std::cout << "nearest_neuron_search() initiated.\n";
	brain.generate_neuron(0.0f, 0.0f, 0.0f);
	brain.generate_neuron(2.0f, 2.0f, 2.0f);
	brain.generate_neuron(0.0f, 2.0f, 0.0f);
	brain.generate_neuron(1.2f, 2.0f, -0.3f);
	brain.generate_neuron(0.12f, 0.12f, 0.12f);
	brain.generate_neuron(2.3f, 2.1f, 2.7f);
	brain.generate_neuron(-0.01f, 0.1f, 0.0f);
	while (true)
	{
		std::cout << "Neuron ID Input: ";
		int iinput;
		{
			std::string input;
			std::getline(std::cin, input);
			if (!input.empty() && (input != "q"))
			{
				std::istringstream stream(input);
				stream >> iinput;
			}
			else if (input == "q")
			{
				break;
			}
		}
		auto neuron = brain.get_neurons()[iinput];
		auto a = brain.get_world().nearest(neuron.get());
		std::cout << "Neuron " << neuron->get_state()->id << ": {"
			<< neuron->get_state()->position[0] << ", "
			<< neuron->get_state()->position[1] << ", "
			<< neuron->get_state()->position[2] << "}" << std::endl;
		std::cout << "Neuron " << a->get_state()->id << ": {"
			<< a->get_state()->position[0] << ", "
			<< a->get_state()->position[1] << ", "
			<< a->get_state()->position[2] << "}" << std::endl;
	}
}
