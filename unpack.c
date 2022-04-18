/*
*                       unpack.c
*
*   
*   Summary: unpack.c is the implementation for unpack.h. unpack.c is 
*            responsible for unpacking the provided code word into an instance
*            of an instruction struct. 
*
*
*   Authors: vmccab01 and pdlami01
*/


#include <stdlib.h> 
#include <stdio.h>
#include <mem.h>
#include <assert.h>

#include "unpack.h"
#include "bitpack.h"

const uint32_t LV = 13;
const uint32_t opcode_width = 4;
const uint32_t opcode_lsb = 28;
const uint32_t val_width = 25;
const uint32_t val_rC_lsb = 0;
const uint32_t reg_width = 3;
const uint32_t rA_lsb = 6;
const uint32_t rA_LV_lsb = 25;
const uint32_t rB_lsb = 3;

/*
* Name: unpack
* Summary: unpack uses bitpacking functions to unpack the provided codeword
*          into an instance of an Instruction struct. 
* Input: word is the instruction codeword fed into the UM by the user input 
*        program file
* Output: returns an instance of the Instruction struct that holds the opcode,
*         registers, and value provided by the codeword. 
* Side Effects: allocates memory for a new instance of an Instruction struct.
* Error Conditions: CRE if not enough available memory for a new Instruction.
*/
Instruction unpack(uint32_t word) 
{
    Instruction curr;
    NEW(curr);
    
    assert(curr != NULL);

    uint32_t opcode = Bitpack_getu(word, opcode_width, opcode_lsb);
    curr -> opcode = opcode;

    if (opcode == LV) {
        curr -> value = Bitpack_getu(word, val_width, val_rC_lsb);
        curr -> rA = Bitpack_getu(word, reg_width, rA_LV_lsb);
        curr -> rB = 0;
        curr -> rC = 0;
    } else {
        curr -> rA = Bitpack_getu(word, reg_width, rA_lsb);
        curr -> rB = Bitpack_getu(word, reg_width, rB_lsb);
        curr -> rC = Bitpack_getu(word, reg_width, val_rC_lsb);
        curr -> value = 0;
    }
    
    return curr;
}




