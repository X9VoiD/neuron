#include <iostream>
#include <cstdio>
#include <thread>
#include <exception>
#include <algorithm>
#include <vector>
#include <unordered_map>
constexpr auto DENDRITECHUNK = 8;
constexpr auto ACTIVATIONBIAS = 0;

/*
		TODO: Axon-Dendrite Linking; Axon Reach; Brain
*/

class Neuron
{
	public:
		Neuron(float x, float y, float z)
		{
			activationbias = ACTIVATIONBIAS;
			dendrite = new CollectiveDendrite(this, x, y, z);
			axon = new Axon();
			nconnections = 0;
		}

		~Neuron()
		{
			delete dendrite;
			delete axon;
		}

		void update()
		{
		}

	private:
		int activationbias;
		int nconnections;

		struct NeuronState
		{
			unsigned int hot;
			float position[3];
		};

		// CollectiveDendrite Start
		class CollectiveDendrite
		{
			public:
				CollectiveDendrite(Neuron *nparent, float x, float y, float z)
				{
					parent = nparent;
					current_nchunk = DENDRITECHUNK;
					pool = (int *)malloc(DENDRITECHUNK * 2 * sizeof(int));
					if (pool == nullptr)
					{
						std::cout << "Error: Dendrite Pool of Neuron";
						std::terminate();
					}
					hot = 0;
					position[0] = x;
					position[1] = y;
					position[2] = z;
					current_nlink = 0;
					fires_received = 0;
				}

				~CollectiveDendrite()
				{
					free(pool);
				}

				void form_link(int64_t amount)
				{
					if ((current_nchunk * DENDRITECHUNK) < (current_nlink + amount * DENDRITECHUNK))
					{
						int *tmp = (int *)realloc(pool, (current_nchunk + 1) * DENDRITECHUNK * sizeof(int));
						if (tmp == nullptr)
						{
							std::cout << "Error: Dendrite chunk failed";
							std::terminate();
						}
						pool = tmp;
						current_nchunk += 1;
					}
				}

				void receive(int pulse, int length)
				{
					*(pool + (current_nlink * 2)) = pulse;
					*(pool + ((current_nlink * 2) + 1)) = length;
					fires_received++;
				}

				void unlink(int amount)
				{
					current_nlink -= amount;
					if ((current_nchunk * DENDRITECHUNK) - current_nlink > DENDRITECHUNK)
					{
						current_nchunk--;
						int *tmp = (int *)realloc(pool, current_nchunk * DENDRITECHUNK * sizeof(int));
						if (tmp == nullptr)
						{
							std::cout << "Error: Dendrite Unlink";
							std::terminate();
						}
						pool = tmp;
					}
				}

				int operator[](int i)
				{
					return *(pool + i);
				}

				unsigned int is_hot()
				{
					return hot;
				}

				void be_hot() { hot = 1; }

				void fire_reset() { fires_received = 0; }

			private:
				int64_t current_nchunk;
				int64_t current_nlink;
				unsigned int fires_received;
				int *pool;
				Neuron *parent;
				float position[3];
				unsigned int hot;
		};
		// CollectiveDendrite End

		// Axon Start
		class Axon
		{
			public:
				Axon()
				{
					hot = 0;
				}
				void send_pulse()
				{
					std::for_each(targets.begin(), targets.end(), [](AxonTarget synapse) {
						synapse.target->receive(synapse.pulse, synapse.length);
					});
				}

			private:
				struct AxonTarget
				{
					CollectiveDendrite *target;
					int length; // TODO
					int pulse;
				};
	
				std::vector<AxonTarget> targets;
				std::vector<AxonTarget> candidates;
				unsigned int hot;
		};
		// Axon End

		Axon *axon;
		CollectiveDendrite *dendrite;

	public:
		inline CollectiveDendrite* get_dendrite() { return dendrite; }
		inline Axon* get_axon() { return axon; }
};

int main()
{
	Neuron *n1 = new Neuron(0,0,0);
	n1->update();
	std::cout << "Alpha Boot Success.";
}