#include <pthread.h>
#include <stdio.h>
#include "thread.hpp"
#include "ts_queue.hpp"
#include "item.hpp"
#include "transformer.hpp"

#ifndef CONSUMER_HPP
#define CONSUMER_HPP

class Consumer : public Thread {
public:
	// constructor
	Consumer(TSQueue<Item*>* worker_queue, TSQueue<Item*>* output_queue, Transformer* transformer);

	// destructor
	~Consumer();

	virtual void start() override;

	virtual int cancel() override;
private:
	TSQueue<Item*>* worker_queue;
	TSQueue<Item*>* output_queue;

	Transformer* transformer;

	bool is_cancel;

	// the method for pthread to create a consumer thread
	static void* process(void* arg);
};

Consumer::Consumer(TSQueue<Item*>* worker_queue, TSQueue<Item*>* output_queue, Transformer* transformer)
	: worker_queue(worker_queue), output_queue(output_queue), transformer(transformer) {
	is_cancel = false;
}

Consumer::~Consumer() {}

void Consumer::start() {
	// TODO: starts a Consumer thread
	pthread_create(&t, 0, Consumer::process, (void*)this);
}

int Consumer::cancel() {
	// TODO: cancels the consumer thread
	// FIXME: pretty sure this is the wrong way.
	return pthread_cancel(t);
}

void* Consumer::process(void* arg) {
	Consumer* consumer = (Consumer*)arg;
	Item *input, *output;
	unsigned long long newVal;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, nullptr);

	while (!consumer->is_cancel) {
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);

		// TODO: implements the Consumer's work
		if (consumer->worker_queue->get_size() > 0) {
			input = consumer->worker_queue->dequeue();
			newVal = consumer->transformer->consumer_transform(input->opcode, input->val);
			output = new Item(input->key, newVal, input->opcode);
			consumer->output_queue->enqueue(output);
		}

		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
	}

	delete consumer;

	return nullptr;
}

#endif // CONSUMER_HPP
