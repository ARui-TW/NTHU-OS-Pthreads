#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <chrono>
#include "consumer.hpp"
#include "ts_queue.hpp"
#include "item.hpp"
#include "transformer.hpp"

#ifndef CONSUMER_CONTROLLER
#define CONSUMER_CONTROLLER

class ConsumerController : public Thread {
public:
	// constructor
	ConsumerController(
		TSQueue<Item*>* worker_queue,
		TSQueue<Item*>* writer_queue,
		Transformer* transformer,
		int check_period,
		int low_threshold,
		int high_threshold
	);

	// destructor
	~ConsumerController();

	virtual void start();

private:
	std::vector<Consumer*> consumers;

	TSQueue<Item*>* worker_queue;
	TSQueue<Item*>* writer_queue;

	Transformer* transformer;

	// Check to scale down or scale up every check period in microseconds.
	int check_period;
	// When the number of items in the worker queue is lower than low_threshold,
	// the number of consumers scaled down by 1.
	int low_threshold;
	// When the number of items in the worker queue is higher than high_threshold,
	// the number of consumers scaled up by 1.
	int high_threshold;

	static void* process(void* arg);
};

// Implementation start

ConsumerController::ConsumerController(
	TSQueue<Item*>* worker_queue,
	TSQueue<Item*>* writer_queue,
	Transformer* transformer,
	int check_period,
	int low_threshold,
	int high_threshold
) : worker_queue(worker_queue),
	writer_queue(writer_queue),
	transformer(transformer),
	check_period(check_period),
	low_threshold(low_threshold),
	high_threshold(high_threshold) {
}

ConsumerController::~ConsumerController() {}

void ConsumerController::start() {
	// TODO: starts a ConsumerController thread
	pthread_create(&t, 0, ConsumerController::process, (void*)this);
}

void* ConsumerController::process(void* arg) {
	// TODO: implements the ConsumerController's work
	auto timeStamp = std::chrono::high_resolution_clock::now();
	ConsumerController* cc = (ConsumerController*)arg;
	while (1)
	{
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::micro> diff = now - timeStamp;
		// printf("i: %d\n", i);

		if (diff.count() > cc->check_period)
		{
			// check
			printf("wq: %d\n", cc->worker_queue->get_size());

			if (cc->worker_queue->get_size() > cc->high_threshold)
			{
				// create new consumer thread
				// printf("wq: %d\n", cc->worker_queue->get_size());
				printf("Scaling up consumers from %d to %d.\n", cc->consumers.size(), cc->consumers.size() + 1);
				Consumer* newConsumer = new Consumer(cc->worker_queue, cc->writer_queue, cc->transformer);
				newConsumer->start();
				cc->consumers.push_back(newConsumer);
			}
			else if (cc->worker_queue->get_size() < cc->low_threshold && cc->consumers.size() > 1)
			{
				// printf("wq: %d\n", cc->worker_queue->get_size());
				printf("Scaling down consumers from %d to %d.\n", cc->consumers.size(), cc->consumers.size() - 1);
				Consumer *poped = cc->consumers.back();
				cc->consumers.pop_back();
				poped->cancel();
			}
			timeStamp = std::chrono::high_resolution_clock::now();
		}
	}
}

#endif // CONSUMER_CONTROLLER_HPP
