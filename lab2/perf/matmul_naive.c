#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../kernel/matrix_ops.h"

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

// Usage: ./matmul_naive [size] [reps]
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

    printf("Performing naive multiplication...\n");
    for (int i=0; i<reps; i++) {
        C = matmul(A, B, A_rows, A_cols, B_rows, B_cols);
        if (i < reps - 1) {
            free_matrix(C, A_rows); // only keep the last result
        }
    }

    // Cleanup
    free_matrix(A, A_rows);
    free_matrix(B, B_rows);
    free_matrix(C, A_rows);

    printf("Naive multiplications completed.\n");

    return 0;
}
