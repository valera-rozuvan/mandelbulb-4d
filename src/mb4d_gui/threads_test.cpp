#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "threads_test.hpp"

#define NUM_THREADS 4

void* performWork(void* argument)
{
  int passedInValue;

  passedInValue = *((int*)argument);
  printf("Hello from thread with argument %d!\n", passedInValue);

  return NULL;
}

void testThreads(void)
{
  pthread_t threads[NUM_THREADS];

  int threadArgs[NUM_THREADS];
  int resultCode;

  unsigned index;

  printf("Testing thread creation.\n");

  // create all threads one by one
  for (index = 0; index < NUM_THREADS; index += 1) {
    threadArgs[index] = index;
    printf("Creating thread %d.\n", index);

    resultCode = pthread_create(&threads[index], NULL, performWork, &threadArgs[index]);
    assert(!resultCode);
  }

  // wait for each thread to complete
  for (index = 0; index < NUM_THREADS; index += 1) {
    // Block until thread 'index' completes.
    resultCode = pthread_join(threads[index], NULL);
    assert(!resultCode);

    printf("Thread %d has completed.\n", index);
  }

  printf("All threads completed successfully.\n");
}
