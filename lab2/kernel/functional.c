#include "functional.h"

float relu(float x)
{
    return x > 0 ? x : 0;
}

void applyRelu(float *input, int inputSize)
{
    for (int i = 0; i < inputSize; i++)
    {
        input[i] = relu(input[i]);
    }
}

float *softmax(float *input, int inputSize)
{
    float maxInput = input[0];
    for (int i = 1; i < inputSize; i++)
    {
        if (input[i] > maxInput)
        {
            maxInput = input[i];
        }
    }

    float *expValues = (float *)malloc(inputSize * sizeof(float));
    float sumExp = 0.0f;
    for (int i = 0; i < inputSize; i++)
    {
        expValues[i] = exp(input[i] - maxInput);
        sumExp += expValues[i];
    }

    float *output = (float *)malloc(inputSize * sizeof(float));
    for (int i = 0; i < inputSize; i++)
    {
        output[i] = logf(expValues[i] / sumExp);
    }
    free(expValues);
    return output;
}