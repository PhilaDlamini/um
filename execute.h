/*
*                       execute.h
*
*
*   Summary: Interface for execute. holds function that is called by main,
*            responsible for iterating through the instructions and properly
*            executing them.
*
*   Authors: vmccab01 and pdlami01
*/

#ifndef EXECUTE_READER
#define EXECUTE_READER

#include <stdlib.h>
#include <stdio.h>

#include "um_reader.h"
#include "bitpack.h"
#include "unpack.h"

typedef enum Um_opcode {
    CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
    NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

/*
* Name: um
* Usage: um is called by main. um creates the sequence of segments and executes
*        the instructions from the supplied segment 0.
* Expected Input: seg0 is expected to be a valid non null sequence of
*                 valid instruction code words.
*/
void um(uint32_t *seg0, int count);

#endif
