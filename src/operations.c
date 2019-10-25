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
#include <string.h>

#include <mpi.h>

#include "fwd.h"

/*  Initialize the distances for an array 
    - Alters distances array by reference  
*/
void initDistance(int numLocalV, int** distances, int disp, int numV) {
    int *dist = *distances;
    for (int i = 0; i < numLocalV; i++) {
        int row = disp / numV;
        int col = disp % numV;
        dist[i] = (dist[i] == 0) ? INF : dist[i];
        if (row == col) {
            dist[i] = 0;
        }
        disp++;
    }
    *distances = dist;
}

/*  Initialize the distances for the entire vertex set 
    return the distances initialized for the vertex set
    return NULL on error occurrence
*/
int* initAllDistances(int numV, int* edgeArray) {
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
    initDistance(numLocalV, &localEdgeArray, displs[rank], numV);
    MPI_Gatherv(localEdgeArray, numLocalV, MPI_INT, destination, sendCounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    //  Free node-local memory pointers
    return destination;
}

/*  Using MPI_Scatterv(), allocate target vertices to nodes 
    returns the local targets for a node, if successful
    returns NULL, on error occurence.
*/
int* allocateTargets(int numV, int* vertexSet, int* numTargets) {
    //  initialize sendCounts and displs for Scatterv
    int *sendCounts, *displs;
    if (!(sendCounts = (int *) malloc(sizeof(int) * clusterSize))) {
        fprintf(stderr, "Error: could not allocate memory to sendCounts @ %s", __func__);
        NULL;
    } 
    if (!(displs = (int *) malloc(sizeof(int) * clusterSize))) {
        fprintf(stderr, "Error: could not allocate memory to displs @ %s", __func__);
        NULL;
    }

    //  Allocate work partition for scatter v
    int rem = numV % clusterSize;
    int sum = 0;
    for (int i = 0; i < clusterSize; i++) {
        sendCounts[i] = numV / clusterSize;
        if (rem > 0) {
            (sendCounts[i])++;
            rem--;
        }

        displs[i] = sum;
        sum += sendCounts[i];
    }

    *numTargets = sendCounts[rank];
    int* targets = (int *) malloc(sizeof(int) * (*numTargets));
    if (!targets) {
        fprintf(stderr, "Error: could not allocate memory to targets @ %s\n", __func__);
        return NULL;
    }
    
    //  Scatter the edge array elements, and initialize the distances
    MPI_Scatterv(vertexSet, sendCounts, displs, MPI_INT, targets, (*numTargets), MPI_INT, 0, MPI_COMM_WORLD);
    return targets;
}

int* calculateAPSP(int numV, int* distances, int numTargets, int* targets) {
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

    int* apsp = (int *) malloc(sizeof(int) * numV * numV);
    if (!apsp) {
        fprintf(stderr, "Error: could not allocate memory to apsp @ %s\n", __func__);
        return NULL;
    }
    
    //  Scatter the edge array elements, and initialize the distances
    MPI_Scatterv(distances, sendCounts, displs, MPI_INT, localEdgeArray, numLocalV, MPI_INT, 0, MPI_COMM_WORLD);
    FloydWarshall(numV, localEdgeArray, numTargets, targets, numLocalV);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gatherv(localEdgeArray, numLocalV, MPI_INT, apsp, sendCounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    return apsp;
}

void FloydWarshall(int numV, int* dist, int numTargets, int* targets, int numLocalV) {
    int k, i, j;
    int kNode;
    int* kRow = (int*) malloc(sizeof(int) * numV);

    for (k = 0; k < numV; k++) {
        if (numV / clusterSize == 0 || numV < clusterSize)
            kNode = 0;
        else 
            kNode = k / (numV / clusterSize);

        if (rank == kNode) {
            getRow(k, kRow, numV, dist);
        }

        MPI_Bcast(kRow, numV, MPI_INT, kNode, MPI_COMM_WORLD);

        for (i = 0; i < numTargets; i++) {
            for (j = 0; j < numV; j++) {
                if (dist[i * numV + k] + kRow[j] < dist[i * numV + j]) {
                    dist[i * numV + j] = dist[i * numV + k] + kRow[j];
                }
            }
        }
    }
    //  Set unreachables to -1
    for (int i = 0; i < numLocalV; i++) {
        if (dist[i] == INF) {
            dist[i] = -1;
        }
    }
}

void getRow(int v, int* rowOfV, int numV, int* dist) {
    int offSet;
    if (numV / clusterSize == 0 || numV < clusterSize) {
        offSet = 0;
    }
    else {
        offSet = v % (numV / clusterSize);
    }
    for (int i = 0; i < numV; i++) {
        rowOfV[i] = dist[i + offSet * numV];
    }
}
