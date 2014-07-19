/*
 * producerConsumer.c
 * ECE254 Group 01
 * By :  Rushan Yogaratnam and Ameen Patel
 * University of Waterloo Computer Engineering
 * Spring 2014
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>
#include "producerConsumer.h"

struct queue_element {
	int value;
	struct queue_element* next;
};

struct queue_element* buffer;

sem_t buff_lock;
sem_t count;
sem_t buff_size;
sem_t prod_num;
sem_t con_num;

int main(int argc, char **argv) {

	int buffer_size;
	int producer_count;
	int consumer_count;
	int production_count;

	if (process_arguments(argc, argv, &buffer_size, &production_count,
			&producer_count, &consumer_count)) {
		printf("Invalid arguments\n");
		return 1;
	}

	//threading code.

	buffer = NULL;

	//init yo sems.
	sem_init(&buff_lock, 0, 1);
	sem_init(&count, 0, 0);
	sem_init(&buff_size, 0, 3);
	sem_init(&prod_num, 0, production_count);
	sem_init(&con_num, 0, producer_count);

	pthread_t producer_ids[producer_count];
	pthread_t consumer_ids[consumer_count];

	int h;

	for (h = 0; h < producer_count; ++h) {
		producer_ids[h] = spawn_producer();
	}

	int q;

	for (q = 0; q < consumer_count; ++q) {
		consumer_ids[q] = spawn_consumer();
	}

	int i;
	for (i = 0; i < producer_count; ++i) {
		pthread_join(producer_ids[i], NULL);
	}

	int j;
	for (j = 0; j < consumer_count; ++j) {
		pthread_join(consumer_ids[j], NULL);
	}

	//clean up semaphores

	return 0;
}

int process_arguments(int argc, char* argv[], int * queue_size,
		int * production_count, int * producer_count, int * consumer_count) {

	*production_count = atoi(argv[1]);
	*queue_size = atoi(argv[2]);
	*producer_count = atoi(argv[3]);
	*consumer_count = atoi(argv[4]);
	return 0;
}
/*
 void add_to_buffer(int x) {

 pthread_t thread_id = pthread_self();
 printf("adding from thread %i\n", thread_id);
 if (buffer == NULL) {
 buffer = malloc(sizeof(struct queue_element));
 buffer->value = x;
 buffer->next = NULL;
 printf("added the value %i as the list head\n", x);
 } else {
 struct queue_element* next_job = malloc(sizeof(struct queue_element));
 next_job->next = buffer;
 next_job->value = x;
 buffer = next_job;
 printf("appended the value %i as the list head\n", x);
 }

 }

 */

void consume_from_queue() {

	pthread_t thread_id = pthread_self();
	printf("consume from thread %i\n", (int) thread_id);

	if (buffer == NULL) {
		printf("failed to read from queue, queue is empty\n");
	} else {
		struct queue_element* current_element;
		current_element = buffer;
		int val = current_element->value;
		buffer = buffer->next;
		free(current_element);
		printf("obtained %i from the queue\n", val);
	}

}

pthread_t spawn_producer() {

	pthread_t p_id;
	pthread_create(&p_id, NULL, &producer, NULL);
	return p_id;
}

pthread_t spawn_consumer() {
	pthread_t p_id;
	pthread_create(&p_id, NULL, &consumer, NULL);
	return p_id;
}

void add_to_buffer() {

	int i = rand();
	pthread_t thread_id = pthread_self();
	printf("I am producer %i and I produced %i\n", (int) thread_id, i);

}

void consume_from_buffer() {
	pthread_t thread_id = pthread_self();
	printf("I am consumer %i and I consumed some garbage\n", (int) thread_id);

}

void* producer(void* unused) {
	srand(time(NULL));
	while (1) {

		//do we need to make more stuff?
		if (sem_trywait(&prod_num)) {
			break;
		}
		//do we have room for stuff?
		sem_wait(&buff_size);
		// trigger the lock
		sem_wait(&buff_lock);
		//put some stuff in the buffer
		add_to_buffer();
		//trigger unlock
		sem_post(&buff_lock);
		//let them now we put some stuff
		//in the buffer.
		sem_post(&count);
	}
	return NULL;
}

void* consumer(void* unused) {

	while (1) {
		//are we expecting more stuff?
		if (sem_trywait(&con_num)) {
			break;
		}
		//if more stuff avaliable?
		sem_wait(&count);
		//trigger the lock
		sem_wait(&buff_lock);
		//take some stuff
		consume_from_buffer();
		//trigger the unlock
		sem_post(&buff_lock);
		//let them know theres space
		//for more stuff
		sem_post(&buff_size);
	}
	return NULL;
}

