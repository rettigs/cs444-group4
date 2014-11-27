#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>   // sleep()

typedef struct _thread_data_t {
  int tid;
  int wait;
} thread_data_t;

sem_t resource;
pthread_barrier_t barrier;
volatile int threads_active;
volatile int set_lock;
pthread_cond_t cond;
pthread_mutex_t lock;

void * thread_task(void *arg);

int main(int argc, char *argv[])
{
  srand(32);
  sem_init(&resource, 0, 3);
  pthread_barrier_init(&barrier, NULL, 3);
  pthread_cond_init(&cond, NULL);
  pthread_mutex_init(&lock, NULL);
  threads_active = 0;
  set_lock = 0;

  int num_threads = 3;
  pthread_t * threads;
  thread_data_t * thread_dataset;
  
  if (argc >= 2) {
    num_threads = atoi(argv[1]);
  }

  printf("number of threads: %d\n\n", num_threads);
  threads = malloc(num_threads * sizeof(pthread_t));
  thread_dataset = malloc(num_threads * sizeof(thread_data_t));

  int i;
  for (i = 0; i < num_threads; i++) {
    thread_dataset[i].tid = i;
    thread_dataset[i].wait = rand() % 10;

    // lock until all threads have exited the resource
    pthread_mutex_lock(&lock);
    while(set_lock || threads_active >= 3) {
      pthread_cond_wait(&cond, &lock);
    }
    pthread_mutex_unlock(&lock);

    int rc = pthread_create(&threads[i], NULL, thread_task, &thread_dataset[i]);
    if (rc) {
      fprintf(stderr, "error: pthread_create, return code : %d\n", rc);
      exit(EXIT_FAILURE);
    }
  }

  for (i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  sem_destroy(&resource);
  free(thread_dataset);
  free(threads);
  exit(EXIT_SUCCESS);
}

void * thread_task(void *arg)
{
  thread_data_t *data = (thread_data_t *)arg;

  printf("thread %d: running\n", data->tid);

  sem_wait(&resource);
  printf("thread %d: obtained semaphore\n", data->tid);

  pthread_mutex_lock(&lock);
  threads_active++;
  if (threads_active >= 3) {
    set_lock = 1;
  }
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&lock);

  printf("thread %d: using resource\n", data->tid);
  sleep(data->wait);
  printf("thread %d: resource released\n", data->tid);

  pthread_barrier_wait(&barrier);
  printf("thread %d: passed barrier\n", data->tid);

  pthread_mutex_lock(&lock);
  threads_active--;
  if (threads_active == 0) {
    set_lock = 0;
  }
  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&lock);  

  printf("thread %d: released semaphore\n", data->tid);
  sem_post(&resource);

  printf("thread %d: exiting\n", data->tid);
  pthread_exit(NULL);
}