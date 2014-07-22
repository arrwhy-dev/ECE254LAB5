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

void* testone(void* unused)
{

	int q;
	for(q=0;q<10;++q)
	{
		printf("producer with %i\n",pthread_self());

	}

}


void* testtwo(void* unused)
{
  
  int l;
 for(l=0;l<10;++l)
 {
      printf("consumer with %i\n",pthread_self());
    
 }
  
}

struct queue_element* buffer;

sem_t buff_lock;
sem_t count;
sem_t buff_size;
sem_t prod_num;
sem_t con_num;
int buffer_size;
int num_to_produce;
int num_producers;

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
	num_to_produce = producer_count;
        num_producers=num_producers;
	sem_init(&buff_lock, 0, 1);
	sem_init(&count, 0, 0);
	sem_init(&buff_size, 0, buffer_size);
	sem_init(&prod_num, 0, production_count);
	sem_init(&con_num, 0, production_count);

	pthread_t producer_ids[producer_count];
	pthread_t consumer_ids[consumer_count];

	int producer_id;

	double time_before_first_thread_created = get_time_in_seconds();
	
	pthread_t p_id [producer_count];
	pthread_t c_id [consumer_count]; 
	
	int i ;
	for(i=0; i<producer_count;++i)
	{
	  pthread_create(&(p_id[i]), NULL, &producer, &i);
	}
	
	int c;
	for(c=0;c<consumer_count;++c)
	{
	    pthread_create(&(c_id[c]), NULL, &consumer, &c);
	}
	
	int j;
	for(j=0;j<producer_count;++j)
	{
	    pthread_join(p_id[j],NULL);
	}
	
	int k;
	for(k=0;k<consumer_count;++k)
	{
	    pthread_join(c_id[k],NULL);
	}



	//clean up semaphores
	sem_destroy(&buff_lock);
	sem_destroy(&count);
	sem_destroy(&buff_size);
	sem_destroy(&prod_num);
	sem_destroy(&con_num);
	
	
	printf("main thread exited\n");

	return 0;
}

void add_to_buffer(int pid, int value) {

	

	printf("Producer %i produced %i\n", pid, value);

	if (buffer == NULL) {
		buffer = malloc(sizeof(struct queue_element));
		buffer->value = value;
		buffer->next = NULL;
		printf("producer %i added %i as list head\n", pid, value);
	} else {
		struct queue_element* new_head = malloc(sizeof(struct queue_element));
		new_head->next = buffer;
		new_head->value = value;
		buffer = new_head;
		printf("producer % added %i \n", pid, value);

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

		int root = sqrt(val);
		int temp = root * root;
		if(temp == val)
		{
			printf("%i %i %i\n",*c_id,root,val);
		}

	}

}


void* producer(void* unused) {
	
	int *pid = (int*)unused;
	printf("inside producer %i\n",*pid);
	int i;
	for(i = *pid;i<num_to_produce;i=i+num_producers)
	{
		//do we have room for stuff?
		sem_wait(&buff_size);
		// trigger the lock
		sem_wait(&buff_lock);
		//put some stuff in the buffer
		add_to_buffer(*pid,i);
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

