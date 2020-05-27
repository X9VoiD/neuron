#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <array>

#include "ThreadPool.h"

using pulse_distance = unsigned int;
using axon_distance = unsigned int;

class Neuron
{
	struct NeuronState
	{
		NeuronState(float, float, float, float, ThreadPool*);

		bool hot = false;
		float polarization = 0.0f;
		std::array<float, 3> position;
		float pulse_resistance;
		unsigned int nconnections;
		ThreadPool* worker;
	};

	class Axon;
	class CollectiveDendrite;

	std::shared_ptr<Axon> axon;
	std::shared_ptr<CollectiveDendrite> dendrite;
	std::shared_ptr<NeuronState> state;

public:
	Neuron(float, float, float, ThreadPool*);

	~Neuron() = default;
	Neuron& operator=(const Neuron&) = delete;
	Neuron(const Neuron&) = delete;
	Neuron& operator=(Neuron&&) = default;
	Neuron(Neuron&&) = default;

	void update();
	inline const std::shared_ptr<NeuronState>& get_state();
	inline const std::shared_ptr<CollectiveDendrite>& get_dendrite();
	inline const std::shared_ptr<Axon>& get_axon();
};



class Neuron::Axon
{
public:
	Axon(const std::shared_ptr<NeuronState>&);

	~Axon() = default;
	Axon& operator=(const Axon&) = delete;
	Axon(const Axon&) = delete;
	Axon& operator=(Axon&&) = delete;
	Axon(Axon&&) = delete;

	inline const std::shared_ptr<NeuronState>& get_state();
	void send_pulse();
	void update();

private:
	struct AxonTarget
	{
		CollectiveDendrite* target;
	};
	Axon* id;
	std::vector<AxonTarget> targets;
	std::vector<AxonTarget> candidates;
	std::shared_ptr<NeuronState> neuron_state;
};



class Neuron::CollectiveDendrite
{
public:
	CollectiveDendrite(const std::shared_ptr<NeuronState>&);

	~CollectiveDendrite() = default;
	CollectiveDendrite& operator=(const CollectiveDendrite&) = delete;
	CollectiveDendrite(const CollectiveDendrite&) = delete;
	CollectiveDendrite& operator=(CollectiveDendrite&&) = delete;
	CollectiveDendrite(CollectiveDendrite&&) = delete;

	void form_link(Axon*, int);
	void remove_link(Axon*);
	void receive_pulse(Axon*);
	void update();

	inline const std::shared_ptr<NeuronState>& get_state();

private:
	struct Link
	{
		std::vector<pulse_distance> pulses;
		int length = 0;
		float weight = 1.0f;
	};
	std::unordered_map<Axon*, std::unique_ptr<Link>> links;
	std::shared_ptr<NeuronState> neuron_state;

	void travel_pulses();
	void collect_pulse(Axon*, const std::unique_ptr<Link>&);
	void update_link_strength(Axon*, bool);
};