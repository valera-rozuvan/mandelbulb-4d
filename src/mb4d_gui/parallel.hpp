#ifndef PARALLEL_HPP
#define PARALLEL_HPP

#include <pthread.h>
#include <stdbool.h>
#include "work_queue.hpp"

class AppState;

class Parallel {
  private:
    AppState* appState;

    pthread_t* startT;
    pthread_t* workersT;

    bool startThreadRunning;
    bool workerThreadsRunning;

    unsigned int numThreads;

  public:
    WorkQueue* todoWork;
    WorkQueue* doneWork;

    pthread_mutex_t startMutex;
    pthread_mutex_t workerMutex;
    pthread_mutex_t todoWorkMutex;
    pthread_mutex_t doneWorkMutex;

    bool selfDestructing;

    unsigned int generationBlockSize;

    void createStartThread(void);
    void startThreadCleanup(void);

    void createWorkers(void);
    void stopWorkers(void);

    Parallel(AppState*, unsigned int, unsigned int);
    ~Parallel(void);
};

#endif // PARALLEL_HPP
