#include <pthread.h>

// FIXME: delete me
#include <stdio.h>
#include <stdlib.h>

#ifndef TS_QUEUE_HPP
#define TS_QUEUE_HPP

#define DEFAULT_BUFFER_SIZE 200

template <class T>
class TSQueue {
public:
	// constructor
	TSQueue();

	explicit TSQueue(int max_buffer_size);

	// destructor
	~TSQueue();

	// add an element to the end of the queue
	void enqueue(T item);

	// remove and return the first element of the queue
	T dequeue();

	// return the number of elements in the queue
	int get_size();
private:
	// the maximum buffer size
	int buffer_size;
	// the buffer containing values of the queue
	T* buffer;
	// the current size of the buffer
	int size;
	// the index of first item in the queue
	int head;
	// the index of last item in the queue
	int tail;

	// pthread mutex lock
	pthread_mutex_t mutex;
	// pthread conditional variable
	pthread_cond_t cond_dequeue;
	// pthread conditional variable
	pthread_cond_t cond_enqueue;
};

// Implementation start

template <class T>
TSQueue<T>::TSQueue() : TSQueue(DEFAULT_BUFFER_SIZE) {
}

template <class T>
TSQueue<T>::TSQueue(int buffer_size) : buffer_size(buffer_size) {
	// TODO: implements TSQueue constructor
	size = 0;
	tail = 0;
	head = 0;

	pthread_cond_init(&cond_enqueue, NULL);
	pthread_cond_init(&cond_dequeue, NULL);
	pthread_mutex_init(&mutex, NULL);

	buffer = new T[buffer_size];
}

template <class T>
TSQueue<T>::~TSQueue() {
	// TODO: implenents TSQueue destructor
	pthread_cond_destroy(&cond_enqueue);
	pthread_cond_destroy(&cond_dequeue);
	pthread_mutex_destroy(&mutex);

	delete buffer;
}

template <class T>
void TSQueue<T>::enqueue(T item) {
	// TODO: enqueues an element to the end of the queue
	pthread_mutex_lock(&mutex);
	// printf("Enqueue get lock, %d\n", size);

	// 	Want to work but can't -> wait dequeue to signal me
	while (size >= buffer_size) {
		printf("Enqueue release lock and sleep, %d\n", size);
		// pthread_cond_wait(&cond_enqueue, &mutex);
	}


	// FIXME: not sure is the correct way to work
	buffer[tail] = item;
	tail = (tail + 1) % buffer_size;
	size++;

	// Have work for dequeue to do -> wake dequeue up
	if (size > 0) {
		// printf("Wake dequeue up.\n");
		pthread_cond_signal(&cond_dequeue);
	}

	// printf("Enqueue release lock, %d\n", size);
	pthread_mutex_unlock(&mutex);
}

// enq:

template <class T>
T TSQueue<T>::dequeue() {
	// TODO: dequeues the first element of the queue
	pthread_mutex_lock(&mutex);
	// printf("Dequeue get lock, %d\n", size);

	// want to dequeue but nothing to dequeue -> wait until I have work to do
	while (size <= 0) {
		// printf("Dequeue release lock and sleep, %d\n", size);
		pthread_cond_wait(&cond_dequeue, &mutex);
	}

	// FIXME: not sure is the correct way to work
	T result = buffer[head];
	head = (head + 1) % buffer_size;
	size--;
	// printf("head: %d\n", prevHead);
	// printf("head: %d\n", head);
	
	// Have job for enqueue to do -> wake it up
	if (size < buffer_size) {
		// printf("Wake enqueue up.\n");
		pthread_cond_signal(&cond_enqueue);
	}

	// printf("Dequeue release lock, %d\n", size);
	pthread_mutex_unlock(&mutex);
	
	return result;
}

template <class T>
int TSQueue<T>::get_size() {
	// TODO: returns the size of the queue
	return size;
}

#endif // TS_QUEUE_HPP
