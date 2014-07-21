/*
 * Producer.c
 * ECE254 Group 01
 * By : Rushan Yogaratnam and Ameen Patel
 * University of Waterloo Computer Engineering
 * Spring 2014
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
	int production_count;
	int num_producers;
	int num_consumers;

	if (process_arguments(argc, argv, &queue_size, &production_count,
			&num_producers, &num_consumers)) {
		printf("Invalid arguments\n");
		return 1;
	}

	struct mq_attr queue_attributes;
	queue_attributes.mq_maxmsg = queue_size;
	queue_attributes.mq_msgsize = sizeof(int);
	queue_attributes.mq_flags = 0;

	mqd_t queue_descriptor;
	mode_t permissions = S_IRUSR | S_IWUSR;

	queue_descriptor = mq_open(queue_name, O_RDWR | O_CREAT, permissions,
			&queue_attributes);

	if (queue_descriptor == -1) {
		printf("error creating the queue %s\n", strerror(errno));
		return 1;
	}

	//create semaphore for the producers and consumers
	//these are used to determine if they need to consumer or pdocuer.

	sem_t *producer_sem = sem_open("producer_sem", O_RDWR | O_CREAT,
			permissions, production_count);
	
	sem_t *consumer_sem = sem_open("consumer_sem", O_RDWR | O_CREAT,
			permissions, production_count);
	

	if (producer_sem == SEM_FAILED) {
		printf("failed to create the production sem\n");
	}
	if (consumer_sem == SEM_FAILED) {
		printf("failed to created consumption sem\n");
	}

	
	int semval1;
	int semval2;
	
	sem_getvalue(producer_sem,&semval1);
	sem_getvalue(consumer_sem,&semval2);
	printf("the producer sem val is %i\n",semval1);
	printf(" the consumer sem val is %i\n",semval2);
	

       double time_before_first_fork = get_time_in_seconds();
	int i;
	for (i = 0; i < num_producers; ++i) {

		 spawn_child("./producer", argv,i,num_producers);
	}

	int h;
	for (h = 0; h < num_consumers; ++h) {

	    spawn_child("./consumer", argv,h,num_consumers);
	}


	//wait on all child processes to finish
	int status, pid;

	while ((pid = wait(&status)) != -1)	
	{
		fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
	}
	
	double time_after_last_consumed = get_time_in_seconds();
	
	double execution_time = time_after_last_consumed - time_before_first_fork;
	printf("System execution time: %f seconds\n",execution_time);

	//Tidy up queues and semaphores

	//close the queue.
	if (mq_close(queue_descriptor) == -1) {
		perror("mq_close failed");
		exit(2);
	}

	//mark the queue for deletion.
	if (mq_unlink(queue_name) != 0) {
		perror("mq_unlink failed");
		exit(3);
	}

	printf("the parent killed the queue!\n");
	if (sem_close(producer_sem) == -1) {
		perror("producer semaphore failed to close");
		exit(2);
	}

	if (sem_close(consumer_sem) == -1) {
		perror("consumption semaphore failed to close");
		exit(2);
	}

	if (sem_unlink("producer_sem") == -1) {
		perror("failed to unlink producer semaphore");
		exit(3);
	}
	if (sem_unlink("consumer_sem") == -1) {
		perror("failed to unlink consumer semaphore");
		exit(3);
	}
	
	printf("Parent exited\n");

	return 0;

}

int process_arguments(int argc, char* argv[], int * queue_size,
		int * production_count, int * producer_count, int * consumer_count) {

	*production_count = atoi(argv[1]);
	*queue_size = atoi(argv[2]);
	*producer_count = atoi(argv[3]);
	*consumer_count = atoi(argv[4]);
	return 0;
}

int spawn_child(char* program, char **arg_list,int p_id,int childCount) {


	arg_list[0] = program;
        char pid [15];
	sprintf(pid,"%d",p_id);
	char consumerCount [15];
	sprintf(consumerCount,"%d",childCount);
	
	arg_list[1] = &pid;
	arg_list[2] = &consumerCount;
	
	pid_t child_pid;
	child_pid = fork();

	if (child_pid > 0) {
		return child_pid;
	} else if (child_pid < 0) {
		printf("error creating the child process %s\n", strerror(errno));
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
