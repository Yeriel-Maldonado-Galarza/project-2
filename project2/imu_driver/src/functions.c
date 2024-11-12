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

struct timespec temptime;
double prev = 0;
FILE *file_ptr;
struct timespec start;
robot *r1_ptr;
int beginning = 0;
int lines = 0;
/**
 * @brief Every 200ms timer sends a signal to run this function, here the robots values are updated.
 *
 * @param x
 */
void TimerHandlerWithSignals(int x)
{
  if (access("../../imu_data.txt", F_OK) == 0 && beginning == 0)
  { // if the file existed prior to this line running we should read its  content to continue where it left off

    file_ptr = fopen("../../imu_data.txt", "r");

    int lines = 0;
    // if i read two \n in a row ive reached the end of the file
    char c;
    for (c = getc(file_ptr); c != EOF; c = getc(file_ptr)) // count the amount of lines in the file
    {
      if (c == '\n')
      {
        lines++;
      }
    }

    robot r1;
    r1.time = .2 * lines; // estimate the elapsed time by multiplying .2 by the amount of lines
    r1.acc_x = .15;
    r1.acc_y = .06;
    r1.spd_x = 0 + (r1.acc_x * (r1.time)); // acceleration is linear thus speed can be calculated off of speed and time
    r1.spd_y = 0 + (r1.acc_y * (r1.time));

    // 𝑝𝑥(𝑖) = 𝑝𝑥(𝑖 − 1) + 𝑣𝑥(𝑖) ∗ 𝑡
    // pos = p0 + vot + 1/2 t^2 with initial speed =0
    r1.pos_x = 20 + (0.5 * (r1.time * r1.time));
    r1.pos_y = 10 + (0.5 * (r1.time * r1.time));

    r1_ptr = &r1;
    fclose(file_ptr);
    beginning = 1;
  }
  if (beginning == 0) // if its the first time running and the file was not creaed previous it will set the robots initial values
  {
    robot r1;
    r1.time = 0;
    r1.pos_x = 20;
    r1.pos_y = 10;
    r1.spd_x = 0;
    r1.spd_y = 0;
    r1.acc_x = .15;
    r1.acc_y = .06;

    r1_ptr = &r1;
    beginning = 1;
  }
  else
  {

    robot r1 = *r1_ptr;
    // creates a text fle in the parent folder
    file_ptr = fopen("../../imu_data.txt", "a");

    fprintf(file_ptr, "%.2f,", r1.time);
    fprintf(file_ptr, "%.4f,", r1.pos_x);
    fprintf(file_ptr, "%.4f,", r1.pos_y);
    fprintf(file_ptr, "%.4f,", r1.spd_x);
    fprintf(file_ptr, "%.4f,", r1.spd_y);
    fprintf(file_ptr, "%.4f,", r1.acc_x);
    fprintf(file_ptr, "%.4f,\n", r1.acc_y);
    fclose(file_ptr);

    clock_gettime(CLOCK_REALTIME, &temptime);
    double temp = (temptime.tv_sec - start.tv_sec) + (temptime.tv_nsec - start.tv_nsec) / 1E9;

    double dummy = temp;
    temp = temp - prev;
    prev = dummy;
    r1.time += temp;
    // 𝑣𝑥(𝑖) = 𝑣𝑥(𝑖 − 1) + 𝑎𝑐𝑐𝑥(𝑖) ∗ 𝑡
    r1.spd_x = r1.spd_x + (r1.acc_x * temp);
    r1.spd_y = r1.spd_y + (r1.acc_y * temp);

    // 𝑝𝑥(𝑖) = 𝑝𝑥(𝑖 − 1) + 𝑣𝑥(𝑖) ∗ 𝑡
    r1.pos_x = r1.pos_x + (r1.spd_x * temp);
    r1.pos_y = r1.pos_y + (r1.spd_y * temp);
    r1_ptr = &r1;
  }
}