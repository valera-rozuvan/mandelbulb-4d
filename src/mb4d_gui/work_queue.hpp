#ifndef WORK_QUEUE_HPP
#define WORK_QUEUE_HPP

#include "work_queue_item.hpp"

class WorkQueue {
    unsigned int num_of_queue_items;

    WorkQueueItem* workQueueItems;
  public:
    void initNewQueueItem(unsigned int, unsigned int, unsigned int, unsigned int);
    void pushItem(WorkQueueItem*);

    void clearQueue(void);

    WorkQueueItem* popNthItem(const unsigned int);
    WorkQueueItem* popRndItem(void);

    WorkQueue(void);
    ~WorkQueue(void);
};

#endif // WORK_QUEUE_HPP
