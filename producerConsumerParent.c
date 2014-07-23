/*
 * producerConsumerParent.c
 * ECE254 Group 01
 * By : Rushan Yogaratnam and Ameen Patel
 * University of Waterloo Computer Engineering
 * Spring 2014
 *
 * Producer Consumer Process Implementation:
 * This is the parent process which fork's and
 * exec's the producer and consumer processes
 * as child processes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <semaphore.h>
#include <string.h>
#include "producer.h"
#include "common.h"

int main(int argc, char **argv) {

	int queue_size;
	int message_count;
	int num_producers;
	int num_consumers;

	//validate the command line arguments
	if (process_arguments(argc, argv, &queue_size, &message_count,
			&num_producers, &num_consumers)) {
		printf("Invalid arguments\n");
		return 1;
	}

	//set queue attributes
	struct mq_attr queue_attributes;
	queue_attributes.mq_maxmsg = queue_size;
	queue_attributes.mq_msgsize = sizeof(int);
	queue_attributes.mq_flags = 0;

	mqd_t queue_descriptor;
	mode_t permissions = S_IRUSR | S_IWUSR;

	//attempt to open queue and perform error checking
	queue_descriptor = mq_open(queue_name, O_RDWR | O_CREAT, permissions,
			&queue_attributes);

	if (queue_descriptor == -1) {
		printf("error creating queue %s\n", strerror(errno));
		return 1;
	}

	//this semaphore is used as a counting semaphore
	//it is used by the consumers to determine wether or not
	//they should continue consumption
	sem_t *consumer_sem = sem_open("consumer_sem2", O_RDWR | O_CREAT,
			permissions, message_count);

	if (consumer_sem == SEM_FAILED) {
		printf("failed to create consumer semaphore\n");
	}

	//get time before first fork
	double time_before_first_fork = get_time_in_seconds();

	//spawn producer processes
	int i;
	for (i = 0; i < num_producers; ++i) {

		spawn_child("./producer", argv, i, num_producers);
	}

	//spawn consumer processes
	int j;
	for (j = 0; j < num_consumers; ++j) {

		spawn_child("./consumer", argv, j, num_consumers);
	}

	int status, pid;

	//busy loop and wait for all of the child
	//processes to complete execution.

	while ((pid = wait(&status)) != -1) {
	}

	double time_after_last_consumed = get_time_in_seconds();

	double execution_time = time_after_last_consumed - time_before_first_fork;
	printf("System execution time: %f seconds\n", execution_time);

	//Tidy up queues and semaphores

	//close queue.
	if (mq_close(queue_descriptor) == -1) {
		perror("mq_close failed");
		exit(2);
	}

	//mark queue for deletion.
	if (mq_unlink(queue_name) != 0) {
		perror("mq_unlink failed");
		exit(3);
	}

	if (sem_close(consumer_sem) == -1) {
		perror("consumption semaphore failed to close");
		exit(2);
	}

	if (sem_unlink("consumer_sem2") == -1) {
		perror("failed to unlink consumer semaphore");
		exit(3);
	}

	return 0;

}

int spawn_child(char* program, char **arg_list, int p_id, int childCount) {

	arg_list[0] = program;

	char pid[15];
	sprintf(pid, "%d", p_id);
	arg_list[2] = &pid;

	pid_t child_pid;
	child_pid = fork();

	if (child_pid > 0) {
		return child_pid;
	} else if (child_pid < 0) {
		printf("error creating child process %s\n", strerror(errno));
		return -1;
	} else {
		execvp(program, arg_list);
		printf("error occurred in execvp %s\n", strerror(errno));
		abort();
	}

}

double get_time_in_seconds() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec + tv.tv_usec / 1000000.0);

}
