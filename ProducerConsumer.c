/*
 * producerConsumer.c
 * ECE254 Group 01
 * By :  Rushan Yogaratnam and Ameen Patel
 * University of Waterloo Computer Engineering
 * Spring 2014
 *
 *Producer Consumer Threading Implementation:
 * This process has the main thread create
 * new threads for the producers and consumers
 * and passes the messages to a global data-structure.
 *
 * Data is passed to a linked list , but items
 * are only added to the head and are removed
 * from the head. This makes the run-time of the linked
 * list operation O(1).
 *
 *A counting semaphore is used to ensure the buffer
 *size is fixed, hence the linked list does not overflow.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "producerConsumer.h"
#include "common.h"

//List node for the queue
struct queue_element {
	int value;
	struct queue_element* next;
};

//struct for the thread parameters
//this is used to pass the producer
//and consume id's to threads.
struct thread_params {
	int id;
};

//global buffer
struct queue_element* buffer;

//semaphore for the critical sections.
sem_t buff_lock;
//semaphore for the number of messages produces.
sem_t count;
//semaphore for the size of the buffer.
sem_t buff_size;
//semaphore for the consumer
sem_t con_num;

//these are declared global to allow
//the producers to read them.
int production_count;
int producer_count;

int main(int argc, char **argv) {

	int consumer_count;
	int buffer_size;

	if (process_arguments(argc, argv, &buffer_size, &production_count,
			&producer_count, &consumer_count)) {
		printf("Invalid arguments\n");
		return 1;
	}

	//initailze the buffer to null.
	//the buffer is dynamically built up
	//using the add and consume operations.
	//the size is enforced by a semaphore.
	buffer = NULL;

	//initalize semaphores
	sem_init(&buff_lock, 0, 1);
	sem_init(&count, 0, 0);
	sem_init(&buff_size, 0, buffer_size);
	sem_init(&con_num, 0, production_count);

	//array of id's used for joining
	pthread_t p_thread_id[producer_count];
	pthread_t c_thread_id[consumer_count];

	//creates structs to pass to threads
	struct thread_params p_id[producer_count];
	struct thread_params c_id[consumer_count];

	//get time before first fork
	double time_before_first_thread_created = get_time_in_seconds();

	//creates producer threads
	int i;
	for (i = 0; i < producer_count; ++i) {
		p_id[i].id = i;
		pthread_create(&(p_thread_id[i]), NULL, &producer, &(p_id[i]));
	}

	//create consumer threads
	int c;
	for (c = 0; c < consumer_count; ++c) {
		c_id[c].id = c;
		pthread_create(&(c_thread_id[c]), NULL, &consumer, &(c_id[c]));
	}

	int j;
	for (j = 0; j < producer_count; ++j) {
		pthread_join(p_thread_id[j], NULL);
	}

	int k;
	for (k = 0; k < consumer_count; ++k) {
		pthread_join(c_thread_id[k], NULL);
	}

	double time_after_last_consumed = get_time_in_seconds();
	double execution_time = time_after_last_consumed
			- time_before_first_thread_created;

	printf("System execution time: %f seconds\n", execution_time);

	//clean up semaphores
	sem_destroy(&buff_lock);
	sem_destroy(&count);
	sem_destroy(&buff_size);
	sem_destroy(&con_num);

	return 0;
}

/*
 Function is used to add an element to the linked list.
 If the linkedlist is null it creates
 the linked list and sets the value of the head.
 All further calls appends the values to the head
 of the linked list.
 */
void add_to_buffer(int value) {

	if (buffer == NULL) {
		buffer = malloc(sizeof(struct queue_element));
		buffer->value = value;
		buffer->next = NULL;
	} else {
		struct queue_element* new_head = malloc(sizeof(struct queue_element));
		new_head->next = buffer;
		new_head->value = value;
		buffer = new_head;
	}
}
/*
 Function takes an item from the head of the linked list
 reassigned the head and deletes the node from the heap.
 This throws an error if the linked list is null.
 If the message obtained is a square number it is printed out.
 */

void consume_from_buffer(int * c_id) {

	if (buffer == NULL) {
		printf("error failed to read from queue, queue is empty\n");
	} else {

		//get the current head of the buffer
		struct queue_element* current_element;
		current_element = buffer;

		int val = current_element->value;

		//reassign the buffer head
		buffer = buffer->next;
		//delete the previous buffer node.
		free(current_element);

		int root = sqrt(val);
		if (val == (root * root)) {
			printf("%i %i %i\n", *c_id, val, root);
		}
	}

}
/*
 Producer thread function.
 producer producers the set of integers
 that satisfy i%num_producers = p_id;

 Produer first waits till the buffer is not full.
 Producer locks the buffer.
 Producer inserts a value into a buffer.
 Producer unlocks the buffer.
 Producer notifies the buffer is not empty, by calling
 sem_post(&count).
 */
void* producer(void* producer_params) {

	struct thread_params *params = (struct thread_params*) producer_params;
	int p_id = params->id;

	int i;
	for (i = p_id; i < production_count; i += producer_count) {

		//wait until buffer is not full
		sem_wait(&buff_size);

		//toggle the lock
		sem_wait(&buff_lock);

		//add item to buffer
		add_to_buffer(i);

		//release the lock
		sem_post(&buff_lock);

		//notify that the buffer is not empty.
		sem_post(&count);

	}
	return NULL;
}

/*
 Consumer thread function.
 Consumer consumes all integers until none are avaliable.
 If the buffer if empty it blocks.

 Consumer waits until buffer is not empty.
 Consumer locks the buffer.
 Consumer retrieves item from the buffer
 Consumer unlocks the buffer.
 Consumer notifies that the buffer is not full,by calling
 sem_post(&buff_size)

 */

void* consumer(void* consumer_params) {

	struct thread_params *params = (struct thread_params*) consumer_params;
	int c_id = params->id;

	while (1) {

		//If no more items are to be consumed
		//exit the thread function
		if (sem_trywait(&con_num)) {
			break;
		}

		//wait until the buffer is not empty.
		sem_wait(&count);

		//lock the buffer
		sem_wait(&buff_lock);

		//consume an item from the buffer
		consume_from_buffer(&c_id);

		//unlock the buffer
		sem_post(&buff_lock);

		//notify that the buffer is not full.
		sem_post(&buff_size);
	}
	return NULL;
}

double get_time_in_seconds() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec + tv.tv_usec / 1000000.0);

}

