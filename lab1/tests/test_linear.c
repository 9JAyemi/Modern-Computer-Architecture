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

// Add more test cases as needed
void test_linear_with_zero_bias(void)
{
    float input[] = {1.0, 2.0, 3.0};
    float *weights[] = {(float[]){1.0, 2.0, 3.0}, (float[]){4.0, 5.0, 6.0}};
    float biases[] = {0.0, 0.0};
    float *output = linear(input, weights, biases, 3, 2);
    TEST_ASSERT_EQUAL_FLOAT(14.0, output[0]);
    TEST_ASSERT_EQUAL_FLOAT(32.0, output[1]);

    // Cleanup
    free(output);
}
void test_linear_with_negative_weights(void)
{
    float input[] = {1.0, 2.0, 3.0};
    float *weights[] = {(float[]){-1.0, -2.0, -3.0}, (float[]){-4.0, -5.0, -6.0}};
    float biases[] = {0.1, 0.2};
    float *output = linear(input, weights, biases, 3, 2);
    TEST_ASSERT_EQUAL_FLOAT(-13.9, output[0]);
    TEST_ASSERT_EQUAL_FLOAT(-31.8, output[1]);

    // Cleanup
    free(output);
}
void test_linear_with_negative_bias(void)
{
    float input[] = {1.0, 2.0, 3.0};
    float *weights[] = {(float[]){1.0, 2.0, 3.0}, (float[]){4.0, 5.0, 6.0}};
    float biases[] = {-0.1, -0.2};
    float *output = linear(input, weights, biases, 3, 2);
    TEST_ASSERT_EQUAL_FLOAT(14.0 - 0.1, output[0]);
    TEST_ASSERT_EQUAL_FLOAT(32.0 - 0.2, output[1]);

    // Cleanup
    free(output);
}
void test_linear_with_all_zero_weights_and_bias(void)
{
    float input[] = {1.0, 2.0, 3.0};
    float *weights[] = {(float[]){0.0, 0.0, 0.0}, (float[]){0.0, 0.0, 0.0}};
    float biases[] = {0.0, 0.0};
    float *output = linear(input, weights, biases, 3, 2);
    TEST_ASSERT_EQUAL_FLOAT(0.0, output[0]);
    TEST_ASSERT_EQUAL_FLOAT(0.0, output[1]);

    // Cleanup
    free(output);
}
void test_linear_with_large_input(void)
{
    float input[] = {1000.0, 2000.0, 3000.0};
    float *weights[] = {(float[]){1.0, 2.0, 3.0}, (float[]){4.0, 5.0, 6.0}};
    float biases[] = {0.1, 0.2};
    float *output = linear(input, weights, biases, 3, 2);
    TEST_ASSERT_EQUAL_FLOAT(14000.1, output[0]);
    TEST_ASSERT_EQUAL_FLOAT(32000.2, output[1]);

    // Cleanup
    free(output);
}
void test_linear_with_large_weights(void)
{
    float input[] = {1.0, 2.0, 3.0};
    float *weights[] = {(float[]){1000.0, 2000.0, 3000.0}, (float[]){4000.0, 5000.0, 6000.0}};
    float biases[] = {0.1, 0.2};
    float *output = linear(input, weights, biases, 3, 2);
    TEST_ASSERT_EQUAL_FLOAT(14000.1, output[0]);
    TEST_ASSERT_EQUAL_FLOAT(32000.2, output[1]);

    // Cleanup
    free(output);
}
void test_linear_with_large_bias(void)
{
    float input[] = {1.0, 2.0, 3.0};
    float *weights[] = {(float[]){1.0, 2.0, 3.0}, (float[]){4.0, 5.0, 6.0}};
    float biases[] = {1000.0, 2000.0};
    float *output = linear(input, weights, biases, 3, 2);
    TEST_ASSERT_EQUAL_FLOAT(14.0 + 1000.0, output[0]);
    TEST_ASSERT_EQUAL_FLOAT(32.0 + 2000.0, output[1]);

    // Cleanup
    free(output);
}
void test_linear_with_negative_input(void)
{
    float input[] = {-1.0, -2.0, -3.0};
    float *weights[] = {(float[]){1.0, 2.0, 3.0}, (float[]){4.0, 5.0, 6.0}};
    float biases[] = {0.1, 0.2};
    float *output = linear(input, weights, biases, 3, 2);
    TEST_ASSERT_EQUAL_FLOAT(-14.0 + 0.1, output[0]);
    TEST_ASSERT_EQUAL_FLOAT(-32.0 + 0.2, output[1]);

    // Cleanup
    free(output);
}

void test_linear_with_negative_weights_and_bias(void)
{
    float input[] = {1.0, 2.0, 3.0};
    float *weights[] = {(float[]){-1.0, -2.0, -3.0}, (float[]){-4.0, -5.0, -6.0}};
    float biases[] = {-0.1, -0.2};
    float *output = linear(input, weights, biases, 3, 2);
    TEST_ASSERT_EQUAL_FLOAT(-14.0 - 0.1, output[0]);
    TEST_ASSERT_EQUAL_FLOAT(-32.0 - 0.2, output[1]);

    // Cleanup
    free(output);
}
