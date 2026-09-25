#include "nn.h"

float *flatten(float ***input, int inputSize, int depth)
{
  // TODO: Implement the flatten function
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

void destroyConvOutput(float ***convOutput, int numFilters, int convOutputSize)
{
    for (int i = 0; i < numFilters; i++)
    {
        for (int j = 0; j < convOutputSize; j++)
        {
            free(convOutput[i][j]);
        }
        free(convOutput[i]);
    }
    free(convOutput);
}

int forwardPass(float ***image, int numChannels, int inputSize, int numFilters, int kernelSize, int fc1OutputSize, int fc2OutputSize, float ****conv1WeightsData, float **fc1WeightsData, float **fc2WeightsData, float *conv1BiasData, float *fc1BiasData, float *fc2BiasData)
{

    // 1. Perform the convolution operation
    int convOutputSize = inputSize - kernelSize + 1;
    float ***convOutput = convolution(image, numChannels, conv1WeightsData, conv1BiasData, numFilters, inputSize, kernelSize);
    if (convOutput == NULL)
    {
        return -1; // Indicate an error
    }

    // 2. Apply ReLU to the convolution output
    for (int f = 0; f < numFilters; f++)
    {
        for (int i = 0; i < convOutputSize; i++)
        {
            applyRelu(convOutput[f][i], convOutputSize);
        }
    }

    // 3. Flatten the output
    float *flattened = flatten(convOutput, convOutputSize, numFilters);
    if (flattened == NULL)
    {
        // Handle memory allocation failure
        destroyConvOutput(convOutput, numFilters, convOutputSize);
        return -1; // Indicate an error
    }

    // 4. Perform the first fully connected operation
    float *fc1Output = linear(flattened, fc1WeightsData, fc1BiasData, convOutputSize * convOutputSize * numFilters, fc1OutputSize);
    if (fc1Output == NULL)
    {
        // Handle memory allocation failure
        free(flattened);
        destroyConvOutput(convOutput, numFilters, convOutputSize);
        return -1; // Indicate an error
    }
    applyRelu(fc1Output, fc1OutputSize);

    // 5. Perform the second fully connected operation
    float *fc2Output = linear(fc1Output, fc2WeightsData, fc2BiasData, fc1OutputSize, fc2OutputSize);
    if (fc2Output == NULL)
    {
        // Handle memory allocation failure
        free(fc1Output);
        free(flattened);
        destroyConvOutput(convOutput, numFilters, convOutputSize);
        return -1; // Indicate an error
    }

    // 6. Apply the final softmax activation
    float *probabilityVector = softmax(fc2Output, fc2OutputSize);
    if (probabilityVector == NULL)
    {
        // Handle memory allocation failure
        free(fc2Output);
        free(fc1Output);
        free(flattened);
        destroyConvOutput(convOutput, numFilters, convOutputSize);
        return -1; // Indicate an error
    }

    // 7. Make predictions
    int predictedClass = predict(probabilityVector, fc2OutputSize);

    // Clean up the memory usage
    free(probabilityVector);
    free(fc2Output);
    free(fc1Output);
    free(flattened);
    destroyConvOutput(convOutput, numFilters, convOutputSize);
    return predictedClass;
}

int predict(float *probabilityVector, int numClasses)
{
    // TODO: Implement the prediction function
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
