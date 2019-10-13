/*
    File Name:      fwd.h
    Description:    Main header file for fwd
    Author:         MIGUEL ARIES SAMBAT TABADERO (22240204)
    Last Modified:  10/10/2019
*/


/*  Global Variables  */
extern const int FILEPATH_MAX;

/*  File I/O functions  */
char*   parseFileName (char*);
int*    fileToPointer (char*);
int**   convertTo2DMatrix   (int, int*);

/*  Utility functions  */
extern void printUsage (void);