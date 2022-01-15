#include <pthread.h>
#include "thread.hpp"
#include "ts_queue.hpp"
#include "item.hpp"
#include "transformer.hpp"

#ifndef PRODUCER_HPP
#define PRODUCER_HPP

class Producer : public Thread {
public:
	// constructor
	Producer(TSQueue<Item*>* input_queue, TSQueue<Item*>* worker_queue, Transformer* transfomrer);

	// destructor
	~Producer();

	virtual void start();
private:
	TSQueue<Item*>* input_queue;
	TSQueue<Item*>* worker_queue;

	Transformer* transformer;

	// the method for pthread to create a producer thread
	static void* process(void* arg);
};

Producer::Producer(TSQueue<Item*>* input_queue, TSQueue<Item*>* worker_queue, Transformer* transformer)
	: input_queue(input_queue), worker_queue(worker_queue), transformer(transformer) {
}

Producer::~Producer() {}

void Producer::start() {
	// TODO: starts a Producer thread
	pthread_create(&t, 0, Producer::process, (void*)this);
}

void* Producer::process(void* arg) {
	// TODO: implements the Producer's work
	Producer* producer = (Producer*)arg;
	Item *input, *output;
	unsigned long long newVal;

	// FIXME: not sure how the while loop should look like
	// while (producer->input_queue->get_size() > 0) {
	while(1) {
		if (producer->input_queue->get_size() > 0) {
			// printf("%d\n", i++);
			input = producer->input_queue->dequeue();
			newVal = producer->transformer->producer_transform(input->opcode, input->val);
			output = new Item(input->key, newVal, input->opcode);
			producer->worker_queue->enqueue(output);
		}
	}

	return nullptr;
}

#endif // PRODUCER_HPP
