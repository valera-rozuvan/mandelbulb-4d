#ifndef PARALLEL_HPP
#define PARALLEL_HPP

#include <pthread.h>
#include <stdbool.h>
#include "work_queue.hpp"

class AppState;

class Parallel {
  WorkQueue* todo_work;
  WorkQueue* done_work;
  AppState* appState;

  pthread_t* startT;
  pthread_t* stopT;
  pthread_t* workersT;

  bool startThreadRunning;
  bool stopThreadRunning;

  unsigned int num_threads;

  pthread_mutex_t start_mutex;
  pthread_mutex_t stop_mutex;
  pthread_mutex_t todo_work_mutex;
  pthread_mutex_t done_work_mutex;

  public:
    void start(void);
    void startThreadEnd(void);

    Parallel(AppState*, unsigned int);
    ~Parallel(void);
};

#endif
