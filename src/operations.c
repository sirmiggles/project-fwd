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
void initDistance(int numV, int** distances) {
    int *dist = *distances;
    for (int i = 0; i < numV; i++) {
        dist[i] = (dist[i] == 0) ? INF : dist[i];
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
    initDistance(numV, &localEdgeArray);
    MPI_Gatherv(localEdgeArray, numLocalV, MPI_INT, destination, sendCounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    //  Free node-local memory pointers
    free(localEdgeArray);
    free(sendCounts);
    free(displs);
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

/*  Using MPI_Scatterv(), convert the given distances array into a 2D matrix 
    returns the edges of the node's targets
    returns NULL on error occurence
*/
int** convertToLocalMatrix(int numV, int* targets, int* distances, int numTargets) {
    //  Initialize output matrix
    int** localMatrix = allocContiguousMatrix(numTargets, numV);
    if(!localMatrix) {
        fprintf(stderr, "Error: could not allocate memory to matrix @ %s\n", __func__);
        return NULL;
    }

    for (int i = 0; i < numTargets; i++) {
        int targ = targets[i];
        int sp = targ * numV;
        int dest = 0;
        for (int j = sp; j < sp + numV; j++) {
            //  If i is the destination, set to 0
            localMatrix[i][dest] = (targ == dest) ? 0 : distances[j];
            dest++;
        }
    }

    return localMatrix;
}

int** gatherLocalMatrices(int numV, int** localMatrices, int numTargets) {
    int** adjMatrix = allocContiguousMatrix(numV, numV);
    if (!adjMatrix) {
        fprintf(stderr, "Error: could not allocate memory to adjMatrix @ %s\n", __func__);
        return NULL;
    }

    int *sendCounts, *displs;
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
            (sendCounts[i]) += numV;
            rem--;
        }

        displs[i] = sum;
        sum += sendCounts[i];
    }

    MPI_Gatherv(&(localMatrices[0][0]), numTargets, MPI_INT, &(adjMatrix[0][0]), sendCounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    return adjMatrix;
}

int** getSPOfTargets(int numV, int numTargets, int* targets, int** adjMatrix) {
    int** targetPaths = allocContiguousMatrix(numTargets, numV);
    if (!targetPaths) {
        fprintf(stderr, "Error: could not allocate memory to targetPaths @ %s\n", __func__);
        return NULL;
    }

    int** dist = allocContiguousMatrix(numV, numV);
    if (!dist) {
        fprintf(stderr, "Error: could not allocate memory to dists @ %s\n", __func__);
        return NULL;
    }
    
    for (int i = 0; i < numV; i++) {
        memcpy(dist[i], adjMatrix[i], sizeof(int) * numV);
    }

    for (int k = 0; k < numV; k++) {
        for (int i = 0; i < numTargets; i++) {
            int target = targets[i];
            for (int j = 0; j < numV; i++) {
                if (adjMatrix[target][k] == INF || adjMatrix[k][j] == INF) {
                    printf("Continuing...\n");
                    continue;
                }
                if (adjMatrix[i][j] > adjMatrix[i][k] + adjMatrix[k][j]) {
                    adjMatrix[i][j] = adjMatrix[i][k] + adjMatrix[k][j];
                }
            }
            targetPaths[i] = adjMatrix[target];
        }
    }

    return targetPaths;
}