#include "attention.h"
#include <stdio.h>
#include <math.h>

// Scaled dot-product attention
float **scaled_dot_product_attention(float **Q, float **K, float **V, int seqLength, int depth)
{
    float **output = (float **)malloc(seqLength * sizeof(float *));
    for (int i = 0; i < seqLength; i++)
    {
        output[i] = (float *)malloc(depth * sizeof(float));
    }

    float scale = sqrtf((float)depth);
    for (int i = 0; i < seqLength; i++)
    {
        float maxScore = -INFINITY;
        float scores[seqLength];
        for (int k = 0; k < seqLength; k++)
        {
            scores[k] = 0.0f;
            for (int d = 0; d < depth; d++)
            {
                scores[k] += Q[i][d] * K[k][d];
            }
            scores[k] /= scale;
            if (scores[k] > maxScore)
            {
                maxScore = scores[k];
            }
        }

        float weightSum = 0.0f;
        for (int k = 0; k < seqLength; k++)
        {
            scores[k] = expf(scores[k] - maxScore);
            weightSum += scores[k];
        }

        for (int d = 0; d < depth; d++)
        {
            output[i][d] = 0.0f;
            for (int k = 0; k < seqLength; k++)
            {
                output[i][d] += (scores[k] / weightSum) * V[k][d];
            }
        }
    }

    return output;
}