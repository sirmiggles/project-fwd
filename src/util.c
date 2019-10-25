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

int** allocContiguousMatrix(int rows, int cols) {
    int* contig = (int*) malloc(sizeof(int) * rows * cols);
    if (!contig) {
        return NULL;
    }
    int** outMatrix = (int**) malloc(rows * sizeof(int*));
    if (!outMatrix) {
        return NULL;
    }
    for (int i = 0; i < cols; i++) {
        outMatrix[i] = &contig[cols * i];
    }

    return outMatrix;
}
