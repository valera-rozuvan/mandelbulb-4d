#ifndef WORK_QUEUE_ITEM_HPP
#define WORK_QUEUE_ITEM_HPP

#include <stdbool.h>

class WorkQueueItem {
  public:
    bool isFirstItem;

    WorkQueueItem* prevItem;
    WorkQueueItem* nextItem;

    unsigned int Ax;
    unsigned int Ay;
    unsigned int Bx;
    unsigned int By;

    // unsigned char* image_piece;

    unsigned int imgWidth;
    unsigned int imgHeight;

    WorkQueueItem(unsigned int, unsigned int, unsigned int, unsigned int);
    ~WorkQueueItem(void);
};

#endif // WORK_QUEUE_ITEM_HPP
