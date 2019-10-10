/*
    File Name:      parser.c
    Description:    Contains the binary file parser for fwd
    Author:         MIGUEL ARIES SAMBAT TABADERO (22240204)
    Last Modified:  10/10/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "fwd.h"

/*  Check if file exists and can be read  */
bool fileAccessible(char* fileName) {
    return (access(fileName, F_OK) != -1 && access(fileName, R_OK) != -1)   \
            ? true : false;
}

/*  Parses the file name to check if it's valid  */
void parseFileName(char* arg, char* destFileName) {
    bool fileIsAccessible = fileAccessible(arg);
    if (fileIsAccessible) {
        strcpy(destFileName, arg);
    }
    else {
        fprintf(stderr, "%s cannot be found or accessed!\n", arg);
    }
}

