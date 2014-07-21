/*
 * producer.c
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
#include "producer.h"
#include "common.h"

int main(int argc, char **argv) {
  
  
  	int pid = atoi(argv[1]);
	int producer_count = atoi(argv[2]);
	
	printf("P: assigned pid  %d to %i \n",pid,getpid());

	mqd_t queue_descriptor;
	queue_descriptor = mq_open(queue_name, O_RDWR);

	if (queue_descriptor == -1) {
		printf("error opening queue in producer %s\n",strerror(errno));
		return 1;
	}


	
	sem_t *producer_sem;
	producer_sem = sem_open("producer_sem", 0);

	if (producer_sem == SEM_FAILED) {
		printf("error opening semaphore in producer %s\n",strerror(errno));
		return 1;
	}

	printf("P: spawned pid %d\n", getpid());
	
	int semval;
	sem_getvalue(producer_sem,&semval);
	printf("P: sem %i\n",semval);
	
	int counter =0;
	while(1)
	{
	  
	  if(sem_trywait(producer_sem) == -1)
	  {
	    printf("P: sem toggle\n");
	    break; 
	  }
	  
	 printf("P: pid %d to send \n", getpid());

	  int message = produce_message(pid,producer_count,counter);
	  if (mq_send(queue_descriptor, (char*) &message, sizeof(int), 0) == -1) 
	  {
		printf("P: pid %d send failed %s \n",getpid(), strerror(errno));
		return 1;

	  }else
	  {
	      printf("P: pid %d sent %i\n",getpid(),message);
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
	
    printf("P: pid %d exiting\n", getpid());


	return 0;

}

int produce_message(int pid,int numProducers,int value)
{
    int i = (rand() % 80) + 1;
	int value = (numProducers * value) + pid;
	return value;
}
