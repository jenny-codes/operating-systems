/* 1. Create n students. */
/* 2. Each student and a TA will run on a separate thread. */
/* 3. Student thread will alternate between programming for a period of time and seeking help from the TA. */
/* 4. If TA available, obtain help. */
/* 5. Or sit in a chare or resume programming. */
/* 6. Student wakes up the TA using a semaphore. */
/* 7. The TA checks if there are waiting student on the chairs. */
/* 8. Let threads sleep for a random period of time to simulate students programming and the TA providing help to a student. */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N_STUDENTS 5
#define MAX_WAITING 3
#define MAX_RANDOM_TIME 5

int n_want_help = 0;
int next_in_line = 0;
int line[MAX_WAITING];
enum { FREE, NAPPING, OCCUPIED } t_state;

sem_t mutex;

void goto_help(*student_num);
void consult_ta(*student_num);
void finish_consulting();

void* runner(void* arg);

void goto_help(*student_num) {
  if (n_want_help >= MAX_WAITING)
    return;
  
  if (next_in_line == *student_num && t_state == FREE) {
    sem_wait(&mutex);
    consult_ta(student_num);
    printf("\nStudent %d is leaving.\n", n);
  }
  else {
    printf("\nStudent %d is lining up.\n", n);
    line[(next_in_line + n_want_help) % MAX_WAITING] = n;
    n_want_help++;
  }
}

void consult_ta() {
  n_want_help--;
  next_in_line = (next_in_line + 1) % MAX_WAITING;

  t_state = OCCUPIED;
  some_time = rand() / MAX_RANDOM_TIME;
  sleep(some_time);
  printf("\nStudent %d finished consulting.\n", n);
  t_state = FREE;

  sem_post(&mutex);
}

void* runner(void* arg) {
  do {
    int student_num = (int *) arg;
    printf("\nStudent %d is going to help\n", student_num);
    goto_help(&student_num);
    sleep(3);
  } while (true);
}

int main(int argc, char *argv[])
{
  pthread_t students[N_STUDENTS];
  sem_init(&mutex, 0, MAX_WAITING);

  t_state = FREE;
  for (int i = 0; i < N_STUDENTS; i++) {
    pthread_create(&students[i], NULL, runner, &i);
  }

  // wait for the thread to exit
  for (int i  = 0; i < N_STUDENTS; i++)
    pthread_join(students[i], NULL);
  pthread_join(ta[i], NULL);

  sem_destory(&mutex);
  return 0;
}
