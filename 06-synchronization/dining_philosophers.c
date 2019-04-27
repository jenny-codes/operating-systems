/* 1. Each philosopher runs as a separate thread. */
/* 2. Philosophers alternate between thinking and eating. */
/* 3. To simulate both activities, have the thread sleep for a random period between one and three seconds. */
/* 4. Invoke `pickup_forks()` when wishing to eat; `return_forks()` on finishing. */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N_PHILOSOPHERS 5
#define LEFT (ph_num + N_PHILOSOPHERS - 1) % N_PHILOSOPHERS
#define RIGHT (ph_num + 1) % N_PHILOSOPHERS

sem_t mutex;
sem_t condition[N_PHILOSOPHERS];

enum { THINKING, HUNGRY, EATING } state[N_PHILOSOPHERS];
int phil_num[N_PHILOSOPHERS];

void * philosophing (void *arg);
void pickup_forks(int ph_num);
void return_forks(int ph_num);
void test(int ph_num);

int main(int argc, char *argv[])
{
  printf("BEGINNING");
  pthread_t ph_thread[N_PHILOSOPHERS];
  sem_init(&mutex, 0, 1);
  for (int i = 0; i < N_PHILOSOPHERS; i++) {
    pthread_create(&ph_thread[i], NULL, philosophing, &phil_num[i]);
    printf("Philosopher #%d starts thinking.", phil_num[i]);
  }

  for (int i = 0; i < N_PHILOSOPHERS; i++)
    pthread_join(ph_thread[i], NULL);

  return(0);
}

void *philosophing(void *arg)
{
  while(1) {
    int *ph_num = (int *) arg;
    sleep(1);
    pickup_forks(*ph_num);
    sleep(1);
    return_forks(*ph_num);
  }
}

void pickup_forks(int ph_num) {
  sem_wait(&mutex);
  state[ph_num] = HUNGRY;
  printf("Philosopher #%d is HUNGRY.", phil_num[ph_num]);
  test(ph_num);
  sem_post(&mutex);
  sem_wait(&condition[ph_num]);
  sleep(1);
}

void return_forks(int ph_num) {
  sem_wait(&mutex);
  condition[ph_num] = THINKING;
  printf("Philosopher #%d is done eating. Now she returns to thinking.", phil_num[ph_num]);
  test(LEFT);
  test(RIGHT);
  sem_post(&mutex);
}

void test(int ph_num) {
  if (state[ph_num] == HUNGRY && 
      state[LEFT] != EATING && 
      state[RIGHT] != EATING) {
    state[ph_num] = EATING;
    printf("Philosopher #%d is EATING.", phil_num[ph_num]);
    sem_post(&condition[ph_num]);
  }
}

/* reference: https://c-program-example.com/2012/02/c-program-to-solve-dining-philosophers-problem.html */
