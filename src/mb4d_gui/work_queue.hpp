#ifndef WORK_QUEUE_HPP
#define WORK_QUEUE_HPP

#include "work_queue_item.hpp"

class WorkQueue {
    unsigned int num_of_queue_items;

    WorkQueueItem* workQueueItems;
  public:
    WorkQueueItem* getRandomQueueItem(void);
    void addNewQueueItem(double Ax, double Ay, double Bx, double By);
    // void removeQueueItem(WorkQueueItem* workQueueItem);
    void clearQueue(void);

    void test_func(void);

    // unsigned int getNumberOfQueueItems(void);

    WorkQueue(void);
    ~WorkQueue(void);
};

#endif // WORK_QUEUE_HPP
