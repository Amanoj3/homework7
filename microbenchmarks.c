#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include "statistics.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NANOSECONDS_PER_SECOND 1E9
#define N_REPS 50000
#define NUM_SPECS 3

struct timespec before;
struct timespec after;
struct timespec afterThreadRun;

struct timespec beforeFork;
struct timespec afterFork;
struct timespec afterProcessRun;

void *childfunc(void *offset)
{
  // start running thread (after);

  clock_gettime(CLOCK_REALTIME,&afterThreadRun);

  return NULL;
}

void test_threads(){
  pthread_t child;
  int createNum;
  // revisit lecture capture around 2:25 PM
  for (long i = 0; i < N_REPS; i++)
  {
    clock_gettime(CLOCK_REALTIME,&before);
    createNum = pthread_create(&child, NULL, childfunc, (void *)i);
    if (createNum == 0) {
      clock_gettime(CLOCK_REALTIME,&after);
      long difference = (after.tv_sec * NANOSECONDS_PER_SECOND + after.tv_nsec) - (before.tv_sec * NANOSECONDS_PER_SECOND + before.tv_nsec);
      printf("this thread took %ld nanoseconds to be spawned.\n",difference);
      pthread_join(child, NULL);
      long runDiff = (afterThreadRun.tv_sec * NANOSECONDS_PER_SECOND + afterThreadRun.tv_nsec) - (after.tv_sec * NANOSECONDS_PER_SECOND + after.tv_nsec);
      printf("this thread took %ld nanoseconds to start running.\n",runDiff);
    }
    //pthread_join(child, NULL);
  }
}

void test_processes(){
  int wpid, status, retval;

  for (int i = 0; i < N_REPS; i++) {
    clock_gettime(CLOCK_REALTIME,&beforeFork);
    retval = fork();
    if(retval == 0){ // in this if statement, you are in the child
    }

    else{

      clock_gettime(CLOCK_REALTIME,&afterFork);
      long forkDiff = (afterFork.tv_sec * NANOSECONDS_PER_SECOND + afterFork.tv_nsec) - (beforeFork.tv_sec * NANOSECONDS_PER_SECOND + beforeFork.tv_nsec);
      printf("This process took %ld nanoseconds to be spawned. \n",forkDiff);

    }
    wpid = wait(&status);
  }
  // wait for all worker processes to finish
  while ((wpid = wait(&status)) > 0);
}


int main(int argc, char **argv)
{

  printf("spawning a lot of threads.\n");
  test_threads();
  printf("spawning a lot of processes.\n");
  test_processes();
  return EXIT_SUCCESS;
}
