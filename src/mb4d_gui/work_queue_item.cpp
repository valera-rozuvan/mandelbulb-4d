#include <stdio.h>
#include "work_queue_item.hpp"

WorkQueueItem::WorkQueueItem(
  unsigned int startX_, unsigned int endX_, unsigned int startY_, unsigned int endY_
)
{
  this->isFirstItem = false;

  this->prevItem = NULL;
  this->nextItem = NULL;

  this->startX = startX_;
  this->endX = endX_;

  this->startY = startY_;
  this->endY = endY_;
}

WorkQueueItem::~WorkQueueItem(void)
{
  fprintf(stdout, "WorkQueueItem::~WorkQueueItem destructor.\n");
}
