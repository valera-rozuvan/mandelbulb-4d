#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "threads_test.hpp"

#define NUM_THREADS 4

void* perform_work(void* argument)
{
  int passed_in_value;

  passed_in_value = *((int*)argument);
  printf( "Hello from thread with argument %d!\n", passed_in_value );

  return NULL;
}

void test_threads(void)
{
  pthread_t threads[NUM_THREADS];

  int thread_args[NUM_THREADS];
  int result_code;

  unsigned index;

  printf("Testing thread creation.\n");

  // create all threads one by one
  for (index = 0; index < NUM_THREADS; index += 1) {
    thread_args[index] = index;
    printf("Creating thread %d.\n", index);

    result_code = pthread_create(&threads[index], NULL, perform_work, &thread_args[index]);
    assert(!result_code);
  }

  // wait for each thread to complete
  for (index = 0; index < NUM_THREADS; index += 1) {
    // Block until thread 'index' completes.
    result_code = pthread_join(threads[index], NULL);
    assert(!result_code);

    printf("Thread %d has completed.\n", index);
  }

  printf("All threads completed successfully.\n");
}
