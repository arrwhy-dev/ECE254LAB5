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

	mqd_t queue_descriptor;
	queue_descriptor = mq_open(queue_name, O_RDWR);

	if (queue_descriptor == -1) {
		printf("there was an error opening the queue in the consumer");
		printf("the error is %s \n", strerror(errno));
		return 1;
	}

	sem_t *producer_sem;
	producer_sem = sem_open("producer_sem", 0);

	if (producer_sem == SEM_FAILED) {
		printf("there was an error opening the semaphore in the producer");
		printf("the error is %s \n", strerror(errno));
		return 1;
	}

	printf("Printing from the producer with pid %d\n", getpid());
	
	int semval;
	sem_getvalue(producer_sem,&semval);
	printf("producer sem val is %i\n",semval);
	
	
	while(1)
	{
	  
	  if(sem_trywait(producer_sem) == -1)
	  {
	    printf("sem toggle in producer exiting while loop\n");
	    break; 
	  }
	  
	 printf("Printing from the producer with pid %d inside while loop \n", getpid());

	  int message = 5;
	  if (mq_send(queue_descriptor, (char*) &message, sizeof(int), 0) == -1) 
	  {
		printf("failed to send message %s \n", strerror(errno));
		return 1;

	  }else
	  {
	      printf("send message %i\n",message);
	  }
	  
	}

	if (mq_close(queue_descriptor) == -1) {
		perror("mq_close failed");
		exit(2);
	}
	
	if (sem_close(producer_sem) == -1) {
		perror("failed to close semaphore in producer");
		exit(2);
	}
	
         printf("exiting the producer with pid %d\n", getpid());


	return 0;

}
