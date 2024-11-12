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
#include "functions.h"

#define PERIOD_MS 200

extern struct timespec start;
/**
 * @brief timer is created and set up to execute every 200ms
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

  /// Se crea el timer

  /*Se configuran los parámetros del timer*/
  struct itimerspec timerParams;
  /*Primer disparo*/
  timerParams.it_value.tv_sec = 0;
  timerParams.it_value.tv_nsec = PERIOD_MS * 1000000;
  /*Como es diferente de cero el timer es periódico*/
  timerParams.it_interval.tv_sec = 0;
  timerParams.it_interval.tv_nsec = PERIOD_MS * 1000000;

  /*Se arranca el timer (tiempo relativo)*/
  timer_settime(timer_id, 0, &timerParams, NULL);
  clock_gettime(CLOCK_REALTIME, &start);

  while (1)
  {
  }
}
