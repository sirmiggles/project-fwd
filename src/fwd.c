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

#include <mpi.h>

#include "fwd.h"

int main(int argc, char** argv) {
    //  Print usage if there are no arguments provided
    if (argc < 2) {
        printUsage();
        return -1;
    }

    char* fileName = malloc(FILEPATH_MAX * sizeof(char));
    if (!fileName) {
        fprintf(stderr, "Error: could not allocate memory. (%d)\n", errno);
        return -1;
    }

    parseFileName(argv[1], fileName);
    printf("%s\n", fileName);

    FILE* fp = fopen(fileName, "rb");
    if (!fp) {
        fprintf(stderr, "Error: could not open file. (%d)\n", errno);
        return -1;
    }
    
    struct stat fileInfo;
    stat(fileName, &fileInfo);
    int* adjMatrix = malloc(fileInfo.st_size);
    if (!adjMatrix) {
        fprintf(stderr, "Error: could not allocate memory. (%d)\n", errno);
        return -1;
    }

    int err = fread(adjMatrix, fileInfo.st_size, 1, fp);
    if (err < 1) {
        fprintf(stderr, "Error: Error parsing the file\n");
        return -1;
    }
    printf("%d\n", adjMatrix[0]);
}