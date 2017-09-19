#include <stdlib.h>
#include <stdio.h>
#include "work_queue_item.hpp"

WorkQueueItem::WorkQueueItem(
  unsigned int Ax_, unsigned int Bx_, unsigned int Ay_, unsigned int By_
)
{
  this->isFirstItem = false;

  this->prevItem = NULL;
  this->nextItem = NULL;

  this->Ax = Ax_;
  this->Bx = Bx_;
  this->Ay = Ay_;
  this->By = By_;

  this->imgWidth = Bx - Ax;
  this->imgHeight = By - Ay;

  // this->image_piece = (unsigned char *)calloc(sizeof(unsigned char) * this->imgWidth * this->imgHeight * 4, sizeof(unsigned char));
}

WorkQueueItem::~WorkQueueItem(void)
{
  fprintf(stdout, "WorkQueueItem::~WorkQueueItem destructor.\n");

  // delete this->image_piece;
}
