/*
    File Name:      util.c
    Description:    Utility functions for fwd
    Author:         MIGUEL ARIES SAMBAT TABADERO (22240204)
    Last Modified:  10/10/2019
*/

#include <stdio.h>
#include <stdlib.h>

#include "fwd.h"

/*  Print the usage message for executing the program  */
void printUsage(void) {
    fprintf(stderr, "Usage: ./fwd <file_1>\n");
}

int** allocContiguousMatrix(int numV) {
    int* contig = (int*) malloc(sizeof(int) * numV * numV);
    int** outMatrix = (int**) malloc(numV * sizeof(int*));
    for (int i = 0; i < numV; i++) {
        outMatrix[i] = &contig[numV * i];
    }

    return outMatrix;
}
