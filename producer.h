/*
* Producer.h
* ECE254 Group 01
* By : Rushan Yogaratnam and Ameen Patel
* University of Waterloo Computer Engineering
* Spring 2014
*
*
* These are a bunch of helper functions for the
* producer.c file. The are just functions which
* make the code cleaner by providing abstraction.
*
*/

#ifndef PRODUCER_H_
#define PRODUCER_H_


//This method provides a convenient way to get the current time in seconds
double get_time_in_seconds();

//this method causes the consumer to wait on the child and
//once the child process is down waiting prints the initialization and
//data transmission times.
int wait_on_child(double time_before_fork, double time_after_fork);


//Provider error checking on the command line arguments,
// if they are invalid -1 is return. The last two parameters
//are pointers to queue size and process count, these are set
//if valid
int process_arguments(int argc, char* argv[], int * queue_size,
		int * production_count, int * producer_count, int * consumer_count);

//spawns the child process and sets the time before forking,
//which is the last parameter.
int spawn_child(char*, char **,int,int);

//The main part of the producer code
//this creates elements and sends them to the msg queue.
void produce_and_send_elements(int, mqd_t);

int produce_message(int pid,int numProducers);

#endif /* PRODUCER_H_ */
