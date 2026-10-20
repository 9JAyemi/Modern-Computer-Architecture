#include "unity/unity.h"
#include "../kernel/kernel.h"
#include "test_matrix_ops.h"


#define EPSILON 0.000001f

void assert_float_array_equal_matmul(float **expected, float **actual, int rows, int cols)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(actual, "Function returned NULL (not implemented?)");
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            UNITY_TEST_ASSERT_FLOAT_WITHIN(EPSILON, expected[i][j], actual[i][j], __LINE__, "Arrays Not Equal!");
        }
    }
}

void test_matmul_square_matrices(void)
{
    // Setup
    float **A = (float **)malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        A[i] = (float *)malloc(2 * sizeof(float));
    }
    A[0][0] = 1.0f;
    A[0][1] = 2.0f;
    A[1][0] = 3.0f;
    A[1][1] = 4.0f;

    float **B = (float **)malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        B[i] = (float *)malloc(2 * sizeof(float));
    }
    B[0][0] = 2.0f;
    B[0][1] = 0.0f;
    B[1][0] = 1.0f;
    B[1][1] = 2.0f;

    float **expected = (float **)malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        expected[i] = (float *)malloc(2 * sizeof(float));
    }
    expected[0][0] = 4.0f;
    expected[0][1] = 4.0f;
    expected[1][0] = 10.0f;
    expected[1][1] = 8.0f;

    // Run function under test
    float **C = matmul_blocking(A, B, 2, 2, 2, 2);

    // Check expectations
    assert_float_array_equal_matmul(expected, C, 2, 2);

    // Cleanup
    for (int i = 0; i < 2; i++)
    {
        free(A[i]);
        free(B[i]);
        free(C[i]);
        free(expected[i]);
    }
    free(A);
    free(B);
    free(C);
    free(expected);
}

void test_matmul_incompatible_dimensions(void)
{
    // Setup
    float **A = (float **)malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        A[i] = (float *)malloc(3 * sizeof(float));
    }

    float **B = (float **)malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        B[i] = (float *)malloc(2 * sizeof(float));
    }

    // Run function under test
    float **C = matmul_blocking(A, B, 2, 3, 2, 2);

    // Check expectations
    UNITY_TEST_ASSERT_NULL(C, __LINE__, "Expected NULL!");

    // Cleanup
    for (int i = 0; i < 2; i++)
    {
        free(A[i]);
        free(B[i]);
    }
    free(A);
    free(B);
}

void test_matmul_quant(void)
{
    // Setup
    int num_quant_element_per_word = sizeof(unsigned int) * 8 / QUANT_PRECISION;
    int A_rows = 2, A_cols = 2;
    int B_rows = num_quant_element_per_word * A_cols, B_cols = 1;

    unsigned int **A = (unsigned int **)malloc(A_rows * sizeof(unsigned int *));
    for (int i = 0; i < A_rows; i++)
    {
        A[i] = (unsigned int *)malloc(A_cols * sizeof(unsigned int));
    }
    A[0][0] = 0x01010101;
    A[0][1] = 0x01010101;
    A[1][0] = 0x02020202;
    A[1][1] = 0x02020202;

    int **B = (int **)malloc(B_rows * sizeof(int *));
    for (int i = 0; i < B_rows; i++)
    {
        B[i] = (int *)malloc(B_cols * sizeof(int));
    }
    for (int i = 0; i < B_rows; i++)
    {
        for (int j = 0; j < B_cols; j++)
        {
            B[i][j] = i;
        }
    }

    float **expected = (float **)malloc(A_rows * sizeof(float *));
    for (int i = 0; i < A_rows; i++)
    {
        expected[i] = (float *)malloc(B_cols * sizeof(float));
    }
    expected[0][0] = 28.0f;
    expected[1][0] = 56.0f;

    // Run function under test
    float **C = matmul_quant(A, B, A_rows, A_cols, B_rows, B_cols);

    // Check expectations
    assert_float_array_equal_matmul(expected, C, A_rows, B_cols);

    // Cleanup
    for (int i = 0; i < A_rows; i++)
    {
        free(A[i]);
        free(C[i]);
        free(expected[i]);
    }
    for (int i = 0; i < B_rows; i++)
    {
        free(B[i]);
    }
    free(A);
    free(B);
    free(C);
    free(expected);
}

// Compares matmul_blocking against the reference matmul on a non-square
// problem whose dimensions are not multiples of any tile size, so partial
// tiles at every edge are exercised.
void test_matmul_blocking_matches_naive(void)
{
    int A_rows = 131, A_cols = 77, B_rows = 77, B_cols = 301;

    float **A = (float **)malloc(A_rows * sizeof(float *));
    for (int i = 0; i < A_rows; i++)
    {
        A[i] = (float *)malloc(A_cols * sizeof(float));
        for (int j = 0; j < A_cols; j++)
        {
            A[i][j] = (float)((i * 7 + j * 3) % 11) - 5.0f;
        }
    }
    float **B = (float **)malloc(B_rows * sizeof(float *));
    for (int i = 0; i < B_rows; i++)
    {
        B[i] = (float *)malloc(B_cols * sizeof(float));
        for (int j = 0; j < B_cols; j++)
        {
            B[i][j] = (float)((i * 5 + j) % 13) * 0.25f - 1.5f;
        }
    }

    float **expected = matmul(A, B, A_rows, A_cols, B_rows, B_cols);
    float **C = matmul_blocking(A, B, A_rows, A_cols, B_rows, B_cols);

    // Inputs are small multiples of 0.25, so every partial sum is exact in
    // float and both summation orders give identical results.
    assert_float_array_equal_matmul(expected, C, A_rows, B_cols);

    for (int i = 0; i < A_rows; i++)
    {
        free(A[i]);
        free(C[i]);
        free(expected[i]);
    }
    for (int i = 0; i < B_rows; i++)
    {
        free(B[i]);
    }
    free(A);
    free(B);
    free(C);
    free(expected);
}

// Example from the handout: A[0][0] = 0x01020304 holds [1, 2, 3, 4] (most
// significant byte first), so C[0][0] = 1*a + 2*b + 3*c + 4*d.
void test_matmul_quant_byte_order(void)
{
    int A_rows = 2, A_cols = 1, B_rows = 4, B_cols = 2;

    unsigned int **A = (unsigned int **)malloc(A_rows * sizeof(unsigned int *));
    for (int i = 0; i < A_rows; i++)
    {
        A[i] = (unsigned int *)malloc(A_cols * sizeof(unsigned int));
    }
    A[0][0] = 0x01020304;
    A[1][0] = 0xFF00800A; // [255, 0, 128, 10]

    int B_data[4][2] = {{10, -1}, {100, 2}, {1000, -3}, {10000, 4}};
    int **B = (int **)malloc(B_rows * sizeof(int *));
    for (int i = 0; i < B_rows; i++)
    {
        B[i] = (int *)malloc(B_cols * sizeof(int));
        for (int j = 0; j < B_cols; j++)
        {
            B[i][j] = B_data[i][j];
        }
    }

    float expected_data[2][2] = {
        {1 * 10 + 2 * 100 + 3 * 1000 + 4 * 10000, 1 * -1 + 2 * 2 + 3 * -3 + 4 * 4},
        {255 * 10 + 0 * 100 + 128 * 1000 + 10 * 10000, 255 * -1 + 0 * 2 + 128 * -3 + 10 * 4}};
    float **expected = (float **)malloc(A_rows * sizeof(float *));
    for (int i = 0; i < A_rows; i++)
    {
        expected[i] = (float *)malloc(B_cols * sizeof(float));
        for (int j = 0; j < B_cols; j++)
        {
            expected[i][j] = expected_data[i][j];
        }
    }

    float **C = matmul_quant(A, B, A_rows, A_cols, B_rows, B_cols);
    assert_float_array_equal_matmul(expected, C, A_rows, B_cols);

    for (int i = 0; i < A_rows; i++)
    {
        free(A[i]);
        free(C[i]);
        free(expected[i]);
    }
    for (int i = 0; i < B_rows; i++)
    {
        free(B[i]);
    }
    free(A);
    free(B);
    free(C);
    free(expected);
}
