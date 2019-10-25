/*
    File Name:      util.c
    Description:    Utility functions for fwd
    Author:         MIGUEL ARIES SAMBAT TABADERO (22240204)
    Last Modified:  25/10/2019
*/

#include <stdio.h>
#include <stdlib.h>

#include "fwd.h"

/*  Print the usage message for executing the program  */
void printUsage(void) {
    fprintf(stderr, "Usage: ./fwd <file_1>\n");
}

/*  Print the results of execution  */
void printOutput(int numV, int* apsp) {
    printf("Number of vertices: %d\n", numV);
    for (int i = 0; i < numV * numV; i++) {
        printf("%d ", apsp[i]);
    }
    printf("\n");
}