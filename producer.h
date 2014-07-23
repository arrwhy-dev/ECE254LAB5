/*
 * Producer.h
 * ECE254 Group 01
 * By : Rushan Yogaratnam and Ameen Patel
 * University of Waterloo Computer Engineering
 * Spring 2014
 *
 *
 * These are a bunch of helper functions.
 * The are just functions which
 * make the code cleaner by providing abstraction.
 *
 */

#ifndef PRODUCER_H_
#define PRODUCER_H_

//This method provides a convenient way to get the current time in seconds
double get_time_in_seconds();

//spawns the child process and sets the time before forking,
//which is the last parameter.
int spawn_child(char*, char **, int, int);

#endif /* PRODUCER_H_ */
