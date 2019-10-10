/*
    File Name:      fwd.c
    Description:    Main C File for fwd
                    > Contains the entry point (main) for the program
    Author:         MIGUEL ARIES SAMBAT TABADERO (22240204)
    Last Modified:  09/10/2019
*/

#include <stdio.h>

#include <mpi.h>

#include "fwd.h"

int main(int argc, char** argv) {
    int processRank, clusterSize;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &clusterSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);
    printf("Hello World from %d of %d\n", processRank, clusterSize);
    MPI_Finalize();
    return 0;
}