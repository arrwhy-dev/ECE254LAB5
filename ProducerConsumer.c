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
#include "common.h"

struct queue_element {
	int value;
	struct queue_element* next;
};

struct thread_params{
  int id;
};

struct queue_element* buffer;

sem_t buff_lock;
sem_t count;
sem_t buff_size;
sem_t con_num;
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

	
	buffer = NULL;   

	sem_init(&buff_lock, 0, 1);
	sem_init(&count, 0, 0);
	sem_init(&buff_size, 0, buffer_size);
	sem_init(&con_num, 0, production_count);

	pthread_t p_thread_id [producer_count];
	pthread_t c_thread_id [consumer_count]; 
	
	struct thread_params p_id [producer_count];
	struct thread_params c_id [consumer_count];

	double time_before_first_thread_created = get_time_in_seconds();
	
	int i;
	for(i=0; i<producer_count;++i)
	{
	  p_id[i].id = i;
	  pthread_create(&(p_thread_id[i]), NULL, &producer, &(p_id[i]));
	}
	
	
	int c;
	for(c=0;c<consumer_count;++c)
	{
	  c_id[c].id = c;
	  pthread_create(&(c_thread_id[c]), NULL, &consumer, &(c_id[c]));
	}
	
	
	
	int j;
	for(j=0;j<producer_count;++j)
	{
	    pthread_join(p_thread_id[j],NULL);
	}
	
	
	int k;
	for(k=0;k<consumer_count;++k)
	{
	    pthread_join(c_thread_id[k],NULL);
	}

	

	//clean up semaphores
	sem_destroy(&buff_lock);
	sem_destroy(&count);
	sem_destroy(&buff_size);
	sem_destroy(&con_num);
	
	
	printf("main thread exited\n");

	return 0;
}

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
		if(val == (root*root))
		{
			printf("%i %i %i\n",*c_id,root,val);
		}
	}

}


void* producer(void* producer_params) {

	
	struct thread_params *params = (struct thread_params*)producer_params;
	int p_id = params->id;
	
	int i;
	for(i = p_id;i<production_count;i+=producer_count)
	{
		//do we have room for stuff?
		sem_wait(&buff_size);
		// trigger the lock
		sem_wait(&buff_lock);
		//put some stuff in the buffer
		add_to_buffer(i);
		//trigger unlock
		sem_post(&buff_lock);
		//let them know we put some stuff
		//in the buffer.
		sem_post(&count);
		
	}
	return NULL;
}

void* consumer(void* consumer_params) {

	struct thread_params *params = (struct thread_params*) consumer_params;
	int c_id = params->id;

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
		consume_from_buffer(&c_id);
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

