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
	queue_descriptor = mq_open(queue_name, O_RDONLY);

	if (queue_descriptor == -1) {
		printf("error opening queue in consumer %s\n",strerror(errno));
		return 1;
	}

	sem_t *consumer_sem;
	consumer_sem = sem_open("consumer_sem", 0);

	if (consumer_sem == SEM_FAILED) {
		printf("error opening semaphore in consumer %s\n",strerror(errno));
		return 1;
	}

	printf("Printing from the consumer with pid %d\n", getpid());
	
	int semval;
	sem_getvalue(consumer_sem,&semval);
	
	printf("consumer sem value is %i\n",semval);

	while(1)
	{
	  
	  if(sem_trywait(consumer_sem) == -1)
	  {
	    printf("sem toggled in consumer exiting while loop\n");
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
		        fflush(stdout);
		}

	}
	if (mq_close(queue_descriptor) == -1) {
		perror("mq_close failed");
		exit(2);
	}
	
	printf("exiting consumer with pid %d\n", getpid());

	return 0;

}
