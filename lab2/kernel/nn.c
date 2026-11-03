#include "nn.h"

float *flatten(float ***input, int inputSize, int depth)
{
    int flattenedSize = inputSize * inputSize * depth;
    float *flattened = (float *)malloc(flattenedSize * sizeof(float));
    int index = 0;
    for (int d = 0; d < depth; d++)
    {
        for (int i = 0; i < inputSize; i++)
        {
            for (int j = 0; j < inputSize; j++)
            {
                flattened[index++] = input[d][i][j];
            }
        }
    }
    return flattened;
}

void destroyConvOutput(float ***convOutput, int convOutputSize)
{
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < convOutputSize; j++)
        {
            free(convOutput[i][j]);
        }
        free(convOutput[i]);
    }
    free(convOutput);
}

int forwardPass(float ***image, int numChannels, float ****conv1WeightsData, float **fc1WeightsData, float **fc2WeightsData, float *conv1BiasData, float *fc1BiasData, float *fc2BiasData)
{
    // 1. Perform the convolution operation

    // 2. Flatten the output

    // 3. Perform the fully connected operations

    // 4. Apply the final softmax activation

    // 5. Make predictions

    // Clean up the memory usage
}

int predict(float *probabilityVector, int numClasses)
{
    int predictedClass = 0;
    float maxProbability = probabilityVector[0];
    for (int i = 1; i < numClasses; i++)
    {
        if (probabilityVector[i] > maxProbability)
        {
            maxProbability = probabilityVector[i];
            predictedClass = i;
        }
    }
    return predictedClass;
}
