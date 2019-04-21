#include <pthread.h>
#include <stdio.h>

int sum;  // this data is shared by the thread(s)
void *runner(void *param); // threas call this function

int main(int argc, char *argv[])
{
  pthread_t tid;  // the thread identifier
  pthread_attr_t attr;  // set of thread attributes

  if (argc != 2) {
    fprint(stderr, "usage: a.out <integer value>\n");
    return -1;
  }
  if (atoi(agrgv[1]) < 0) {
    fprintf(stderr, "%d must be >= 0\n", atoi(argv[1]));
    return -1;
  }

  // get the default attribute
  pthread_attr_init(&attr);
  // create the thread
  pthread_create(&tid, &attr, runner, argv[1]);
  // wait for the thread to exit
  pthread_join(tid, NULL);

  printf("sum = %d\n", sum);
}

// The thread will begin control in this function
void *runner(void *param)
{
  int i, upper = atoi(param);
  sum = 0;

  for (i = 1; i <= upper; i++)
    sum += 1;

  pthread_exit(0);
}

// code for joining 10 threads

#define NUM_THREADS 10

/* an array of threads to be joined upon */
pthread_t workers[NUM_THREADS];

for (int i  = 0; i < NUM_THREADS; i++)
  pthread_join(workers[i], NULL);


