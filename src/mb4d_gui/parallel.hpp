#ifndef PARALLEL_HPP
#define PARALLEL_HPP

#include "work_queue.hpp"

class Parallel {
  WorkQueue* workQueue;

  public:
    unsigned int num_threads;

    Parallel(void);
    ~Parallel(void);
};

#endif
