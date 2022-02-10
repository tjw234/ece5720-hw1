/** @author Tyrone Whitmore-Wilson (tjw234)
 * @file tjw234_hw1_1.c
 * @brief Row by column multiplication example for ECE 5720
 * @version 0.1
 * @date 2022-02-08
 *
 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// set range for testing
#define MIN_SIZE 1 << 5
#define MAX_SIZE 1 << 11

// Flags for operation optinos
#define RAND_FLAG 0b1

int main(int argc, char** argv)
{

  // loop parameters since we cant use c99
  int i, j, k, row, column;
  // other parameters
  int FLAGS;
  char time_write_buffer[32];
  int log_size = 5;
  
  // open file to record time measurements
  FILE* fp = NULL;
  fp = fopen("rbyc.csv", "w");

  // declare matrices
  double **a, **b, **c;

  // time measurement variables
  double time;
  struct timespec start, end;

  // get clock resolution
  clock_getres(CLOCK_MONOTONIC, &start);
  printf("resolution of CLOCK_MONOTONIC is %ld ns\n", start.tv_nsec);

  // if using random matrices, set seed srand48(1);
  int n = MAX_SIZE;

  // for check of correctness use special matrices
  // then set matrices to what is needed

  // allocate memory and initialize a
  a = (double**)malloc(n * sizeof(double*));
  for (i = 0; i < n; i++) {
    a[i] = (double*)malloc(n * sizeof(double));
    for (j = 0; j < n; j++) {
        a[i][j] = 1.0 * i;
    }
  }
  // allocate memory and initialize b
  b = (double**)malloc(n * sizeof(double*));
  for (i = 0; i < n; i++) {
    b[i] = (double*)malloc(n * sizeof(double));
    for (j = 0; j < n; j++) {
      b[i][j] = 1.0 * j;
    }
  }
  // allocate memory for output (c) and initalize it to 0
  c = (double**)malloc(n * sizeof(double*));
  for (i = 0; i < n; i++) {
    c[i] = (double*)malloc(n * sizeof(double));
    for (j = 0; j < n; j++) {
      c[i][j] = 0.0;
    }
  }

  // ------ loop from MIN_SIZE, doubling the size, up to MAX_SIZE -----
  int size;
  for (size = MIN_SIZE; size <= n; size *= 2) {

    // start clock
    clock_gettime(CLOCK_MONOTONIC, &start);

    for(i = 0; i < size; i++){
      for(j = 0; j < size; j++){
        double dot_product = 0.0;
        for(k = 0; k < size; k++){
          dot_product = dot_product + (a[i][k] * b[k][j]);
        }
        c[i][j] = dot_product;
      }
    }
    // stop clock
    clock_gettime(CLOCK_MONOTONIC, &end);

    // calculate time taken for this size
    double sec = end.tv_sec - start.tv_sec;
    double ns = end.tv_nsec - start.tv_nsec;
    double time = (sec) + (ns * (double) pow(10., -9.));

    // record absolute time
    sprintf(time_write_buffer, "%03.9f, %03.9f \n", (double) log_size, time);
    printf(time_write_buffer, "%03.9f, %03.9f \n", (double) log_size, time);
    log_size++;

    // write to file
    fputs(time_write_buffer, fp);

  } // end of size loop

  // for sanity check print 8 by 8 upper left submatrix of the product c
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      printf("%8.2e ", c[i][j]);
    }
    printf("\n");
  }

  fclose(fp);
  //Free 
  for(i = 0; i < n; i++){
    free(a[i]);
    free(b[i]);
    free(c[i]);
  }
  free(a);
  free(b);
  free(c);

  /*
   *  Create one way pipe line with call to popen()
   *  need rbyc.csv file and plot_tile.gp file
   */

  FILE* tp = NULL;
  if ((tp = popen("gnuplot plot_rbyc.gp", "w")) == NULL) {
    perror("popen");
    exit(1);
  }

  // Close the pipe
  pclose(tp);
}