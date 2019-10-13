/*
    File Name:      fwd.c
    Description:    Main C File for fwd
                    > Contains the entry point (main) for the program
    Author:         MIGUEL ARIES SAMBAT TABADERO (22240204)
    Last Modified:  10/10/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>

#include <mpi.h>

#include "fwd.h"

int main(int argc, char** argv) {
    //  Print usage if there are no arguments provided
    if (argc < 2) {
        printUsage();
        return -1;
    }

    char* fileName = parseFileName(argv[1]);
    if (!fileName) {
        return -1;
    }

    int* inputData = fileToPointer(fileName);
    if (!inputData) {
        return -1;
    }

    int numV = inputData[0];
    int* edgeArray = malloc(sizeof(int) * numV * numV);
    if (!edgeArray) {
        fprintf(stderr, "Error: could not allocate memory to edgeArray @ %s\n", __func__);
        return -1;
    }

    memcpy(edgeArray, inputData + 1, sizeof(int) * numV * numV);

    free(inputData);

    int** adjMatrix = convertTo2DMatrix(numV, edgeArray);
    if (!adjMatrix) {
        return -1;
    }

    printf("%d -> %d : %d\n", 0, 0, adjMatrix[0][0]);
    printf("%d -> %d : %d\n", numV - 1, numV - 1, adjMatrix[numV - 1][numV - 1]);
    return 0;
}
