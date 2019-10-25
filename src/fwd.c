/*
    File Name:      fwd.c
    Description:    Main C File for fwd
                    > Contains the entry point (main) for the program
    Author:         MIGUEL ARIES SAMBAT TABADERO (22240204)
    Last Modified:  25/10/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#include <mpi.h>

#include "fwd.h"

int main(int argc, char** argv) {
    //  Initialize MPI environment
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &clusterSize);

    char* sourceFileName;
    int* edgeArray;
    int* vertexSet;
    int  numV = -1;
    struct timeval start, end;

    //  Head node file I/O
    if (rank == 0) {
        if (argc < 2) {
            printUsage();
            return -1;
        }

        char* fileName = parseFileName(argv[1]);
        if (!fileName) {
            MPI_Finalize();
            return -1;
        }

        sourceFileName = (char*) malloc(sizeof(char) * FILEPATH_MAX);
        if (!sourceFileName) {
            fprintf(stderr, "Error: could not allocate memory to sourceFileName @ %s\n", __func__);
            MPI_Finalize();
            return -1;
        }

        //  Copy argument into sourceFileName for log output
        strcpy(sourceFileName, fileName);

        int* inputData = fileToPointer(fileName);
        if (!inputData) {
            MPI_Finalize();
            return -1;
        }
       
        numV = inputData[0];
        if (numV < 1) {
            fprintf(stderr, "Error: insufficient number of vertices, numV. numV must be greater than 0.\n");
            MPI_Finalize();
            return -1;
        }

        gettimeofday(&start, NULL);

        //  Create vertex set for work allocation to processors
        vertexSet = (int *) malloc(sizeof(int) * numV);
        if (!vertexSet) {
            fprintf(stderr, "Error: could not allocate memory to vertexSet @ %s\n", __func__);
            MPI_Finalize();
            return -1;
        }
        for (int i = 0; i < numV; i++) {
            vertexSet[i] = i;
        }

        edgeArray = (int *) malloc(sizeof(int) * numV * numV);
        if (!edgeArray) {
            fprintf(stderr, "Error: could not allocate memory to edgeArray @ %s\n", __func__);
            MPI_Finalize();
            return -1;
        }
        memcpy(edgeArray, inputData + 1, sizeof(int) * numV * numV);
        free(inputData);
    }

    //  Broadcast the number of vertices to all nodes, and wait for all of them to receive
    MPI_Bcast(&numV, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //  Initialize the target matrices that this matrix will complete
    int numTargets = 0;
    int* targets = allocateTargets(numV, vertexSet, &numTargets);
    if (!targets) {
        MPI_Finalize();
        return -1;
    }

    //  Initialize the distances
    int* distances = (int *) malloc(sizeof(int) * numV * numV);
    if(!(distances = initAllDistances(numV, edgeArray))) {
        MPI_Finalize();
        return -1;
    }

    //  Broadcast the distances to all processors and wait for all to receive this broadcast
    MPI_Bcast(distances, numV * numV, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    //  Calculate APSP
    int* apsp = calculateAPSP(numV, distances, numTargets, targets);

    //  Log the results of execution
    if (rank == 0) {
        gettimeofday(&end, NULL);
        printOutput(numV, apsp);
        double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
        printf("Total Execution Time: %10.6fs\n", delta);
        logOutput(sourceFileName, numV, apsp, delta);
        free(vertexSet);
        free(edgeArray);
        free(sourceFileName);
    }
    
    MPI_Finalize();
    return apsp[0];
}
