#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "app_state.hpp"
#include "parallel.hpp"
#include "generate_fractal.hpp"

void* workerFunction(void* appState_)
{
  AppState* appState = (AppState*)appState_;

  // Do work.

  WorkQueueItem* nextWorkItem;

  do {
    pthread_mutex_lock(&(appState->parallel->todoWorkMutex));
    nextWorkItem = appState->parallel->todoWork->pop_rnd_item();
    pthread_mutex_unlock(&(appState->parallel->todoWorkMutex));

    if (nextWorkItem != NULL) {
      generate_fractal(appState, nextWorkItem);

      pthread_mutex_lock(&(appState->parallel->doneWorkMutex));
      appState->parallel->doneWork->push_item(nextWorkItem);
      pthread_mutex_unlock(&(appState->parallel->doneWorkMutex));
    }

    usleep(1);

  } while (nextWorkItem != NULL);

  pthread_exit((void*) 0);

  return NULL;
}

void* startThreadFunc(void* appState_)
{
  AppState* appState = (AppState*)appState_;

  const unsigned int block_size = 99;

  unsigned int i;
  unsigned int j;

  unsigned int left_over_i;
  unsigned int left_over_j;

  unsigned int i_max_floor = floor(((double)appState->wMandel) / ((double)block_size));
  unsigned int j_max_floor = floor(((double)appState->hMandel) / ((double)block_size));

  for (i = 0; i < i_max_floor; i += 1) {
    for (j = 0; j < j_max_floor; j += 1) {
      appState->parallel->todoWork->initNewQueueItem(
        i * block_size, block_size * (i + 1) - 1,
        j * block_size, block_size * (j + 1) - 1
      );
    }
  }

  left_over_i = appState->wMandel - i_max_floor * block_size;
  if (left_over_i > 0) {
    for (j = 0; j < j_max_floor; j += 1) {
      appState->parallel->todoWork->initNewQueueItem(
        i_max_floor * block_size, i_max_floor * block_size + left_over_i  - 1,
        j * block_size, block_size * (j + 1) - 1
      );
    }
  }

  left_over_j = appState->hMandel - j_max_floor * block_size;
  if (left_over_j > 0) {
    for (i = 0; i < i_max_floor; i += 1) {
      appState->parallel->todoWork->initNewQueueItem(
        i * block_size, block_size * (i + 1) - 1,
        j_max_floor * block_size, j_max_floor * block_size + left_over_j  - 1
      );
    }
  }

  if ((left_over_i > 0) && (left_over_j > 0)) {
    appState->parallel->todoWork->initNewQueueItem(
      i_max_floor * block_size, i_max_floor * block_size + left_over_i  - 1,
      j_max_floor * block_size, j_max_floor * block_size + left_over_j  - 1
    );
  }

  appState->parallel->createWorkers();
  appState->parallel->startThreadCleanup();

  pthread_exit((void*) 0);

  return NULL;
}

void Parallel::createStartThread(void)
{
  pthread_attr_t attr;

  if (this->selfDestructing == true) {
    return;
  }

  pthread_mutex_lock(&(this->startMutex));

  if (this->selfDestructing == true) {
    pthread_mutex_unlock(&(this->startMutex));

    return;
  }

  if ((this->startThreadRunning == false) && (this->workerThreadsRunning == false)) {
    this->startThreadRunning = true;

    if (this->startT != NULL) {
      free(this->startT);
    }
    this->startT = (pthread_t *)calloc(sizeof(pthread_t) * 1, sizeof(pthread_t));

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(this->startT, &attr, startThreadFunc, (void*)(this->appState));

    pthread_attr_destroy(&attr);
  }

  pthread_mutex_unlock(&(this->startMutex));
}

void Parallel::startThreadCleanup(void)
{
  pthread_mutex_lock(&(this->startMutex));

  this->startThreadRunning = false;

  pthread_mutex_unlock(&(this->startMutex));
}

void Parallel::createWorkers(void)
{
  unsigned int i;
  pthread_attr_t attr;

  if (this->selfDestructing == true) {
    return;
  }

  pthread_mutex_lock(&(this->workerMutex));

  if (this->selfDestructing == true) {
    pthread_mutex_unlock(&(this->workerMutex));

    return;
  }

  this->workerThreadsRunning = true;

  this->workersT = (pthread_t *)calloc(sizeof(pthread_t) * this->numThreads, sizeof(pthread_t));

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  for (i = 0; i < this->numThreads; i += 1) {
    pthread_create(&(this->workersT[i]), &attr, workerFunction, (void*)(this->appState));
  }

  pthread_attr_destroy(&attr);

  pthread_mutex_unlock(&(this->workerMutex));
}

void Parallel::stopWorkers(void)
{
  unsigned int i;

  pthread_mutex_lock(&(this->workerMutex));

  if (this->workerThreadsRunning == false) {
    return;
  }

  for (i = 0; i < this->numThreads; i += 1) {
    pthread_cancel(this->workersT[i]);
    pthread_join(this->workersT[i], NULL);
  }

  this->workerThreadsRunning = false;

  pthread_mutex_unlock(&(this->workerMutex));
}

Parallel::Parallel(AppState* appState_, unsigned int numThreads_)
{
  this->appState = appState_;

  this->numThreads = numThreads_;

  this->startThreadRunning = false;
  this->stopThreadRunning = false;
  this->workerThreadsRunning = false;
  this->selfDestructing = false;

  this->startT = NULL;
  this->stopT = NULL;
  this->workersT = NULL;

  pthread_mutex_init(&(this->startMutex), NULL);
  pthread_mutex_init(&(this->workerMutex), NULL);
  pthread_mutex_init(&(this->todoWorkMutex), NULL);
  pthread_mutex_init(&(this->doneWorkMutex), NULL);

  this->todoWork = new WorkQueue();

  // this->todoWork->initNewQueueItem(1, 2, 3, 4);
  // this->todoWork->initNewQueueItem(20, 30, 40, 50);
  // this->todoWork->initNewQueueItem(300, 400, 500, 600);

  this->doneWork = new WorkQueue();

  // this->doneWork->push_item(this->todoWork->pop_rnd_item());
  // this->doneWork->push_item(this->todoWork->pop_rnd_item());
  // this->doneWork->push_item(this->todoWork->pop_rnd_item());
  // this->doneWork->push_item(this->todoWork->pop_rnd_item());
}

Parallel::~Parallel(void)
{
  fprintf(stdout, "Parallel::~Parallel destructor.\n");

  // We need to make sure that no new Start Thread is initializing.
  // When we are sure, set the "selfDestructing" flag to "true".
  // This will prevent any new Start Thread from starting.
  pthread_mutex_lock(&(this->startMutex));
  this->selfDestructing = true;
  pthread_mutex_unlock(&(this->startMutex));

  // If some StartThread is running, we need to wait for it to exit.
  while (this->startThreadRunning == true) {}

  pthread_mutex_destroy(&(this->startMutex));

  if (this->startT != NULL) {
    free(this->startT);
  }

  this->stopWorkers();

  pthread_mutex_destroy(&(this->workerMutex));

  if (this->workersT != NULL) {
    free(this->workersT);
  }

  this->todoWork->clearQueue();
  this->doneWork->clearQueue();

  delete this->doneWork;
  delete this->todoWork;
  pthread_mutex_destroy(&(this->todoWorkMutex));
  pthread_mutex_destroy(&(this->doneWorkMutex));
}
