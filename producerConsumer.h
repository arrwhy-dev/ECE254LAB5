/*
 * producerConsumer.h
 *
 *  Created on: Jul 19, 2014
 *      Author: rushan
 */

#ifndef PRODUCERCONSUMER_H_
#define PRODUCERCONSUMER_H_

void* producer(void* unused);
void* consumer(void* unused);
void add_to_buffer();
void consume_from_buffer(int * c_id);
void add_to_buffer(int value);
pthread_t spawn_child();

#endif /* PRODUCERCONSUMER_H_ */
