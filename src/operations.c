/*
    File Name:      operations.c
    Description:    Functions for the Floyd-Warshall Operation
    Author:         MIGUEL ARIES SAMBAT TABADERO (22240204)
    Last Modified:  10/10/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "fwd.h"

/*  Initialize the distances of all vertices to other vertices  */
/*  MPI-ify?  */
int** initDistances(int numV, int** adjMatrix) {
    unsigned int numEdges = numV * numV;
    int** distances = (int**) malloc(sizeof(int) * numEdges);
    for (int i = 0; i < numV; i++) {
        if (!(distances[i] = (int*) malloc(sizeof(int) * numEdges))) {
            fprintf(stderr, "Could not allocate memory for distances from edge %d @ %s (%d)", i, __func__, errno);
            return NULL;
        }
    }

    for (int i = 0; i < numV; i++) {
        for (int j = 0; j < numV; j++) {
            //  If the edge isn't pointing to itself, initialize the distance
            if (i != j) {
                distances[i][j] = (adjMatrix[i][j] != 0) ? adjMatrix[i][j] : INF;
            }
            //  Set dist (from edge to itself) to 0
            else {
                distances[i][j] = 0;
            }
        }
    }
    return distances;
}

int** floydWarshall(int numV, int** adjMatrix, int** distances) {
    for(int k = 0; k < numV; k++) {
        for (int i = 0; i < numV; i++) {
            for (int j = 0; j < numV; j++) {
                //  If these have uninitialized vertices, don't do anything
                if (distances[i][k] == INF || distances[k][j] == INF) {
                    continue;
                }
                if (distances[i][j] > distances[i][k] + distances[k][j]) {
                    distances[i][j] = distances[i][k] + distances[k][j];
                }               
            }
        }
    }
    return distances;
}
