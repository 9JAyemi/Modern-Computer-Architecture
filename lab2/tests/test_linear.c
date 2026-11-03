#include "unity/unity.h"
#include "../kernel/kernel.h"
#include "test_linear.h"


void test_linear_basic(void)
{
    float input[] = {1.0, 2.0, 3.0};
    float *weights[] = {(float[]){1.0, 2.0, 3.0}, (float[]){4.0, 5.0, 6.0}};
    float biases[] = {0.1, 0.2};
    float *output = linear(input, weights, biases, 3, 2);
    TEST_ASSERT_EQUAL_FLOAT(14.1, output[0]);
    TEST_ASSERT_EQUAL_FLOAT(32.2, output[1]);

    // Cleanup
    free(output);
}

void test_linear_basic2(void)
{
    float input[] = {1.0, 2.0, 3.0};
    float *weights[] = {(float[]){1.0, 2.0, 3.0}, (float[]){4.0, 5.0, 6.0}};
    float biases[] = {0.1, 0.2};
    float *output = linear(input, weights, biases, 3, 2);
    TEST_ASSERT_EQUAL_FLOAT(14.1, output[0]);
    TEST_ASSERT_EQUAL_FLOAT(32.2, output[1]);

    // Cleanup
    free(output);
}

// Empty input (size 0 or NULL pointers) has no valid output
void test_linear_empty_input(void)
{
    float input[] = {1.0};
    float *weights[] = {(float[]){1.0}};
    float biases[] = {0.0};
    TEST_ASSERT_NULL(linear(input, weights, biases, 0, 1));
    TEST_ASSERT_NULL(linear(input, weights, biases, 1, 0));
    TEST_ASSERT_NULL(linear(NULL, NULL, NULL, 0, 0));
}

// Add more test cases as needed