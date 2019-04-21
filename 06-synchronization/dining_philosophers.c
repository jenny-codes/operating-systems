int pickup_forks(int philosopher_number) {
  return 0;
}

int return_forks(int philosopher_number) {
  return 0;
}


/* 1. Each philosopher runs as a separate thread. */
/* 2. Philosophers alternate between thinking and eating. */
/* 3. To simulate both activities, have the thread sleep for a random period between one and three seconds. */
/* 4. Invoke `pickup_forks()` when wishing to eat; `return_forks()` on finishing. */
/* 5. C does not have a monitor, so we will assicate a conditional variable with a mutex lock. */
/* 6. Conditional Variable code below: */

pthread_mutex_t mutex;
pthread_cond_t cond_var;

pthread_mutex_init(&mutex, NULL);
pthread_cond_init(&cond_var, NULL);

/* when a thread waits for a condition to be true */
pthread_mutex_lock(&mutex);  // acquire the lock
while (a != b)
  pthread_cond_wait(&mutex, &cond_var);

pthread_mutex_unlock(&mutex);

/* a thread that modifies the shared data */
pthread_mutex_lock(&mutex);  // acquire the lock
a = b;
pthread_cond_signal(&cond_var);
pthread_mutex_unlock(&mutex);
