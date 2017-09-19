#ifndef WORK_QUEUE_HPP
#define WORK_QUEUE_HPP

#include "work_queue_item.hpp"

class WorkQueue {
    unsigned int num_of_queue_items;

    WorkQueueItem* workQueueItems;
  public:
    void initNewQueueItem(unsigned int, unsigned int, unsigned int, unsigned int);
    void push_item(WorkQueueItem*);

    void clearQueue(void);

    WorkQueueItem* pop_nth_item(const unsigned int);
    WorkQueueItem* pop_rnd_item(void);

    WorkQueue(void);
    ~WorkQueue(void);
};

#endif // WORK_QUEUE_HPP
