/*
 * producerConsumer.h
 * ECE254 Group 01
 * By : Rushan Yogaratnam and Ameen Patel
 * University of Waterloo Computer Engineering
 * Spring 2014
 *
 *
 * These are a bunch of helper functions.
 * The are just functions which
 * make the code cleaner by providing abstraction.
 *
 */
#ifndef PRODUCERCONSUMER_H_
#define PRODUCERCONSUMER_H_

//Thread function for the producer threads
void* producer(void* unused);

//Thread functionf for the consumer threads
void* consumer(void* unused);

#endif /* PRODUCERCONSUMER_H_ */
