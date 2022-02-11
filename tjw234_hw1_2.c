/** @author Tyrone Whitmore-Wilson (tjw234)
 * @file tjw234_hw1_2.c
 * @brief Tile based matrix multiplication
 * @version 0.1
 * @date 2022-02-09
 *
 * @copyright Copyright (c) 2022
 *
 */
/*
 * Blocked matrix multiplication, c = a*b
 *    subblocks of a, b and c are cached
 * There are two outer loops, one over size of matrices, the other
 * over size of blocks.
 * Outer loop increases matrix dimension n from MIN_SIZE doubling
 * on each pass until MAX_SIZE is reached
 * Inner loop increases block size b_s from MIN_BLOCK doubling
 * on each pass untill MAX_BLOCK is reached
 * It is assumed that n is divisible by b_s
 *
 * NOTE: for MAX_SIZE = 2^13 it takes many minutes to finish
 *
 * Timing results are stored in file tile.csv as a 2D array where
 * rows correspond to growing sizes of blocks, and columns correspond
 * to growing dimensions of the matrices plus one. Make the first column
 * the column of indices against which other columns are plotted
 *
 * compile: gcc mm_tile.c -o mm_tile
 * run code:   ./mm_tile
 *
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// for benchmarking various sizes of matrices and blocks
// set sizes here, otherwise read from the command line
#define MIN_SIZE 1 << 6
#define MAX_SIZE 1 << 11
#define MIN_BLOCK 1 << 3
#define MAX_BLOCK 1 << 6
#define BILLION 1000000000L

int main(int argc, char *argv[]) {
    char time_write_buffer[32];
    // loop and other indices
    int n = MAX_SIZE;
    int i, j, k, dim_b, ib, jb, kb;
    int arr_size;
    int block_size;
    // variables for timings
    struct timespec start, end;
    float sec, nsec, exec_time;
    float log_size = 6;

    // open file for recording time measurements
    FILE *fp = NULL;
    fp = fopen("tile.csv", "w");

    // define subblocks and allocate memory
    float *tile_a, *tile_b, *tile_c;

    // define matrices
    float **a, **b, **c;                 // Matricies as pointer arrays
    float *a_array, *b_array, *c_array;  // Matricies as 1 d array

    // allocate space for c, use malloc or  calloc

    // a and b are set in such a way that it easy to check
    // for corretness of the product see
    // once verified that the code is correct, initialize as needed

    // allocate memory and initialize a
    a = (float **)malloc(n * sizeof(float *));
    for (i = 0; i < n; i++) {
        a[i] = (float *)malloc(n * sizeof(float));
        for (j = 0; j < n; j++) {
            a[i][j] = 1.0 * i;
        }
    }
    // allocate memory and initialize b
    b = (float **)malloc(n * sizeof(float *));
    for (i = 0; i < n; i++) {
        b[i] = (float *)malloc(n * sizeof(float));
        for (j = 0; j < n; j++) {
            b[i][j] = 1.0 * j;
        }
    }
    // allocate memory for output (c) and initalize it to 0
    c = (float **)malloc(n * sizeof(float *));
    for (i = 0; i < n; i++) {
        c[i] = (float *)malloc(n * sizeof(float));
        for (j = 0; j < n; j++) {
            c[i][j] = 0.0;
        }
    }



    // ------- loop from MIN_SIZE, doubling the size, up to MAX_SIZE
    for (arr_size = MIN_SIZE; arr_size <= MAX_SIZE; arr_size = arr_size * 2) {
        sprintf(time_write_buffer, "%03.9f", (float)log_size);
        fputs(time_write_buffer, fp);
        // ------- loop from MIN_BLOCK, doubling the size, up to MAX_BLOCK
        for (block_size = MIN_BLOCK; block_size <= MAX_BLOCK;
             block_size = block_size * 2) {
            // Clear C before calculation
            for (i = 0; i < n; i++) {
                for (j = 0; j < n; j++) {
                    c[i][j] = 0.0;
                }
            }
            // start the clock
            clock_gettime(CLOCK_MONOTONIC, &start);
            // Multiply a * b = c

            //The first two for loops choose some tile C(ib, jb) to evaluate
            //ib, jb and kb use absolute posistion instead of changing the coordinate system 
            for(ib = 0; ib < arr_size; ib += block_size){
                for(jb = 0; jb < arr_size; jb += block_size){
                    //Then select the corresponding row and column of tiles to calculate the dot product of
                    //A(ib, kb)*B(ib,kb)
                    for(kb = 0; kb <arr_size; kb += block_size){
                        //Standard dot product inside the tile. 
                        //A(ib,kb)(i, k)
                        for(i = ib; i < ib + block_size; i++){
                            for(j = jb; j < jb + block_size; j++){
                                float parital_dot_product = 0.0;
                                //calculate the dot product inside the tile
                                for(k = kb; k < kb + block_size; k++){
                                    parital_dot_product += a[i][k] * b[k][j];
                                }
                                //Add the partial dot product to the parital sum currently in c[i][j]
                                c[i][j] += parital_dot_product;
                            }
                        }
                        // printf("Submatrix A(%d, %d)*B(%d, %d) finished\n", ib, kb, kb, jb);
                    }
                }
            }
            // Stop the clock
            clock_gettime(CLOCK_MONOTONIC, &end);
            // calculate time taken for this size
            float sec = end.tv_sec - start.tv_sec;
            float ns = end.tv_nsec - start.tv_nsec;
            float time = (sec) + (ns * (float)pow(10., -9.));
            //normalized to n^3
            double normalizer =(double)  arr_size *(double)  arr_size *(double)  arr_size;
            double normalize = time / normalizer * (double) pow(10., 9.);

            // record normalized time
            sprintf(time_write_buffer, ", %03.9f", normalize);
            // write to file
            fputs(time_write_buffer, fp);

        }
        log_size++; //Update size for printing purposes
        fputs("\n", fp);
    }   // end of matrix size loop
    // close the file and free memory
    fclose(fp);
    // Free
    for (i = 0; i < n; i++) {
        free(a[i]);
        free(b[i]);
        free(c[i]);
    }
    free(a);
    free(b);
    free(c);
    /*
     *  Create one way pipe line with call to popen()
     *  need tile.csv file and plot_tile.gp file
     */

    FILE *tp = NULL;
    if ((tp = popen("gnuplot plot_tile.gp", "w")) == NULL) {
        perror("popen");
        exit(1);
    }
    // Close the pipe
    pclose(tp);


    return 0;
}
