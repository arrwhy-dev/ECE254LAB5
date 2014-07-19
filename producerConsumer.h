/*
 * producerConsumer.h
 *
 *  Created on: Jul 19, 2014
 *      Author: rushan
 */

#ifndef PRODUCERCONSUMER_H_
#define PRODUCERCONSUMER_H_

int process_arguments(int argc, char* argv[], int * queue_size,
		int * production_count, int * producer_count, int * consumer_count);

void create_buffer(int bufferSize);

pthread_t spawn_producer();

void* producer(void* unused);

void add_to_buffer();
pthread_t spawn_consumer();

void* consumer(void* unused);

#endif /* PRODUCERCONSUMER_H_ */
