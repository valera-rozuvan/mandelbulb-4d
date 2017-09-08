#ifndef WORK_QUEUE_ITEM_HPP
#define WORK_QUEUE_ITEM_HPP

#include <stdbool.h>

class WorkQueueItem {
  public:
    bool isFirstItem;

    WorkQueueItem* prevItem;
    WorkQueueItem* nextItem;

    double Ax;
    double Ay;
    double Bx;
    double By;

    unsigned char* image_piece;

    unsigned int imgWidth;
    unsigned int imgHeight;

    WorkQueueItem(double Ax_, double Ay_, double Bx_, double By_);

    ~WorkQueueItem(void);
};

#endif // WORK_QUEUE_ITEM_HPP
