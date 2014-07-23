/*
 * common.c
 *
 * ECE254 Group 01
 * By : Rushan Yogaratnam and Ameen Patel
 * University of Waterloo Computer Engineering
 * Spring 2014
 *
 * Implementation of common.h
 *
 */

#include <mqueue.h>
#include <stdlib.h>

//constant queue_name for both producer and consumer.
const char* queue_name = "/mailbox_ece254_ryogarat";

int process_arguments(int argc, char* argv[], int * queue_size,
		int * message_count, int * producer_count, int * consumer_count) {

	if (argc < 5) {
		return 1;
	} else {
		*message_count = atoi(argv[1]);
		*queue_size = atoi(argv[2]);
		*producer_count = atoi(argv[3]);
		*consumer_count = atoi(argv[4]);

		return ((*message_count <= 0 || *queue_size <= 0 || *producer_count <= 0
				|| *consumer_count <= 0) ? 1 : 0);
	}

}

