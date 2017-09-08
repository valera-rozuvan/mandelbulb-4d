#include <stdio.h>
#include "parallel.hpp"

Parallel::Parallel(void)
{
  this->num_threads = 4;

  this->workQueue = new WorkQueue();

  this->workQueue->addNewQueueItem(1, 2, 3, 4);
  this->workQueue->addNewQueueItem(2, 3, 4, 5);
  this->workQueue->addNewQueueItem(3, 4, 5, 6);

  this->workQueue->test_func();
}

Parallel::~Parallel(void)
{
  fprintf(stdout, "Parallel::~Parallel destructor.\n");

  delete this->workQueue;
}
