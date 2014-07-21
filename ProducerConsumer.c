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
#include <math.h>
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
int buffer_size;

int main(int argc, char **argv) {

	int producer_count;
	int consumer_count;
	int production_count;

	if (process_arguments(argc, argv, &buffer_size, &production_count,
			&producer_count, &consumer_count)) {
		printf("Invalid arguments\n");
		return 1;
	}

	buffer = NULL;

	sem_init(&buff_lock, 0, 1);
	sem_init(&count, 0, 0);
	sem_init(&buff_size, 0, buffer_size);
	sem_init(&prod_num, 0, production_count);
	sem_init(&con_num, 0, production_count);

	pthread_t producer_ids[producer_count];
	pthread_t consumer_ids[consumer_count];

	int producer_id;

	double time_before_first_thread_created = get_time_in_seconds();
	for (producer_id = 0; producer_id < producer_count; ++producer_id) {
		producer_ids[producer_id] = spawn_producer(producer_id);
	}

	int consumer_id;

	for (consumer_id = 0; consumer_id < consumer_count; ++consumer_id) {
		consumer_ids[consumer_id] = spawn_child(consumer_id);
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
	sem_destroy(&buff_lock);
	sem_destroy(&count);
	sem_destroy(&buff_size);
	sem_destroy(&prod_num);
	sem_destroy(&con_num);

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

pthread_t spawn_producer(int producer_id) {

	pthread_t p_id;
	pthread_create(&p_id, NULL, &producer, &producer_id);
	return p_id;
}

pthread_t spawn_child(int consumer_id) {
	pthread_t p_id;
	pthread_create(&p_id, NULL, &consumer, &consumer_id);
	return p_id;
}

void add_to_buffer(int * p_id) {

	int i = (rand() % 80) + 1;
	int value = (buffer_size * i) + *p_id;

	printf("Producer %i produced %i\n", *p_id, value);

	if (buffer == NULL) {
		buffer = malloc(sizeof(struct queue_element));
		buffer->value = value;
		buffer->next = NULL;
		printf("producer %i added %i as list head\n", *p_id, value);
	} else {
		struct queue_element* new_head = malloc(sizeof(struct queue_element));
		new_head->next = buffer;
		new_head->value = value;
		buffer = new_head;
		printf("producer %i added %i \n", *p_id, value);

	}
}

//This is called from the consumer.

void consume_from_buffer(int * c_id) {

	if (buffer == NULL) {
		printf("failed to read from queue, queue is empty\n");
	} else {

		//get the current head of the buffer
		struct queue_element* current_element;
		current_element = buffer;

		int val = current_element->value;

		//reassign the buffer head
		buffer = buffer->next;
		//delete the previous buffer node.
		free(current_element);
		printf("consumer %i consumed %i\n", *c_id, val);

		double root = sqrt(val);
		double temp = root * root;
		if(temp == val)
		{
			printf("%i %f %i\n",*c_id,root,val);
		}

	}

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
		int * p_id = (int*) unused;
		add_to_buffer(p_id);
		//trigger unlock
		sem_post(&buff_lock);
		//let them know we put some stuff
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

		//is more stuff avaliable?
		sem_wait(&count);
		//trigger the lock
		sem_wait(&buff_lock);
		//take some stuff
		int* c_id = (int*) unused;
		consume_from_buffer(c_id);
		//trigger the unlock
		sem_post(&buff_lock);
		//let producers know theres space
		//for more stuff
		sem_post(&buff_size);
	}
	return NULL;
}

double get_time_in_seconds() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec + tv.tv_usec / 1000000.0);

}

