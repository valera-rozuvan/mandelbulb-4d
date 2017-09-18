#include <stdio.h>
#include "app_state.hpp"
#include "parallel.hpp"
#include "generate_fractal.hpp"

void* startThreadFunc(void* appState_)
{
  AppState* appState = (AppState*)appState_;

  generate_fractal(appState);

  appState->parallel->startThreadEnd();

  pthread_exit((void*) 0);

  return NULL;
}

Parallel::Parallel(AppState* appState_, unsigned int num_threads_)
{
  this->appState = appState_;

  this->num_threads = num_threads_;

  this->startThreadRunning = false;
  this->stopThreadRunning = false;
  this->startT = NULL;
  this->stopT = NULL;

  pthread_mutex_init(&(this->start_mutex), NULL);
  pthread_mutex_init(&(this->stop_mutex), NULL);
  pthread_mutex_init(&(this->todo_work_mutex), NULL);
  pthread_mutex_init(&(this->done_work_mutex), NULL);

  this->todo_work = new WorkQueue();

  this->todo_work->initNewQueueItem(1, 2, 3, 4);
  this->todo_work->initNewQueueItem(20, 30, 40, 50);
  this->todo_work->initNewQueueItem(300, 400, 500, 600);

  this->done_work = new WorkQueue();

  // this->done_work->push_item(this->todo_work->pop_rnd_item());
  // this->done_work->push_item(this->todo_work->pop_rnd_item());
  // this->done_work->push_item(this->todo_work->pop_rnd_item());
  // this->done_work->push_item(this->todo_work->pop_rnd_item());
}

Parallel::~Parallel(void)
{
  fprintf(stdout, "Parallel::~Parallel destructor.\n");

  if (this->startT != NULL) {
    pthread_cancel(*(this->startT));

    pthread_join(*(this->startT), NULL);
  }

  pthread_mutex_destroy(&(this->start_mutex));
  pthread_mutex_destroy(&(this->stop_mutex));
  pthread_mutex_destroy(&(this->todo_work_mutex));
  pthread_mutex_destroy(&(this->done_work_mutex));

  delete this->done_work;
  delete this->todo_work;
}

void Parallel::start(void)
{
  pthread_mutex_lock(&(this->start_mutex));
  pthread_mutex_lock(&(this->stop_mutex));

  if (this->startThreadRunning == false) {
    this->startThreadRunning = true;

    if (this->startT != NULL) {
      delete this->startT;
    }
    this->startT = new pthread_t;

    // TODO: Make this thread detached. System will take care of
    //       freeing resources used by this thread.
    pthread_create(this->startT, NULL, startThreadFunc, (void*)(this->appState));
  }

  pthread_mutex_unlock(&(this->stop_mutex));
  pthread_mutex_unlock(&(this->start_mutex));
}

void Parallel::startThreadEnd(void)
{
  pthread_mutex_lock(&(this->start_mutex));
  this->startThreadRunning = false;
  pthread_mutex_unlock(&(this->start_mutex));
}
