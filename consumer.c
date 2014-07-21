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
#include <math.h>
#include "common.h"

int main(int argc, char **argv) {
  
  
  
  	int c_id = atoi(argv[2]);
	
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
		

	while(1)
	{
	  
	  if(sem_trywait(consumer_sem) == -1)
	  {
	    break; 
	  }
	     int message;
	     if (mq_receive(queue_descriptor, (char*) &message, sizeof(int), 0)== -1) 
	      {
			  printf("failed to receive message %s \n", strerror(errno));
			  return 1;
	       } else 
		{
			int temp = sqrt(message);	
		  
			if((temp *temp ) == message)
			{
			  printf("%i %i %i\n",c_id,message,temp);
			}
		}

	}
	if (mq_close(queue_descriptor) == -1) {
		perror("mq_close failed");
		exit(2);
	}
	
	return 0;

}
