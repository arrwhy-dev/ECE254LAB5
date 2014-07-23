/*
 * producer.c
 * ECE254 Group 01
 * By : Rushan Yogaratnam and Ameen Patel
 * University of Waterloo Computer Engineering
 * Spring 2014
 *
 * Producer Consumer Process Implementation:
 * This is the producer child process that 
 * the parent process forks.
 *
 */

#include <mqueue.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <string.h>
#include "producer.h"
#include "common.h"

int main(int argc, char **argv) {

	//this is the assigned producer id, between 0 and P-1.
	int pid = atoi(argv[2]);

	//the number of producer, P
	int num_producers = atoi(argv[3]);
	//number of messages to produce
	int messages_to_produce = atoi(argv[1]);

	//attempt to open the queue and perform error handling
	mqd_t queue_descriptor;
	queue_descriptor = mq_open(queue_name, O_RDWR);

	if (queue_descriptor == -1) {
		printf("error opening queue in producer %s\n", strerror(errno));
		return 1;
	}

	//produce only the set of elements that satisfy i%num_producers = pid.
	int i;
	for (i = pid; i < messages_to_produce; i += num_producers) {
		//send a message to the queue, blocks if queue is full.
		int message = i;
		if (mq_send(queue_descriptor, (char*) &message, sizeof(int), 0) == -1) {
			printf("P: pid %d send failed %s \n", getpid(), strerror(errno));
			return 1;
		}
	}

	//close the queue.
	if (mq_close(queue_descriptor) == -1) {
		perror("mq_close failed in producer");
		exit(2);
	}

	return 0;

}

