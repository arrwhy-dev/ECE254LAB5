/*
 * consumer.c
 * ECE254 Group 01
 * By : Rushan Yogaratnam and Ameen Patel
 * University of Waterloo Computer Engineering
 * Spring 2014
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
#include "common.h"

int main(int argc, char **argv) {

	int queue_size = atoi(argv[2]);
	int messages_to_consume = atoi(argv[1]);
	
	printf("in the consumer: queue size is %i\n",queue_size);
	printf("in the consumer: messages to consume is %i\n",messages_to_consume);

	struct mq_attr queue_attributes;
	queue_attributes.mq_maxmsg = queue_size;
	queue_attributes.mq_msgsize = sizeof(int);
	queue_attributes.mq_flags = 0;

	mqd_t queue_descriptor;
	queue_descriptor = mq_open(queue_name, O_RDONLY);

	if (queue_descriptor == -1) {
		printf("there was an error opening the queue in the consumer");
		printf("the error is %s \n", strerror(errno));
		return 1;
	}

	sem_t *consumer_sem;
	consumer_sem = sem_open("consumer_sem", 0);

	if (consumer_sem == SEM_FAILED) {
		printf("there was an error opening the semaphore in the consumer");
		printf("the error is %s \n", strerror(errno));
		return 1;
	}

	printf("Printing from the consumer with pid %d\n", getpid());
	

	while(1)
	{
	  
	  if(sem_trywait(consumer_sem))
	  {
	    break; 
	  }
	
		 printf("Printing from the consumer with pid %d inside while loop \n", getpid());

		  int message;
		  if (mq_receive(queue_descriptor, (char*) &message, sizeof(int), 0)== -1) 
		  {
			  printf("failed to receive message %s \n", strerror(errno));
			  return 1;
		} else 
		{
			printf("%i is consumed\n", message);
		}

	}
	if (mq_close(queue_descriptor) == -1) {
		perror("mq_close failed");
		exit(2);
	}

	return 0;

}
