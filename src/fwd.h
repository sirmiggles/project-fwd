/*
    File Name:      fwd.h
    Description:    Main header file for fwd
    Author:         MIGUEL ARIES SAMBAT TABADERO (22240204)
    Last Modified:  25/10/2019
*/


/*  Global Constants  */
extern const int FILEPATH_MAX;
extern const int INF;

extern int rank;
extern int clusterSize;

/*  File I/O functions  */
char*   parseFileName (char*);
int*    fileToPointer (char*);

/*  Utility functions  */
extern void printUsage (void);

/*  Operation functions  */
extern void     initDistances (int, int**);
//extern int*     initMatrix(int, int*, int, int);
