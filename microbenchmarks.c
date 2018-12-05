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
struct timespec after2;

struct timespec beforeFork;
struct timespec afterFork;
struct timespec afterFork2;

void *childfunc(void *offset)
{
  clock_gettime(CLOCK_REALTIME,&after2); // after thread start running
  return NULL;
}

void * malloc_shared(size_t size){
   return mmap(NULL,size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
}

void test_threads(){
  int createNum;
  pthread_t child;
  StatObject ctr = NewStatObject();
  StatObject ctr2 = NewStatObject();
  for (long i = 0; i < N_REPS; i++)
  {
    clock_gettime(CLOCK_REALTIME,&before);
    createNum = pthread_create(&child, NULL, childfunc, (void *)i);
    clock_gettime(CLOCK_REALTIME,&after);  // Discussion question in lecture says A is correct, so I measure it after ASAP
    long difference = (after.tv_sec * NANOSECONDS_PER_SECOND + after.tv_nsec) -     (before.tv_sec * NANOSECONDS_PER_SECOND + before.tv_nsec);
    pthread_join(child, NULL);
    long difference2 = (after2.tv_sec * NANOSECONDS_PER_SECOND + after2.tv_nsec) -     (after.tv_sec * NANOSECONDS_PER_SECOND + after.tv_nsec);
    if (createNum == 0) {
      //add to ctr for stats;
      printf("this thread took %ld nanoseconds to be spawned.\n",difference);
      printf("this thread took %ld nanoseconds to run.\n",difference2);
      stat_object_add(ctr,difference);
      stat_object_add(ctr2,difference2);
    }
  }
}

void test_processes(){
  int wpid, status, retval;
  StatObject ctr3 = NewStatObject();
  StatObject ctr4 = NewStatObject();

  for (int i = 0; i < N_REPS; i++) {
    clock_gettime(CLOCK_REALTIME,&beforeFork);
    retval = fork(); // same logic as thread: must timestamp immediately as opposed
    // to timestamping in an if-statement
    clock_gettime(CLOCK_REALTIME,&afterFork);
    long forkDifference = (afterFork.tv_sec * NANOSECONDS_PER_SECOND + afterFork.tv_nsec) -     (beforeFork.tv_sec * NANOSECONDS_PER_SECOND + beforeFork.tv_nsec);
    if(retval == 0){
      printf("This process took %ld nanoseconds to be spawned!\n",forkDifference);
      stat_object_add(ctr3,forkDifference);

      exit(0);
    }
    else{
      // do nothing
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

