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
  
  
  	int pid = atoi(argv[2]);
	int producer_count = atoi(argv[3]);
	int production_count = atoi(argv[1]);
	

	mqd_t queue_descriptor;
	queue_descriptor = mq_open(queue_name, O_RDWR);

	if (queue_descriptor == -1) {
		printf("error opening queue in producer %s\n",strerror(errno));
		return 1;
	}

         int i;
	 for(i = pid ; i<production_count;i +=producer_count)
	 {
	    int message = i;
	    if (mq_send(queue_descriptor, (char*) &message, sizeof(int), 0) == -1) 
	    {
		printf("P: pid %d send failed %s \n",getpid(), strerror(errno));
		return 1;
	    }
	 }

	if (mq_close(queue_descriptor) == -1) {
		perror("mq_close failed");
		exit(2);
	}


	return 0;

}

