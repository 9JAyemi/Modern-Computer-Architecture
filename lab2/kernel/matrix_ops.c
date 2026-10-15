#include "matrix_ops.h"
#include <stdint.h>

#ifndef BLOCK_SIZE_I
#define BLOCK_SIZE_I 169
#endif

#ifndef BLOCK_SIZE_J
#define BLOCK_SIZE_J 32
#endif

#ifndef BLOCK_SIZE_K
#define BLOCK_SIZE_K 9
#endif


float **matmul(float **A, float **B, int A_rows, int A_cols, int B_rows, int B_cols) 
{
    // TODO: Implement matrix multiplication
    if (A_cols != B_rows) {
        return NULL; // Incompatible matrix dimensions
    }

    // Allocate memory for the result matrix
    float **C = (float **)malloc(A_rows * sizeof(float *));
    for (int i = 0; i < A_rows; i++) {
        C[i] = (float *)malloc(B_cols * sizeof(float));
    }

    // Perform matrix multiplication
    for (int i = 0; i < A_rows; i++) {
        for (int j = 0; j < B_cols; j++) {
            C[i][j] = 0.0f;
            for (int k = 0; k < A_cols; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return C;
}

// Matmul with blocking optimization
float **matmul_blocking(float **A, float **B, int A_rows, int A_cols, int B_rows, int B_cols)
{
    // TODO: Implement matrix multiplication with blocking (loop tiling)
    if (A_cols != B_rows) {
        return NULL; // Incompatible matrix dimensions
    }

    // Allocate memory for the result matrix
    float **C = (float **)malloc(A_rows * sizeof(float *));
    for (int i = 0; i < A_rows; i++) {
        C[i] = (float *)malloc(B_cols * sizeof(float));
        for (int j = 0; j < B_cols; j++) {
            C[i][j] = 0.0f;
        }
    }

    for (int i = 0; i < A_rows; i += BLOCK_SIZE_I)
    {
        for (int j = 0; j < B_cols; j += BLOCK_SIZE_J)
        {
            for (int k = 0; k < A_cols; k += BLOCK_SIZE_K)
            {
                // TODO: Implement the block multiplication
                for (int ii = i; ii < i + BLOCK_SIZE_I && ii < A_rows; ++ii)
                {
                    for (int jj = j; jj < j + BLOCK_SIZE_J && jj < B_cols; ++jj)
                    {
                        float sum = 0.0f;
                        for (int kk = k; kk < k + BLOCK_SIZE_K && kk < A_cols; ++kk)
                        {
                            sum += A[ii][kk] * B[kk][jj];
                        }
                        // Accumulate the result into the output matrix
                        // Assuming C is the output matrix, you need to allocate it before this loop
                        C[ii][jj] += sum;
                    }
                }
            }
        }
    }
    return C;
}

// Matmul with quantization
float **matmul_quant(unsigned int **A, int **B, int A_rows, int A_cols, int B_rows, int B_cols)
{
    if ((QUANT_PRECISION % 2) != 0 || QUANT_PRECISION <= 0 || QUANT_PRECISION > 16)
    {
        printf("Incompatible precision assigned to the matrix.\n");
        return NULL;
    }

    if (A_rows <= 0 || A_cols <= 0 || B_rows <= 0 || B_cols <= 0 || A == NULL || B == NULL)
    {
        printf("Invalid matrix dimensions or input.\n");
        return NULL;
    }

    int elements_per_uint = (sizeof(unsigned int) * 8) / QUANT_PRECISION;
    if (elements_per_uint <= 0 || B_rows % elements_per_uint != 0 || B_rows / elements_per_uint != A_cols)
    {
        printf("Matrix dimensions incompatible for multiplication.\n");
        return NULL;
    }

    const unsigned int mask = (1u << QUANT_PRECISION) - 1u;
    float **C = (float **)malloc(A_rows * sizeof(float *));
    if (C == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < A_rows; ++i)
    {
        if (A[i] == NULL)
        {
            for (int row = 0; row < i; ++row)
            {
                free(C[row]);
            }
            free(C);
            return NULL;
        }

        C[i] = (float *)malloc(B_cols * sizeof(float));
        if (C[i] == NULL)
        {
            for (int row = 0; row < i; ++row)
            {
                free(C[row]);
            }
            free(C);
            return NULL;
        }
    }

    for (int row = 0; row < B_rows; ++row)
    {
        if (B[row] == NULL)
        {
            for (int output_row = 0; output_row < A_rows; ++output_row)
            {
                free(C[output_row]);
            }
            free(C);
            return NULL;
        }
    }

    for (int i = 0; i < A_rows; ++i)
    {
        for (int j = 0; j < B_cols; ++j)
        {
            int64_t sum = 0;
            for (int word = 0; word < A_cols; ++word)
            {
                for (int element = 0; element < elements_per_uint; ++element)
                {
                    int k = word * elements_per_uint + element;
                    unsigned int shift = (unsigned int)(element * QUANT_PRECISION);
                    unsigned int quantized_value = (A[i][word] >> shift) & mask;
                    sum += (int64_t)quantized_value * (int64_t)B[k][j];
                }
            }
            C[i][j] = (float)sum;
        }
    }

    return C;
}