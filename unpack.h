/*
*                       unpack.h
*
*   
*   Summary: unpack.h is the interface for unpack.c
*
*
*   Authors: vmccab01 and pdlami01
*/



#ifndef UNPACK_READER
#define UNPACK_READER

#include <stdlib.h> 
#include <stdio.h>
#include <stdint.h>

/*
* Instruction is the struct used for holding the information from 
* unpacked code words. Holds the registers, value, and opcode.
*/
struct Instruction {
    uint32_t rA, rB, rC;
    uint32_t value;
    uint32_t opcode;
};

typedef struct Instruction *Instruction; 

/*
* Name: unpack
* Usage: unpack is used to unpack the instruction from a provided code word. 
*        Used to unpack the words provided by the program .um file provided
*        to the um. Unpacks word into an instance of an Instruction struct.
* Expected Input: expects a valid code word that holds an opcode and applicable
*                 registers and or value.
*/
Instruction unpack(uint32_t word);


#endif