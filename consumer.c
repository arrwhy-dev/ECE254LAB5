/*
 * consumer.c
 * ECE254 Group 01
 * By : Rushan Yogaratnam and Ameen Patel
 * University of Waterloo Computer Engineering
 * Spring 2014
 *
 *Producer Consumer process implemenation:
 * This is the consumer process that the 
 * parent process creates.
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
  
  
    //the assigned consumer id, between 0 and C-1.
  	int c_id = atoi(argv[2]);
	
	//open the queue and perform error handlind
	mqd_t queue_descriptor;
	queue_descriptor = mq_open(queue_name, O_RDONLY);

	if (queue_descriptor == -1) {
		printf("error opening queue in consumer %s\n",strerror(errno));
		return 1;
	}

	//open a descriptor the the consumer semeaphore.
	sem_t *consumer_sem;
	consumer_sem = sem_open("consumer_sem", 0);

	if (consumer_sem == SEM_FAILED) {
		printf("error opening semaphore in consumer %s\n",strerror(errno));
		return 1;
	}
		

	//infinite loop and keep consuming elements.
	//the consumer exits this loop when the consumer_sem
	//indicated that there are no more items to be expected.
	while(1)
	{
	  //decrement the consumer semaphore.
	  //when this reaches 0 all callers will
	  //stop consuming.	
	  if(sem_trywait(consumer_sem) == -1)
	  {
	    break; 
	  }
	  //recieve a message, this will block if the queue is empty.
	     int message;
	     if (mq_receive(queue_descriptor, (char*) &message, sizeof(int), 0)== -1) 
	      {
			  printf("failed to receive message in consumer %s \n", strerror(errno));
			  return 1;
	       } else 
		{
			//print out if the message is a square number.
			int root = sqrt(message);	
			if((root *root ) == message)
			{
			  printf("%i %i %i\n",c_id,message,root);
			}
		}

	}

	//close the descriptor to queue.
	if (mq_close(queue_descriptor) == -1) {
		perror("mq_close failed in consumer");
		exit(2);
	}

	//close the descriptor to the semaphore
	if (sem_close(consumer_sem) == -1) {
		perror("sem_close failed in consumer");
		exit(2);
	}
	
	return 0;

}
