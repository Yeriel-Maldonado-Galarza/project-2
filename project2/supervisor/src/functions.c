#include "functions.h"

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
/**
 * @brief Get the Pid By Name object
 *
 * @param process_name name of the process whose PID im looking for,  namely imu_data
 * @return int
 */
int GetPidByName(const char *process_name)
{
  char command[256];
  snprintf(command, sizeof(command), "pgrep -x %s", process_name);
  FILE *fp = popen(command, "r");
  if (fp == NULL)
  {
    perror("popen");
    return -1;
  }
  int pid = -1;
  if (fscanf(fp, "%d", &pid) == 1)
  {
    // Successfully retrieved a PID
    char buf[256];
    snprintf(buf, sizeof(buf), "pid of imu_data %d\n", pid);
    write(1, buf, strlen(buf));
  }
  pclose(fp);
  return pid;
}
/**
 * @brief sets a flaf whenever the getpid by name function cant find the pid of imu_data
 *
 * @param x
 */
void TimerHandlerWithSignals(int x)
{
  int get_pid = GetPidByName("imu_driver");
  if (get_pid == -1)
  {
    flag = 1;
  }
}
/**
 * @brief Waits for children to die
 *
 * @param signal SIGCHLD
 */
void ChildCuller(int signal)
{
  while (waitpid(-1, NULL, WNOHANG) > 0)
  {
  }
}