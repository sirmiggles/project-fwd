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
extern char*    parseFileName (char*);
extern int*     fileToPointer (char*);
extern void     logOutput(char*, int, int*, double);

/*  Utility functions  */
extern void     printUsage              (void);
extern void     printOutput             (int, int*);

/*  Operation functions  */
extern int*     allocateTargets         (int, int*, int*);
extern void     initDistance            (int, int**, int, int);
extern int*     initAllDistances        (int, int*);
extern int*     calculateAPSP           (int, int*, int, int*);
extern void     FloydWarshall           (int, int*, int, int*, int);
extern void     getRow                  (int, int*, int, int*);