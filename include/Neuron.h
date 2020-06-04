#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <array>

#include "ThreadPool.h"

using pulse_distance = unsigned int;
using axon_distance = unsigned int;

class meta_neuron;

class ThreadPool;

class Neuron
{
	friend class meta_neuron;

	struct NeuronState
	{
		unsigned int id;
		bool hot = false;
		float polarization = 0.0f;
		std::array<float, 3> position;
		float pulse_resistance;
		unsigned int nconnections;
		ThreadPool* worker;

		NeuronState(float x, float y, float z, float p_pulse_resistance,
			ThreadPool* p_worker, unsigned int nid) noexcept :
			position({ x, y, z }),
			pulse_resistance(p_pulse_resistance),
			worker(p_worker),
			id(nid),
			nconnections(0)
		{

		}
	};

	class Axon;
	class CollectiveDendrite;

	std::shared_ptr<Axon> axon;
	std::shared_ptr<CollectiveDendrite> dendrite;
	std::shared_ptr<NeuronState> state;

public:
	Neuron(float, float, float, ThreadPool*, unsigned int);

	~Neuron() = default;
	Neuron& operator=(const Neuron&) = delete;
	Neuron(const Neuron&) = delete;
	Neuron& operator=(Neuron&&) = default;
	Neuron(Neuron&&) = default;

	void update();
	inline const std::shared_ptr<NeuronState>& get_state() noexcept { return state; }
	inline const std::shared_ptr<CollectiveDendrite>& get_dendrite() noexcept { return dendrite; }
	inline const std::shared_ptr<Axon>& get_axon() noexcept { return axon; }
};



class Neuron::Axon
{
	friend class meta_neuron;

	struct AxonTarget
	{
		CollectiveDendrite* target;
		float distance;
		float reach;

		AxonTarget(CollectiveDendrite* ptarget, float pdistance, float preach) noexcept :
			target(ptarget), distance(pdistance), reach(preach)
		{

		}
	};
	Axon* id;
	std::vector<AxonTarget> targets;
	std::vector<AxonTarget> candidates;
	std::shared_ptr<NeuronState> neuron_state;

public:
	explicit Axon(const std::shared_ptr<NeuronState>& pneuron_state) :
		neuron_state(pneuron_state), id(this)
	{

	}

	~Axon() = default;
	Axon& operator=(const Axon&) = delete;
	Axon(const Axon&) = delete;
	Axon& operator=(Axon&&) = delete;
	Axon(Axon&&) = delete;

	inline const std::shared_ptr<NeuronState>& get_state() noexcept { return neuron_state; }
	void send_pulse();
	void update();
};



class Neuron::CollectiveDendrite
{
	friend class meta_neuron;

	struct Link
	{
		std::vector<pulse_distance> pulses;
		int length = 0;
		float weight = 1.0f;
	};
	std::unordered_map<Axon*, std::unique_ptr<Link>> links;
	std::shared_ptr<NeuronState> neuron_state;
	std::vector<unsigned int> pulse_del_buffer;

	void travel_pulses();
	void collect_pulse(Axon*, const Link&);
	void update_link_strength(Axon*, bool);

public:
	explicit CollectiveDendrite(const std::shared_ptr<NeuronState>& pneuron_state) :
		neuron_state(pneuron_state)
	{

	}

	~CollectiveDendrite() = default;
	CollectiveDendrite& operator=(const CollectiveDendrite&) = delete;
	CollectiveDendrite(const CollectiveDendrite&) = delete;
	CollectiveDendrite& operator=(CollectiveDendrite&&) = delete;
	CollectiveDendrite(CollectiveDendrite&&) = delete;

	void form_link(Axon*, int);
	void remove_link(Axon*);
	void receive_pulse(Axon*);
	void update();

	inline const std::shared_ptr<NeuronState>& get_state() noexcept { return neuron_state; }
};
