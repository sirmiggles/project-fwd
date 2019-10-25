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
int* initMatrix(int numV, int* edgeArray) {
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

/*  
    Using MPI_Scatterv(), allocate target vertices to nodes
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

/*  Using MPI_Scatterv(), convert the given distances array into a 2D matrix  */
int** convertEdgesToMatrix(int numV, int* targets, int* distances, int clusterSize) {
    //  Initialize output matrix
    int** matrix = (int**) malloc(sizeof(int*) * numV);
    for (int i = 0; i < numV; i++) {
        if(!(matrix[i] = (int*) malloc(sizeof(int*) * numV))) {
            fprintf(stderr, "Error: could not allocate memory to matrix @ %s\n", __func__);
            return NULL;
        }
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
    
    return matrix;
}

