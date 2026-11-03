#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../kernel/matrix_ops.h"

// Profiling-only variant of matmul_blocking: identical tiling, but the loops
// inside a tile run in i-j-k order (the first version written for this lab).
// The innermost loop walks down a column of B, so each iteration touches a new
// cache line. Used to separate the effect of loop order from tiling.
#ifndef BLOCK_SIZE_I
#define BLOCK_SIZE_I 64
#endif
#ifndef BLOCK_SIZE_J
#define BLOCK_SIZE_J 256
#endif
#ifndef BLOCK_SIZE_K
#define BLOCK_SIZE_K 32
#endif

float **matmul_blocking_ijk(float **A, float **B, int A_rows, int A_cols, int B_rows, int B_cols)
{
    if (A_cols != B_rows) {
        return NULL;
    }
    float **C = (float **)malloc(A_rows * sizeof(float *));
    for (int i = 0; i < A_rows; i++) {
        C[i] = (float *)calloc(B_cols, sizeof(float));
    }
    for (int i0 = 0; i0 < A_rows; i0 += BLOCK_SIZE_I)
        for (int j0 = 0; j0 < B_cols; j0 += BLOCK_SIZE_J)
            for (int k0 = 0; k0 < A_cols; k0 += BLOCK_SIZE_K)
                for (int i = i0; i < i0 + BLOCK_SIZE_I && i < A_rows; i++)
                    for (int j = j0; j < j0 + BLOCK_SIZE_J && j < B_cols; j++) {
                        float sum = 0.0f;
                        for (int k = k0; k < k0 + BLOCK_SIZE_K && k < A_cols; k++)
                            sum += A[i][k] * B[k][j];
                        C[i][j] += sum;
                    }
    return C;
}

float **generate_random_matrix(int rows, int cols) {
    float **matrix = (float **)malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (float *)malloc(cols * sizeof(float));
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = (float)rand() / RAND_MAX;
        }
    }
    return matrix;
}

void free_matrix(float **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Usage: ./matmul_blocking_ijk [size] [reps]
// Multiplies two random size x size matrices reps times so that the runtime
// of the program is dominated by the function under test.
int main(int argc, char *argv[]) {
    srand(time(NULL));
    int size = argc > 1 ? atoi(argv[1]) : 1300;
    int reps = argc > 2 ? atoi(argv[2]) : 5;
    int A_rows = size, A_cols = size, B_rows = size, B_cols = size;

    if (A_cols != B_rows) {
        printf("Matrix dimensions incompatible for multiplication.\n");
        return 1;
    }

    float **A = generate_random_matrix(A_rows, A_cols);
    float **B = generate_random_matrix(B_rows, B_cols);
    float **C;

    printf("Performing ijk-tiled multiplication...\n");
    for (int i=0; i<reps; i++) {
        C = matmul_blocking_ijk(A, B, A_rows, A_cols, B_rows, B_cols);
        if (i < reps - 1) {
            free_matrix(C, A_rows); // only keep the last result
        }
    }

    // Cleanup
    free_matrix(A, A_rows);
    free_matrix(B, B_rows);
    free_matrix(C, A_rows);

    printf("Blocking multiplications completed.\n");

    return 0;
}
