#include "conv.h"

// Basic convolution operation
float ***convolution(float ***image, int numChannels, float ****kernel, float *biasData, int numFilters, int inputSize, int kernelSize)
{
    // TODO: Implement the convolution operation
    int outputSize = inputSize - kernelSize + 1;

    // Allocate memory for the output
    float ***output = (float ***)malloc(numFilters * sizeof(float **));
    for (int f = 0; f < numFilters; f++)
    {
        output[f] = (float **)malloc(outputSize * sizeof(float *));
        for (int i = 0; i < outputSize; i++)
        {
            output[f][i] = (float *)malloc(outputSize * sizeof(float));
        }
    }

    // Perform the convolution operation
    for (int f = 0; f < numFilters; f++)
    {
        for (int i = 0; i < outputSize; i++)
        {
            for (int j = 0; j < outputSize; j++)
            {
                output[f][i][j] = biasData[f];
                for (int c = 0; c < numChannels; c++)
                {
                    for (int ki = 0; ki < kernelSize; ki++)
                    {
                        for (int kj = 0; kj < kernelSize; kj++)
                        {
                            output[f][i][j] += image[c][i + ki][j + kj] * kernel[f][c][ki][kj];
                        }
                    }
                }
            }
        }
    }

    return output;
}
