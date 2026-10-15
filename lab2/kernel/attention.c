#include "attention.h"
#include <stdio.h>
#include <math.h>

// Scaled dot-product attention
float **scaled_dot_product_attention(float **Q, float **K, float **V, int seqLength, int depth)
{
    // TODO: Implement the attention algorithm
    // Allocate memory for the output matrix
    float **output = (float **)malloc(seqLength * sizeof(float *));
    for (int i = 0; i < seqLength; i++) {
        output[i] = (float *)malloc(depth * sizeof(float));
    }

    // TODO: Implement the attention computation
    for (int i = 0; i < seqLength; i++) {
        for (int j = 0; j < depth; j++) {
            float sum = 0.0f;
            for (int k = 0; k < seqLength; k++) {
                sum += Q[i][k] * K[k][j];
            }
            output[i][j] = sum / sqrt((float)depth);
        }
    }

    return output;
}