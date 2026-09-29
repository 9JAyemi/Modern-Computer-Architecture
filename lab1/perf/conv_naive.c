#include "../kernel/conv.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <inputSize>\n", argv[0]);
        return 1;
    }
    int inputSize = atoi(argv[1]);
    int kernelSize = 3; // Example kernel size
    int numChannels = 3; // Example number of channels
    int numFilters = 2;  // Example number of filters
    if (inputSize < kernelSize)
    {
        printf("inputSize must be >= kernelSize (%d)\n", kernelSize);
        return 1;
    }
    float ***image = (float ***)malloc(numChannels * sizeof(float **));
    for (int c = 0; c < numChannels; c++)
    {
        image[c] = (float **)malloc(inputSize * sizeof(float *));
        for (int i = 0; i < inputSize; i++)
        {
            image[c][i] = (float *)malloc(inputSize * sizeof(float));
            for (int j = 0; j < inputSize; j++)
            {
                image[c][i][j] = 1.0f; // Example initialization
            }
        }
    }

    float ****kernel = (float ****)malloc(numFilters * sizeof(float ***));
    for (int f = 0; f < numFilters; f++)
    {
        kernel[f] = (float ***)malloc(numChannels * sizeof(float **));
        for (int c = 0; c < numChannels; c++)
        {
            kernel[f][c] = (float **)malloc(kernelSize * sizeof(float *));
            for (int ki = 0; ki < kernelSize; ki++)
            {
                kernel[f][c][ki] = (float *)malloc(kernelSize * sizeof(float));
                for (int kj = 0; kj < kernelSize; kj++)
                {
                    kernel[f][c][ki][kj] = 1.0f; // Example initialization
                }
            }
        }
    }

    float *biasData = (float *)malloc(numFilters * sizeof(float));
    for (int f = 0; f < numFilters; f++)
    {
        biasData[f] = 0.0f; // Example initialization
    }

    float ***output = convolution(image, numChannels, kernel, biasData, numFilters, inputSize, kernelSize);

    // Free allocated memory
    for (int c = 0; c < numChannels; c++)
    {
        for (int i = 0; i < inputSize; i++)
        {
            free(image[c][i]);
        }
        free(image[c]);
    }
    free(image);

    for (int f = 0; f < numFilters; f++)
    {
        for (int c = 0; c < numChannels; c++)
        {
            for (int ki = 0; ki < kernelSize; ki++)
            {
                free(kernel[f][c][ki]);
            }
            free(kernel[f][c]);
        }
        free(kernel[f]);
    }
    free(kernel);

    free(biasData);

    for (int f = 0; f < numFilters; f++)
    {
        for (int i = 0; i < inputSize - kernelSize + 1; i++)
        {
            free(output[f][i]);
        }
        free(output[f]);
    }
    free(output);

    return 0;
}
