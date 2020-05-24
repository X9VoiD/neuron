#pragma once
#include <memory>
#include <vector>
#include <unordered_map>

class Neuron {
	struct NeuronState;

	class Axon;
	class CollectiveDendrite;

	std::shared_ptr<Axon> axon;
	std::shared_ptr<CollectiveDendrite> dendrite;
	std::shared_ptr<NeuronState> state;

public:
	Neuron(float, float, float);

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



class Neuron::Axon {
public:
	Axon(const std::shared_ptr<NeuronState>&);

	~Axon() = default;
	Axon& operator=(const Axon&) = delete;
	Axon(const Axon&) = delete;
	Axon& operator=(Axon&&) = delete;
	Axon(Axon&&) = delete;

	void send_pulse();
	void update();

private:
	struct AxonTarget;
	Axon* id;
	std::vector<AxonTarget> targets;
	std::vector<AxonTarget> candidates;
	std::shared_ptr<NeuronState> neuron_state;
};



class Neuron::CollectiveDendrite {
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

private:
	struct Link;
	std::unordered_map<Axon*, std::unique_ptr<Link>> links;
	std::shared_ptr<NeuronState> neuron_state;
};