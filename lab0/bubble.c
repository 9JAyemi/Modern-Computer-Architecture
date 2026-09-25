#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int *bubble(int *input, int n) {
    bool swapped;

    for (int i = 0; i < n - 1; i++) {
        swapped = false;

        for (int j = 0; j < n - i - 1; j++) {
            if (input[j] > input[j + 1]) {
                int temp = input[j];
                input[j] = input[j + 1];
                input[j + 1] = temp;
                swapped = true;
            }
        }

        if (!swapped)
            break;
    }

    return input;
}

// verify() checks that the array is sorted
// DO NOT call verify() when profiling your code 
void verify(int *input, int n) {
    for (int i = 1; i < n; i++) {
        assert(input[i] >= input[i-1]);
    }
    printf("array is correctly sorted \n");
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: ./bubble <input_size> <input_file>\n");
        return 1;
    }
    int n = atoi(argv[1]);
    int *input = (int *) malloc(sizeof(int)*n);
    int *output; // you will need to allocate memory for output if not sorting in place
    FILE *fp = fopen(argv[2], "r");
    
    for (int i = 0; i < n; i++) {
        fscanf(fp, "%d,", &input[i] );
    }

    output = bubble(input, n);

    // verify(output, n ); // keep this line commented out when profiling

    free(input);
    return 0;
}