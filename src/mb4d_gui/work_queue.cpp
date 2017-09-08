#include "boost/random.hpp"
#include <iostream>
#include <ctime>

#include <stdio.h>
#include "work_queue.hpp"

void WorkQueue::test_func(void)
{
  const int rangeMin = 1;
  const int rangeMax = 10;

  typedef boost::uniform_int<> NumberDistribution;
  typedef boost::mt19937 RandomNumberGenerator;
  typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;

  NumberDistribution distribution(rangeMin, rangeMax);
  RandomNumberGenerator generator;
  Generator numberGenerator(generator, distribution);
  generator.seed(std::time(0)); // seed with the current time

  std::cout << numberGenerator() << std::endl;
  std::cout << numberGenerator() << std::endl;
  std::cout << numberGenerator() << std::endl;
  std::cout << numberGenerator() << std::endl;
  std::cout << numberGenerator() << std::endl;
  std::cout << numberGenerator() << std::endl;
  std::cout << numberGenerator() << std::endl;
}

WorkQueue::WorkQueue(void)
{
  this->num_of_queue_items = 0;
}

void WorkQueue::addNewQueueItem(double Ax, double Ay, double Bx, double By)
{
  WorkQueueItem* lastItem;
  WorkQueueItem* newItem;

  if (this->num_of_queue_items == 0) {
    this->workQueueItems = new WorkQueueItem(Ax, Ay, Bx, By);
    this->workQueueItems->isFirstItem = true;
    this->workQueueItems->nextItem = this->workQueueItems;
    this->workQueueItems->prevItem = this->workQueueItems;
  } else {
    lastItem = this->workQueueItems->prevItem;

    lastItem->nextItem = new WorkQueueItem(Ax, Ay, Bx, By);

    newItem = lastItem->nextItem;

    this->workQueueItems->prevItem = newItem;

    newItem->prevItem = lastItem;
    newItem->nextItem = this->workQueueItems;
  }

  this->num_of_queue_items += 1;
}

void WorkQueue::clearQueue(void)
{
  WorkQueueItem* lastItem;

  if (this->num_of_queue_items > 1) {
    lastItem = this->workQueueItems->prevItem;

    while (lastItem->isFirstItem != true) {
      lastItem = lastItem->prevItem;

      delete lastItem->nextItem;

      this->num_of_queue_items -= 1;
    }
  }

  if (this->num_of_queue_items == 1) {
    delete this->workQueueItems;

    this->num_of_queue_items = 0;
  }
}

WorkQueue::~WorkQueue(void)
{
  fprintf(stdout, "WorkQueue::~WorkQueue destructor.\n");

  this->clearQueue();
}
