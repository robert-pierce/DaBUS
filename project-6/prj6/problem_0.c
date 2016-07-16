#include <stdio.h>
#include <assert.h>
#include <pthread.h>

pthread_t p1, p2;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond  = PTHREAD_COND_INITIALIZER;
int count, finished;

void Pthread_mutex_lock(pthread_mutex_t *mutex) {
  int rc = pthread_mutex_lock(mutex);
  assert(rc == 0);
}

void Pthread_mutex_unlock(pthread_mutex_t *mutex) {
  int rc = pthread_mutex_unlock(mutex);
  assert(rc == 0);
}

void Pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
  int rc = pthread_cond_wait(cond, mutex);
  assert(rc == 0);
}

void Pthread_cond_signal(pthread_cond_t *cond) {
  int rc = pthread_cond_signal(cond);
  assert(rc == 0);
}

void Pthread_create(pthread_t *p, void * (*increment) (void*), void *arg) {
  int rc = pthread_create(p, NULL, increment, arg);
  assert(rc == 0);
}

void *increment(void *arg) {
   int *i = (int*) arg;
  
  while(*i <= 1000) {
    Pthread_mutex_lock(&lock);
    (*i)++;
    printf("count = %d\n", *i);
    
    if(*i == 1000) {
      finished = 1;
      Pthread_cond_signal(&cond);
    }

    Pthread_mutex_unlock(&lock);
  }  
}

int main() {
  count = 0;
  Pthread_create(&p1, increment, &count);
  Pthread_create(&p2, increment, &count);
 
  Pthread_mutex_lock(&lock);
  while(!finished) {
    Pthread_cond_wait(&cond, &lock);
    Pthread_mutex_unlock(&lock);
  }

  printf("------ The main process is shutting down-----  ");

  return 0;
}
