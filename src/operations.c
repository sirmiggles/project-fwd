/*
    File Name:      operations.c
    Description:    Functions for the Floyd-Warshall Operation
    Author:         MIGUEL ARIES SAMBAT TABADERO (22240204)
    Last Modified:  25/10/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <mpi.h>

#include "fwd.h"

/*  Initialize the distances for an array  */
void initDistance(int numV, int** distances) {
    int *dist = *distances;
    for (int i = 0; i < numV; i++) {
        dist[i] = (dist[i] == 0) ? INF : dist[i];
    }
    *distances = dist;
}

/*  Initialize the distances for the entire vertex set  */
int* initMatrix(int numV, int* edgeArray, int rank, int clusterSize) {
    int *sendCounts, *displs, *localEdgeArray;
    //  initialize sendCounts and displs for Scatterv
    if (!(sendCounts = (int *) malloc(sizeof(int) * clusterSize))) {
        fprintf(stderr, "Error: could not allocate memory to sendCounts @ %s", __func__);
        NULL;
    } 
    if (!(displs = (int *) malloc(sizeof(int) * clusterSize))) {
        fprintf(stderr, "Error: could not allocate memory to displs @ %s", __func__);
        NULL;
    }

    int rem = (numV * numV) % clusterSize;
    int sum = 0;
    for (int i = 0; i < clusterSize; i++) {
        sendCounts[i] = (numV * numV) / clusterSize;
        if (rem > 0) {
            (sendCounts[i])++;
            rem--;
        }

        displs[i] = sum;
        sum += sendCounts[i];
    }

    //  Destination pointer for scatter
    int numLocalV = sendCounts[rank];
    if (!(localEdgeArray = (int*) malloc(sizeof(int) * numLocalV))) {
        fprintf(stderr, "Error: could not allocate memory to localEdgeArray @ %s\n", __func__);
        return NULL;
    }

    //  Wait for processes to finish before performing scatter
    MPI_Barrier(MPI_COMM_WORLD);

    int* destination = (int *) malloc(sizeof(int) * numV * numV);
    if (!destination) {
        fprintf(stderr, "Error: could not allocate memory to destination @ %s\n", __func__);
        return NULL;
    }

    //  Scatter the edge array elements, and initialize the distances
    MPI_Scatterv(edgeArray, sendCounts, displs, MPI_INT, localEdgeArray, numLocalV, MPI_INT, 0, MPI_COMM_WORLD);
    initDistance(numV, &localEdgeArray);
    MPI_Gatherv(localEdgeArray, numLocalV, MPI_INT, destination, sendCounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    free(localEdgeArray);

    return destination;
}
