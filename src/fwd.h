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
extern void     printUsage              (void);
extern int**    allocContiguousMatrix   (int);

/*  Operation functions  */
extern int*     allocateTargets         (int, int*, int*);
extern void     initDistance            (int, int**);
extern int*     initAllDistances        (int, int*);
extern int**    convertToLocalMatrix    (int, int*, int*, int);
extern int**    gatherLocalMatrices     (int, int**, int);
