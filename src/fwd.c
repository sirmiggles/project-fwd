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
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#include <mpi.h>

#include "fwd.h"

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &clusterSize);

    time_t now;
    struct tm *execStart;
    char outFileName[32];
    int* edgeArray;
    int* vertexSet;
    int  numV = -1;
    //  Head Node File I/O
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

        int* inputData = fileToPointer(fileName);
        if (!inputData) {
            MPI_Finalize();
            return -1;
        }

        //  Initialize outfile params
        now = time(NULL);
        execStart = localtime(&now);
        strftime(outFileName, 32, "%Y-%m-%d_%H%M.out", execStart);

        numV = inputData[0];
        printf("Number of vertices in graph: %d\n", numV);
        if (numV < 1) {
            fprintf(stderr, "Error: insufficient number of vertices, numV. numV must be greater than 0.\n");
            MPI_Finalize();
            return -1;
        }

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
    MPI_Bcast(&numV, 1, MPI_INT, rank, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    //  Initialize the target matrices that this matrix will complete
    int numTargets = 0;
    int* targets = allocateTargets(numV, vertexSet, &numTargets);
    if (!targets) {
        MPI_Finalize();
        return -1;
    }

    //  Initialize the matrices
    int* distances = (int *) malloc(sizeof(int) * numV * numV);
    if(!(distances = initAllDistances(numV, edgeArray))) {
        MPI_Finalize();
        return -1;
    }

    //  Broadcast the distances
    MPI_Bcast(&distances[0], numV * numV, MPI_INT, rank, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    int** localTargetDistances = convertToLocalMatrix(numV, targets, distances, numTargets);
    if (!localTargetDistances) {
        return -1;
    }

    int** adjMatrix = allocContiguousMatrix(numV);
    if (!adjMatrix) {
        return -1;
    }

    //  Broadcast the newfound matrix
    adjMatrix = gatherLocalMatrices(numV, localTargetDistances, numTargets);
    MPI_Bcast(&(adjMatrix[0][0]), numV * numV, MPI_INT, rank, MPI_COMM_WORLD);

    if (!adjMatrix) {
        return -1;
    }
    

    if (rank == 0) {
        printf("%s\n", outFileName);
        free(edgeArray);        //  No longer needed
    }
    free(adjMatrix);
    free(distances);

    MPI_Finalize();
    return 0;
}
