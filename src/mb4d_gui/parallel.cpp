#include <stdio.h>
#include <stdlib.h>
#include "app_state.hpp"
#include "parallel.hpp"
#include "generate_fractal.hpp"

void* workerFunction(void* appState_)
{
  AppState* appState = (AppState*)appState_;

  // Do work.

  pthread_exit((void*) 0);

  return NULL;
}

void* startThreadFunc(void* appState_)
{
  AppState* appState = (AppState*)appState_;

  appState->parallel->createWorkers();
  appState->parallel->startThreadCleanup();

  pthread_exit((void*) 0);

  return NULL;
}

void Parallel::createStartThread(void)
{
  pthread_attr_t attr;

  pthread_mutex_lock(&(this->startMutex));

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

  this->workerThreadsRunning = true;

  this->workersT = (pthread_t *)calloc(sizeof(pthread_t) * this->numThreads, sizeof(pthread_t));

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  for (i = 0; i < this->numThreads; i += 1) {
    pthread_create(&(this->workersT[i]), &attr, workerFunction, (void*)(this->appState));
  }

  pthread_attr_destroy(&attr);
}

void Parallel::stopWorkers(void)
{
  unsigned int i;

  if ((this->workerThreadsRunning == false) || (this->workersT == NULL)) {
    return;
  }

  for (i = 0; i < this->numThreads; i += 1) {
    pthread_cancel(this->workersT[i]);
    pthread_join(this->workersT[i], NULL);
  }

  this->workerThreadsRunning = false;
}

Parallel::Parallel(AppState* appState_, unsigned int numThreads_)
{
  this->appState = appState_;

  this->numThreads = numThreads_;

  this->startThreadRunning = false;
  this->stopThreadRunning = false;
  this->workerThreadsRunning = false;

  this->startT = NULL;
  this->stopT = NULL;
  this->workersT = NULL;

  pthread_mutex_init(&(this->startMutex), NULL);
  pthread_mutex_init(&(this->stopMutex), NULL);
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

  pthread_mutex_destroy(&(this->startMutex));
  pthread_mutex_destroy(&(this->stopMutex));
  pthread_mutex_destroy(&(this->todoWorkMutex));
  pthread_mutex_destroy(&(this->doneWorkMutex));

  if (this->startT != NULL) {
    free(this->startT);
  }

  this->stopWorkers();
  if (this->workersT != NULL) {
    free(this->workersT);
  }

  delete this->doneWork;
  delete this->todoWork;
}
