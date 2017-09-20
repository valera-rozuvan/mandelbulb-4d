#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "app_state.hpp"
#include "generate_fractal.hpp"
#include "parallel.hpp"

void* workerFunction(void* appState_)
{
  AppState* appState = (AppState*)appState_;

  WorkQueueItem* nextWorkItem;

  do {
    pthread_mutex_lock(&(appState->parallel->todoWorkMutex));
    nextWorkItem = appState->parallel->todoWork->popRndItem();
    pthread_mutex_unlock(&(appState->parallel->todoWorkMutex));

    if (nextWorkItem != NULL) {
      generateFractal(appState, nextWorkItem);

      pthread_mutex_lock(&(appState->parallel->doneWorkMutex));
      appState->parallel->doneWork->pushItem(nextWorkItem);
      pthread_mutex_unlock(&(appState->parallel->doneWorkMutex));
    }
  } while ((nextWorkItem != NULL) && (appState->parallel->selfDestructing == false));

  pthread_exit((void*)0);

  return NULL;
}

void* startThreadFunc(void* appState_)
{
  AppState* appState = (AppState*)appState_;

  unsigned int block_size = appState->parallel->generationBlockSize;

  unsigned int i;
  unsigned int j;

  unsigned int leftOverI;
  unsigned int leftOverJ;

  unsigned int iMaxFloor = floor(((double)appState->wMandel) / ((double)block_size));
  unsigned int jMaxFloor = floor(((double)appState->hMandel) / ((double)block_size));

  for (i = 0; i < iMaxFloor; i += 1) {
    for (j = 0; j < jMaxFloor; j += 1) {
      appState->parallel->todoWork->initNewQueueItem(
        i * block_size, block_size * (i + 1) - 1,
        j * block_size, block_size * (j + 1) - 1
      );
    }
  }

  leftOverI = appState->wMandel - iMaxFloor * block_size;
  if (leftOverI > 0) {
    for (j = 0; j < jMaxFloor; j += 1) {
      appState->parallel->todoWork->initNewQueueItem(
        iMaxFloor * block_size, iMaxFloor * block_size + leftOverI  - 1,
        j * block_size, block_size * (j + 1) - 1
      );
    }
  }

  leftOverJ = appState->hMandel - jMaxFloor * block_size;
  if (leftOverJ > 0) {
    for (i = 0; i < iMaxFloor; i += 1) {
      appState->parallel->todoWork->initNewQueueItem(
        i * block_size, block_size * (i + 1) - 1,
        jMaxFloor * block_size, jMaxFloor * block_size + leftOverJ  - 1
      );
    }
  }

  if ((leftOverI > 0) && (leftOverJ > 0)) {
    appState->parallel->todoWork->initNewQueueItem(
      iMaxFloor * block_size, iMaxFloor * block_size + leftOverI  - 1,
      jMaxFloor * block_size, jMaxFloor * block_size + leftOverJ  - 1
    );
  }

  appState->parallel->createWorkers();
  appState->parallel->startThreadCleanup();

  pthread_exit((void*)0);

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
    pthread_join(this->workersT[i], NULL);
  }

  this->workerThreadsRunning = false;

  pthread_mutex_unlock(&(this->workerMutex));
}

Parallel::Parallel(
  AppState* appState_,
  unsigned int numThreads_,
  unsigned int generationBlockSize_
)
{
  this->appState = appState_;

  this->numThreads = numThreads_;

  this->generationBlockSize = generationBlockSize_;

  this->startThreadRunning = false;
  this->workerThreadsRunning = false;
  this->selfDestructing = false;

  this->startT = NULL;
  this->workersT = NULL;

  pthread_mutex_init(&(this->startMutex), NULL);
  pthread_mutex_init(&(this->workerMutex), NULL);
  pthread_mutex_init(&(this->todoWorkMutex), NULL);
  pthread_mutex_init(&(this->doneWorkMutex), NULL);

  this->todoWork = new WorkQueue();
  this->doneWork = new WorkQueue();
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
