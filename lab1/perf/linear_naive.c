#include "../kernel/linear.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <inputSize>\n", argv[0]);
        return 1;
    }
    int fcInputSize = atoi(argv[1]);
    int fcOutputSize = fcInputSize; // Example output size

    float *input = (float *)malloc(fcInputSize * sizeof(float));
    for (int i = 0; i < fcInputSize; i++)
    {
        input[i] = 1.0f; // Example initialization
    }

    float **weights = (float **)malloc(fcOutputSize * sizeof(float *));
    for (int i = 0; i < fcOutputSize; i++)
    {
        weights[i] = (float *)malloc(fcInputSize * sizeof(float));
        for (int j = 0; j < fcInputSize; j++)
        {
            weights[i][j] = 1.0f; // Example initialization
        }
    }

    float *biases = (float *)malloc(fcOutputSize * sizeof(float));
    for (int i = 0; i < fcOutputSize; i++)
    {
        biases[i] = 0.0f; // Example initialization
    }

    float *output = linear(input, weights, biases, fcInputSize, fcOutputSize);

    free(output);
    free(biases);
    for (int i = 0; i < fcOutputSize; i++)
    {
        free(weights[i]);
    }
    free(weights);
    free(input);

    return 0;
}
