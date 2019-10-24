/*
    File Name:      fileIO.c
    Description:    Contains the binary file parser for fwd
    Author:         MIGUEL ARIES SAMBAT TABADERO (22240204)
    Last Modified:  10/10/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "fwd.h"

/*  Check if file exists and can be read  */
bool fileAccessible(char* fileName) {
    return (access(fileName, F_OK) != -1 && access(fileName, R_OK) != -1)   \
            ? true : false;
}

/*  Parses the file name to check if it's valid  */
char* parseFileName(char* arg) {
    char* fileName = (char*) malloc(FILEPATH_MAX * sizeof(char));
    if (!fileName) {
        fprintf(stderr, "Error: could not allocate memory for fileName @%s (%d)\n", __func__, errno);
        return NULL;
    }

    bool fileIsAccessible = fileAccessible(arg);
    if (fileIsAccessible) {
        strcpy(fileName, arg);
        return fileName;
    }
    
    else {
        fprintf(stderr, "%s cannot be found or accessed!\n", arg);
        free(fileName);
        return NULL;
    }
}

/*  Convert the file to an int pointer  */
int* fileToPointer(char* fileName) {
    printf("Attempting to read file @%s...\n", fileName);
    FILE* fp = fopen(fileName, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Could not open the file %s. (%d)\n ", fileName, errno);
        return NULL;
    }

    //  Determine out the size of the file and elements required in pointer
    struct stat fileInfo;
    stat(fileName, &fileInfo);

    //  Read the file and convert it into an int pointer
    int* adjMatrix = (int*) malloc(fileInfo.st_size * sizeof(int));
    if (!adjMatrix) {
        fprintf(stderr, "Error: could not allocate memory for adjMatrix @ %s (%d)\n", __func__, errno);
        return NULL;
    }

    int err = fread(adjMatrix, fileInfo.st_size, sizeof(int), fp);
    if (err < 1) {
        fprintf(stderr, "Error: Error parsing the file\n");
        return NULL;
    }
    fclose(fp);
    return adjMatrix;
}
