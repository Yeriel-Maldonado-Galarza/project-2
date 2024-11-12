#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <sys/resource.h>
#include <sched.h>
#include <signal.h>
#include <string.h>
#include <wait.h>
#include "functions.h"
#define N_SAMPLES 1
#define PERIOD_MS 500
#define PERIOD_S 1
int counter = 0;

extern int flag = 0;
/**
 * @brief Sets up timers and signals
 *
 * @return int
 */
int main()
{
  timer_t timer_id;
  timer_create(CLOCK_REALTIME, NULL, &timer_id);
  struct sigaction action;

  action.sa_handler = TimerHandlerWithSignals;
  action.sa_flags = 0;
  sigaction(SIGALRM, &action, NULL);

  struct sigaction child;
  child.sa_handler = ChildCuller;
  child.sa_flags = 0;
  sigaction(SIGCHLD, &child, NULL);

  /*Se configuran los parámetros del timer*/
  struct itimerspec timerParams;
  /*Primer disparo*/
  timerParams.it_value.tv_sec = PERIOD_S;
  timerParams.it_value.tv_nsec = (PERIOD_MS) * 1000000;
  /*Como es diferente de cero el timer es periódico*/
  timerParams.it_interval.tv_sec = PERIOD_S;
  timerParams.it_interval.tv_nsec = (PERIOD_MS) * 1000000;

  /*Se arranca el timer (tiempo relativo)*/
  timer_settime(timer_id, 0, &timerParams, NULL);

  struct timespec start;

  struct timespec end;

  clock_gettime(CLOCK_REALTIME, &start);
  while (1)
  {
    usleep(100000);
    if (flag == 1)
    {
      flag--;

      pid_t pid;
      pid = fork();
      if (pid == 0)
      {
        execlp("./imu_driver", "./imu_driver", NULL);
      }
    }
  }
  clock_gettime(CLOCK_REALTIME, &end);
  double temp = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1E9;
  char buf[64];
  snprintf(buf, sizeof(buf), "%f\n", temp);
  write(1, buf, strlen(buf));
}
