#include "attention.h"
#include <stdio.h>
#include <math.h>

// Scaled dot-product attention: softmax(Q K^T / sqrt(depth)) V
// Q, K, V are seqLength x depth; the output is seqLength x depth.
float **scaled_dot_product_attention(float **Q, float **K, float **V, int seqLength, int depth)
{
    float **output = (float **)malloc(seqLength * sizeof(float *));
    for (int i = 0; i < seqLength; i++)
    {
        output[i] = (float *)malloc(depth * sizeof(float));
    }

    float scale = sqrtf((float)depth);
    // One row of attention scores, reused for every query row. Heap-allocated
    // so long sequences cannot overflow the stack.
    float *scores = (float *)malloc(seqLength * sizeof(float));
    for (int i = 0; i < seqLength; i++)
    {
        // scores[k] = (Q[i] . K[k]) / sqrt(depth), tracking the max for a
        // numerically stable softmax
        float maxScore = -INFINITY;
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

        // Softmax over the scores of row i
        float weightSum = 0.0f;
        for (int k = 0; k < seqLength; k++)
        {
            scores[k] = expf(scores[k] - maxScore);
            weightSum += scores[k];
        }

        // output[i] = sum_k softmax(scores)[k] * V[k]
        for (int d = 0; d < depth; d++)
        {
            output[i][d] = 0.0f;
            for (int k = 0; k < seqLength; k++)
            {
                output[i][d] += (scores[k] / weightSum) * V[k][d];
            }
        }
    }

    free(scores);
    return output;
}