#include "matrix_ops.h"
#include <stdint.h>

// Tile sizes for matmul_blocking. They can be overridden at compile time
// (e.g. -DBLOCK_SIZE_I=64) so different tilings can be profiled without
// editing this file. The defaults were the fastest configuration measured on
// the ece5755 node (Xeon Silver 4514Y: 48 KB L1d, 2 MB L2 per core).
#ifndef BLOCK_SIZE_I
#define BLOCK_SIZE_I 64
#endif

#ifndef BLOCK_SIZE_J
#define BLOCK_SIZE_J 256
#endif

#ifndef BLOCK_SIZE_K
#define BLOCK_SIZE_K 32
#endif

static inline int min_int(int a, int b)
{
    return a < b ? a : b;
}

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
//
// C = A * B computed tile by tile. The three outer loops walk over tiles of
// size BLOCK_SIZE_I x BLOCK_SIZE_K (A), BLOCK_SIZE_K x BLOCK_SIZE_J (B) and
// BLOCK_SIZE_I x BLOCK_SIZE_J (C). Inside a tile the loops run in i-k-j order:
//   - A[i][k] is loaded once and kept in a register,
//   - B[k][j..] and C[i][j..] are read/written with unit stride, so every
//     cache line brought in is fully used.
// The B tile (BLOCK_SIZE_K x BLOCK_SIZE_J floats) is reused by all
// BLOCK_SIZE_I rows of the A tile while it is still resident in L1/L2,
// instead of being streamed from memory once per output row as in matmul().
float **matmul_blocking(float **A, float **B, int A_rows, int A_cols, int B_rows, int B_cols)
{
    if (A_cols != B_rows) {
        return NULL; // Incompatible matrix dimensions
    }

    // Allocate the result matrix and zero it: tiles along k accumulate into C
    float **C = (float **)malloc(A_rows * sizeof(float *));
    for (int i = 0; i < A_rows; i++) {
        C[i] = (float *)calloc(B_cols, sizeof(float));
    }

    for (int i0 = 0; i0 < A_rows; i0 += BLOCK_SIZE_I) {
        int i_end = min_int(i0 + BLOCK_SIZE_I, A_rows);

        for (int k0 = 0; k0 < A_cols; k0 += BLOCK_SIZE_K) {
            int k_end = min_int(k0 + BLOCK_SIZE_K, A_cols);

            for (int j0 = 0; j0 < B_cols; j0 += BLOCK_SIZE_J) {
                int j_end = min_int(j0 + BLOCK_SIZE_J, B_cols);

                // Multiply tile A[i0:i_end][k0:k_end] by tile
                // B[k0:k_end][j0:j_end] and accumulate into C[i0:i_end][j0:j_end]
                for (int i = i0; i < i_end; i++) {
                    float *C_row = C[i];
                    for (int k = k0; k < k_end; k++) {
                        float a = A[i][k];
                        float *B_row = B[k];
                        for (int j = j0; j < j_end; j++) {
                            C_row[j] += a * B_row[j];
                        }
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

    // Each word A[i][w] packs elements_per_uint quantized values, most
    // significant byte first: A[i][0] = 0x01020304 holds [1, 2, 3, 4], which
    // multiply B rows 0, 1, 2, 3. Value e of a word is therefore at bit offset
    // (elements_per_uint - 1 - e) * QUANT_PRECISION and is extracted with a
    // shift followed by a mask.
    for (int i = 0; i < A_rows; ++i)
    {
        for (int j = 0; j < B_cols; ++j)
        {
            int64_t sum = 0; // integer accumulation, converted to float once
            for (int word = 0; word < A_cols; ++word)
            {
                unsigned int packed = A[i][word];
                for (int element = 0; element < elements_per_uint; ++element)
                {
                    int k = word * elements_per_uint + element;
                    unsigned int shift = (unsigned int)((elements_per_uint - 1 - element) * QUANT_PRECISION);
                    unsigned int quantized_value = (packed >> shift) & mask;
                    sum += (int64_t)quantized_value * (int64_t)B[k][j];
                }
            }
            C[i][j] = (float)sum;
        }
    }

    return C;
}