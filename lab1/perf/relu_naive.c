#include "kernel/functional.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <inputSize>\n", argv[0]);
        return 1;
    }
    int inputSize = atoi(argv[1]);

    float *input = (float *)malloc(inputSize * sizeof(float));
    for (int i = 0; i < inputSize; i++)
    {
        input[i] = (i % 2 == 0) ? (float)i : -(float)i; // Example initialization
    }

    applyRelu(input, inputSize);

    free(input);

    return 0;
}
