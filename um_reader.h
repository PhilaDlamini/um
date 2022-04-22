/*
*                       um_reader.h
*
*
*   Summary: Interface for um_reader. Defines the reader function called in
*            um.c that reads in the input from provided program file.
*
*   Authors: vmccab01 and pdlami01
*/


#ifndef UM_READER
#define UM_READER

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>



/*
* Name: reader
* Usage: called by um.c to read through the provided um file.
* Expected Input: fp is expected to be a valid non null file pointer holding
*                 valid um code word instructions, bytes is the size of the
*                 provided .um file.
*/
extern uint32_t *reader(FILE *fp, int bytes);

#endif
