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
    int** adjMatrix = convertTo2DMatrix(inputData);
    if (!adjMatrix) {
        return -1;
    }

    printf("N(V) = %d\n", numV);

    for (int i = 0; i < numV; i++) {
        for (int j = 0; j < numV; j++) {
            printf("%d -> %d : %d\n", i, j, adjMatrix[i][j]);
        }
    }
    return 0;
}
