#include "linear.h"

float *linear(float *input, float **weights, float *biases, int inputSize, int outputSize)
{
    // Empty or missing input/weights: no valid output
    if (input == NULL || weights == NULL || biases == NULL || inputSize <= 0 || outputSize <= 0)
    {
        return NULL;
    }

    float *output = (float *)malloc(outputSize * sizeof(float));
    for (int i = 0; i < outputSize; i++)
    {
        output[i] = biases[i];
        for (int j = 0; j < inputSize; j++)
        {
            output[i] += input[j] * weights[i][j];
        }
    }
    return output;
}
