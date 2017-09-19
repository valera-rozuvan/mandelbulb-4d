#include <stdio.h>
#include "utils.hpp"
#include "work_queue.hpp"

void WorkQueue::initNewQueueItem(
  unsigned int Ax, unsigned int Bx, unsigned int Ay, unsigned int By
)
{
  WorkQueueItem* newQueueItem = new WorkQueueItem(Ax, Bx, Ay, By);

  this->push_item(newQueueItem);
}

void WorkQueue::push_item(WorkQueueItem* newQueueItem)
{
  WorkQueueItem* lastItem;
  WorkQueueItem* newItem;

  if (newQueueItem == NULL) {
    return;
  }

  if (this->num_of_queue_items == 0) {
    this->workQueueItems = newQueueItem;
    this->workQueueItems->isFirstItem = true;
    this->workQueueItems->nextItem = this->workQueueItems;
    this->workQueueItems->prevItem = this->workQueueItems;
  } else {
    lastItem = this->workQueueItems->prevItem;

    lastItem->nextItem = newQueueItem;

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
  }

  this->num_of_queue_items = 0;
  this->workQueueItems = NULL;
}

WorkQueueItem* WorkQueue::pop_nth_item(const unsigned int n)
{
  unsigned int i;

  WorkQueueItem* item_to_pop;
  WorkQueueItem* old_next_item;
  WorkQueueItem* old_prev_item;


  // If we don't have any items.
  if (this->num_of_queue_items == 0) {
    return NULL;
  }

  // If a request comes for a non-existent item.
  if (n >= this->num_of_queue_items) {
    return NULL;
  }


  // Find the item to pop.
  item_to_pop = this->workQueueItems;

  i = 0;
  while (i < n) {
    i += 1;

    item_to_pop = item_to_pop->nextItem;
  }


  // Take it out of the queue.
  if (this->num_of_queue_items == 1) {
    item_to_pop->isFirstItem = false;

    this->num_of_queue_items = 0;
    this->workQueueItems = NULL;
  } else {
    old_next_item = item_to_pop->nextItem;
    old_prev_item = item_to_pop->prevItem;

    old_next_item->prevItem = old_prev_item;
    old_prev_item->nextItem = old_next_item;

    if (n == 0) {
      item_to_pop->isFirstItem = false;

      old_next_item->isFirstItem = true;
      this->workQueueItems = old_next_item;
    }

    this->num_of_queue_items -= 1;
  }

  return item_to_pop;
}

WorkQueueItem* WorkQueue::pop_rnd_item(void)
{
  unsigned int item_index;

  if (this->num_of_queue_items == 0) {
    return NULL;
  }

  if (this->num_of_queue_items == 1) {
    return this->pop_nth_item(0);
  }

  item_index = rnd_from_range(0, this->num_of_queue_items - 1);

  return this->pop_nth_item(item_index);
}

WorkQueue::WorkQueue(void)
{
  this->num_of_queue_items = 0;
  this->workQueueItems = NULL;
}

WorkQueue::~WorkQueue(void)
{
  fprintf(stdout, "WorkQueue::~WorkQueue destructor.\n");

  this->clearQueue();
}
