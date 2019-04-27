/* 1. Create n students. */
/* 2. Each student and a TA will run on a separate thread. */
/* 3. Student thread will alternate between programming for a period of time and seeking help from the TA. */
/* 4. If TA available, obtain help. */
/* 5. Or sit in a chare or resume programming. */
/* 6. Student wakes up the TA using a semaphore. */
/* 7. The TA checks if there are waiting student on the chairs. */
/* 8. Let threads sleep for a random period of time to simulate students programming and the TA providing help to a student. */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define N_STUDENTS 5
#define MAX_WAITING 3
#define MAX_RANDOM_TIME 5

int n_want_help = 0;
int next_in_line = 0;
int line[MAX_WAITING] = { -1 };
enum { FREE, NAPPING, OCCUPIED } t_state;

pthread_mutex_t mutex;

void goto_help(int *student_num);
void consult_ta(int *student_num);
void finish_consulting();

void* runner(void* arg);

int main(int argc, char *argv[])
{
  pthread_t students[N_STUDENTS];
  pthread_mutex_init(&mutex, NULL);
  int student_nums[N_STUDENTS];
  for (int i = 0; i < N_STUDENTS; i++)
    student_nums[i] = i + 1;

  t_state = FREE;
  for (int i = 0; i < N_STUDENTS; i++)
    pthread_create(&students[i], NULL, runner, &student_nums[i]);

  // wait for the thread to exit
  for (int i  = 0; i < N_STUDENTS; i++)
    pthread_join(students[i], NULL);

  pthread_mutex_destroy(&mutex);

  return 0;
}

void* runner(void* arg) {
  int *student_num = (int *) arg;

  for (int i = 0; i < 3; i++) {
    pthread_mutex_lock(&mutex);

    printf("Student %d is going to help.\n", *student_num);

    if (n_want_help >= MAX_WAITING)
      continue;

    if ((line[next_in_line == -1] || line[next_in_line] == *student_num) && t_state == FREE) {
      printf("TA catched. Consulting student %d!\n", *student_num);
      n_want_help--;
      line[next_in_line] = -1;
      next_in_line = (next_in_line + 1) % MAX_WAITING;

      t_state = OCCUPIED;
      int some_time = rand() % MAX_RANDOM_TIME;
      sleep(some_time);
      printf("\nStudent %d finished consulting.\n", *student_num);
      t_state = FREE;
      printf("Student %d is leaving.\n", *student_num);
    }
    else {
      printf("Student %d is lining up.\n", *student_num);
      line[(next_in_line + n_want_help) % MAX_WAITING] = *student_num;
      n_want_help++;
    }
    sleep(1);

    pthread_mutex_unlock(&mutex);
  };

  pthread_exit(0);
}

void goto_help(int *student_num) {
  printf("Inside goto_help.\n");
  /* printf("Inside goto_help. next_in_line: %d. t_state: %u", *student_num, t_state); */
}

void consult_ta(int *student_num) {
  n_want_help--;
  next_in_line = (next_in_line + 1) % MAX_WAITING;

  t_state = OCCUPIED;
  int some_time = rand() / MAX_RANDOM_TIME;
  sleep(some_time);
  printf("\nStudent %d finished consulting.\n", *student_num);
  t_state = FREE;
}

