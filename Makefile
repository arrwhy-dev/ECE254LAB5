
cc==gcc
CFLAGS=-Wall -o
LD=gcc
LDFLAGS=-g


thread:
	gcc -o produce ProducerConsumer.c common.c -lrt -lpthread -lm
process:
	gcc -o producer producer.c common.c -lrt -lpthread
	gcc -o consumer consumer.c common.c -lrt -lpthread -lm
	gcc -o produce producerConsumerParent.c common.c -lrt -lpthread
