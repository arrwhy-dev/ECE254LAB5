/*
* common.h
*
* ECE254 Group 01
* By : Rushan Yogaratnam and Ameen Patel
* University of Waterloo Computer Engineering
* Spring 2014
*
* The purpose of this header file is to
* declare common constants and functions
* that are used between both the producer and the consumer.
*/

#ifndef COMMON_H_
#define COMMON_H_

//global queue_name
extern const char* queue_name;

//global semaphore queue_name
extern const char* consumer_sem_name;

int process_arguments(int argc, char* argv[], int * queue_size,
		int * message_count, int * producer_count, int * consumer_count);

#endif
