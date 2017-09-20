#ifndef WORK_QUEUE_ITEM_HPP
#define WORK_QUEUE_ITEM_HPP

#include <stdbool.h>

class WorkQueueItem {
  public:
    bool isFirstItem;

    WorkQueueItem* prevItem;
    WorkQueueItem* nextItem;

    unsigned int startX;
    unsigned int endX;

    unsigned int startY;
    unsigned int endY;

    WorkQueueItem(unsigned int, unsigned int, unsigned int, unsigned int);
    ~WorkQueueItem(void);
};

#endif // WORK_QUEUE_ITEM_HPP
