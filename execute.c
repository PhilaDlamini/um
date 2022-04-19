/*
*                       execute.c
*
*
*   Summary: execute.c is the implementation for execute.h. it holds all the
*            individual functinos for all the opcodes. execute is responsible
*            for iterating through segment 0, unpacking, and executing the
*            instructions.
*
*   Authors: vmccab01 and pdlami01
*/

#include "execute.h"
#include "unpack.h"
#include "assert.h"
#include "execute.h"
#include <mem.h>
#include <inttypes.h>
#include <math.h>

const int hint = 16;


/*
* Name: free_sequences
* Summary: free sequences is a helper funciton that frees the memory of the
*          given sequences: segments and ids.
* Input: segments is the sequence that holds all of the program's segments and
*        ids holds the unmapped segment identifiers. both are expected to be
*         valid non NULL Seq_T.
* Output: N/A
* Side Effects: permenantly frees memory allocated for the segments and ids.
* Error Conditions: CRE if segments is NULL, CRE if ids is NULL
*/
void free_sequences(Seq_T segments, Seq_T ids)
{
    assert(segments != NULL && ids != NULL);
    int len = Seq_length(segments);
    for (int i = len - 1; i >= 0; i--) {
        Seq_T curr = (Seq_T) Seq_get(segments, i);
        if (curr != NULL) {
            Seq_free(&curr);
        }
    }
    Seq_free(&segments);
    Seq_free(&ids);
}

/*
* Name: load_value
* Summary: load value loads the given value into the given register.
* Input: value is the desired value to be loaded into a given register, r is
*        a pointer to the address of the desired register to be updated.
* Output: N/A
* Side Effects: register is updated by reference.
* Error Conditions: N/A
*/
void load_value(uint32_t value, uint32_t *r)
{
    *r = value;
}


/*
* Name: Output
* Summary: ouptut writes the given value to standard output.
* Input: value is an integer expected to be between 0 and 255.
* Output: No return value but 'value' is pritned to standard output.
* Side Effects: N/A
* Error Conditions: CRE if given value is not between 0 and 255.
*/
void output(int value)
{
    assert(value >= 0 && value <= 255);
    printf("%c", value);
}

/*
* Name: add
* Summary: add updates the value at register rA to be equal to rB + rC.
* Input: add takes in the address of register rA and the values at registers
*        rB and rC. rA is expected to be a valid non null address.
* Output: N/A
* Side Effects: register rA is updated by reference.
* Error Conditions: CRE if rA is a null pointer.
*/
void add(uint32_t *rA, uint32_t rB, uint32_t rC)
{
    assert(rA != NULL);
    *rA = rB + rC;
}

/*
* Name: mult
* Summary: mult updates the value at register rA to be equal to the product of
*          rB and rC
* Input: rA is the address of register rA, rB is the value at register rB, and
*        rC is the value at register rC. rA is expected to be a valid non null
*        address.
* Output: N/A
* Side Effects: register rA is updated by reference.
* Error Conditions: CRE if rA is a null pointer
*/
void mult(uint32_t *rA, uint32_t rB, uint32_t rC)
{
    assert(rA != NULL);
    *rA = rB * rC;
}

/*
* Name: division
* Summary: division updates the value at register rA to be equal to the value
*          at register rB divided by the value at register rC.
* Input: rA is the address of register rA, rB is the value at register rB, and
*        rC is the value at register rC. rA is expected to be a valid non null
*        address and rC is expected to be a non zero value.
* Output: N/A
* Side Effects: rA is updated by reference.
* Error Conditions: CRE if user attempts to divide by zero
*/
void division(uint32_t *rA, uint32_t rB, uint32_t rC)
{
    assert(rC != 0);
    *rA = (rB / rC);
}

/*
* Name: cmov
* Summary: cmov is the funciton for the conditional move operator in which the
*          value at register rA is to be equal to the value at register rB if
*          the value at register rC is not zero.
* Input: rA is the address of register rA, rB is the value at register rB, and
*        rC is the value at register rC. rA is expected to be a valid non null
*        address.
* Output: N/A
* Side Effects: rA is updated by reference.
* Error Conditions: CRE if rA is null.
*/
void cmov(uint32_t *rA, uint32_t rB, uint32_t rC)
{
    assert(rA != NULL);
    if (rC != 0) {
        *rA = rB;
    }
}

/*
* Name: in
* Summary: in accepts 1 character from standard input and updates register rC
*          to be the inputted character. if the input is signalled to be end of
*          input, rC is populated with a uint32_t of all 1s.
* Input: rC is the address of register rC. a valid non null register address is
*        expected.
* Output: N/A
* Side Effects: rC is updated by reference.
* Error Conditions: CRE if rC is NULL.
*/
void in(uint32_t *rC)
{
    assert(rC != NULL);
    char c = getc(stdin);
    if (c != EOF) {
        *rC =  c;
    } else {
        *rC = ~0;
    }
}

/*
* Name: nand
* Summary: nand is a bitwise NAND function that "ands" the values at registers
*          rB and rC, "nots" them, then loads the value into register rA.
* Input: rA is the address of register rA, rB is the value at register rB, and
*        rC is the value at register rC. rA is expected to be a valid non null
*        address.
* Output: N/A
* Side Effects: rA is updated by reference.
* Error Conditions: CRE if rA is NULL.
*/
void nand(uint32_t *rA, uint32_t rB, uint32_t rC)
{
    assert(rA != NULL);
    *rA = ~(rB & rC);
}

/*
* Name: map
* Summary: map() maps a new segment of size rC using the given register id.
*          the new segment is added to the sequence of segments ('segments')
*          and the value at register rB is updated to be equal to the id of
*          the newly mapped segment.
* Input: new_id is the register id to be used for the newly mapped segment,
*        segments is the sequence of segments, rB is the address of register
*        rB, and rC is the value at register rC. new_id is expected to be non 0
*        and rB is expected to be a valid non null address.
* Output: N/A - no return value
* Side Effects: The sequence of segments is permanently updated and register
*               rB is updated by reference.
* Error Conditions: CRE if new_id is 0, CRE if no space available for
*                  Sequence'new_map', CRE if rB is null.
*/
void map(uint32_t new_id, Seq_T segments, uint32_t *rB, uint32_t rC)
{

    assert(new_id != 0 && rB != NULL);
    Seq_T new_map = Seq_new(rC);
    assert(new_map != NULL);

    //Initialize the segment with 0s
    for (uint32_t i = 0; i < rC; i++) {
        Seq_addhi(new_map, (void*) (uintptr_t) 0);
    }

    if (new_id < (uint32_t) Seq_length(segments)) {
        Seq_put(segments, new_id, new_map);
    } else {
        Seq_addhi(segments, new_map);

    }

    *rB = new_id;

}

/*
* Name: unmap
* Summary: unmap frees the segment with identifier rC from the sequence
*          of segments. the identifier rC is added to the sequence of
*          identifiers that can be reused.
* Input: ids is the sequence of reusable segment ids, segments is the sequence
*        of segments, and rC is the value at register rC. ids and segments are
*        expected to be non null and rC is expected to be a non zero value.
* Output: N/A
* Side Effects: permenantly frees the memory allocated for the unmapped segment
*               and the ids sequence is updated to include the unmapped segment
*               identifier.
* Error Conditions: CRE if rC is 0 (user attempts to unmap segment 0), CRE if
*                   rC points to a segment that has not been mapped, CRE if ids
*                   or segments are null.
*/
void unmap(Seq_T ids, Seq_T segments, uint32_t rC)
{
    assert(rC != 0 && ids != NULL && segments != NULL);
    assert(rC < (uint32_t) Seq_length(segments));

    //Save the unmapped id
    Seq_addhi(ids, (void *) (uintptr_t) rC);

    //Unmap (will seg fault if rC is unmapped already)
    Seq_T to_remove = (Seq_T) Seq_get(segments, rC);
    Seq_free(&to_remove);
    Seq_put(segments, rC, NULL);

}

/*
* Name: sload
* Summary: sload is the function for the segmented load opcode. sload sets
*          the value at register rA equal to the value at segment with
*          identifier rB at index rC.
* Input: segments is the sequence of segments, rA is the address of register
*        rA, rB is the value in register rB, and rC is the value in register
*        rC. segments is expected to be non null and rA is expected to be a
*        valid non null adress.
* Output: N/A
* Side Effects: register rA is updated by reference.
* Error Conditions: CRE if segments is null, CRE if rA is null.
*/

void sload(Seq_T segments, uint32_t *rA, uint32_t rB, uint32_t rC)
{
    assert(segments != NULL && rA != NULL);
    Seq_T curr = (Seq_T) Seq_get(segments, rB);
    uint32_t value = (uint32_t) (uintptr_t) Seq_get(curr, rC);
    *rA = value;
}

/*
* Name: sstore
* Summary: sstore is the function for the segmented store opcode. sstore stores
*          the value from register rA into the segment with identifier rB at
*          index rC.
* Input: segments is the sequence of segments, rA is the value at register rA,
*        rB is the value at register rB, and rC is the value at register rC.
* Output: N/A
* Side Effects: segment with identifier rB is updated at index rC.
* Error Conditions: CRE if segments is NULL.
*/
void sstore(Seq_T segments, uint32_t rA, uint32_t rB, uint32_t rC)
{
    assert(segments != NULL);
    Seq_T curr = (Seq_T) Seq_get(segments, rA);
    Seq_put(curr, rB, (void*) (uintptr_t) rC);
}

/*
* Name: loadp
* Summary: loadp duplicates the desired segment, frees the existing segment 0,
*          and loads the duplicated segment into segment 0. rB is the
*          identifier of the segment the user wants to duplicate.
* Input: segments is the sequence of segments, rB is the value at register rB.
* Output: N/A.
* Side Effects: memory of existing segment 0 is freed, segments is updated to
*               have the duplicated segment at index 0 (new segment 0)
* Error Conditions: CRE if segments is null, CRE if not enough memory for
*                   segment to be duplicated
*/
void loadp(Seq_T segments, uint32_t rB)
{
    assert(segments != NULL);
    Seq_T segB = Seq_get(segments, rB);

    Seq_T duplicate = Seq_new(Seq_length(segB));
    assert(duplicate != NULL);

    //Duplicate segment
    for(int i = 0; i < Seq_length(segB); i++) {
        Seq_addhi(duplicate, Seq_get(segB, i));
    }

    //Free segment 0
    Seq_T seg0 = (Seq_T) Seq_get(segments, 0);
    Seq_free(&seg0);

    // loads duplicated segment to be the new segment 0
    Seq_put(segments, 0, duplicate);
}


/*
* Name: execute
* Summary: execute is responsible for calling the appropriate
*          functions in response to different opcodes. it is
*          just a large switch statement that passes the appropriate
*          registers and parameters to the applicable function.
* Input: intruction is an instance of the Instruction struct that holds the
*        current opcode and registers, segments is the sequence of segments,
*        ids is the sequence of previously unmapped segment identifiers that
*        can be reused, registers is a pointer to the 32 bit registers 0-7,
*        and counter is a pointer to the program counter.
* Output: N/A
* Side Effects: side effects of called function.
* Error Conditions: error conditions of called funciton.
*/
void execute(Instruction instruction, Seq_T segments, Seq_T ids,
            uint32_t *registers, int *counter)
{

    uint32_t opcode = instruction -> opcode;
    switch(opcode) {
        case CMOV:
            cmov(&registers[instruction -> rA], registers[instruction -> rB],
                registers[instruction -> rC]);
            break;
        case SLOAD:
            sload(segments, &registers[instruction -> rA],
                registers[instruction -> rB],
                registers[instruction -> rC]);
            break;
        case SSTORE:
            sstore(segments, registers[instruction -> rA],
                registers[instruction -> rB],
                registers[instruction -> rC] );
            break;
        case ADD:

            add(&registers[instruction -> rA], registers[instruction -> rB],
                registers[instruction -> rC]);
            break;
        case MUL:
            mult(&registers[instruction -> rA], registers[instruction -> rB],
                    registers[instruction -> rC]);
                break;

        case DIV:
            division(&registers[instruction -> rA],
                    registers[instruction -> rB],
                    registers[instruction -> rC]);
                break;

        case NAND:
            nand(&registers[instruction -> rA], registers[instruction -> rB],
                registers[instruction -> rC]);
            break;
        case ACTIVATE:
        {
            uint32_t new_id = Seq_length(ids) == 0
                    ? (uint32_t) Seq_length(segments)
                    : (uint32_t) (uintptr_t) Seq_remlo(ids);

            map(new_id, segments, &registers[instruction -> rB],
                registers[instruction -> rC]);
            break;
        }
        case INACTIVATE:
        {
            unmap(ids, segments, registers[instruction -> rC]);

        }
            break;
        case OUT:
        {
            int value = registers[instruction -> rC];
            output(value);
            break;
        }
        case IN:
            in(&registers[instruction -> rC]);
            break;
        case LOADP:

            if (registers[instruction -> rB] != 0) {
                loadp(segments, registers[instruction -> rB]);
            }
            *counter = registers[instruction -> rC];
            break;
        case LV:
            load_value(instruction -> value, &registers[instruction -> rA]);
            break;
    }

}

/*
* Name: um
* Summary: um is the function called by um.c in main. um takes in the read in
*          segment 0, unpacks the instruction, and passes the instructions to
*          execute(). these happen in a while loop that runs while the program
*          counter is less than the size of segment 0
* Input: segment 0 (sequence). expected to be a valid non-null Seq_T.
* Output: N/A
* Side Effects: Memory allocated for Seq_T 'segments' that holds all segments,
*               memory allocated and free'd for instances of
*               Instruction struct, and memory allocated for Sequence
*               of unmapped identifiers. Function that frees memory for
*               unmapped identifiers and segments called here.
*
* Error Conditions: CRE if seg0 is null. all error conditions of called opcode
*                   instructions apply.
*/
void um(Seq_T seg0)
{
    assert(seg0 != NULL);

    Seq_T segments = Seq_new(hint);
    assert(segments != NULL);

    Seq_addhi(segments, seg0);

    // reusable identifiers
    Seq_T ids = Seq_new(1);
    assert(ids != NULL);

    uint32_t *registers = malloc(8 * sizeof(uint32_t));
    assert(registers != NULL);

    for (int i = 0; i < 8; i++) {
        registers[i] = 0;
    }

    int counter = 0;
    while (counter < Seq_length(Seq_get(segments, 0))) {

        uint32_t word = (uint32_t) (uintptr_t) Seq_get(Seq_get(segments, 0),
                                                       counter);
        Instruction instruction;
        NEW(instruction);

        /* unpack in place */
        uint32_t opcode = word >> 28;
        instruction -> opcode = opcode;

        if(opcode == LV) {
          uint32_t wordrA = word;
          uint32_t wordVal = word;
          instruction -> value = (wordrA << 7) >> 7;
          instruction -> rA = (wordVal << 4) >> 29;
          instruction -> rB = 0;
          instruction -> rC = 0;
        } else {
          uint32_t wordrA = word;
          uint32_t wordrB = word;
          uint32_t wordrC = word;
          instruction -> rA = (wordrA << 23)  >> 29;
          instruction -> rB = (wordrB << 26)  >> 29;
          instruction -> rC = (wordrC << 29)  >> 29;
          instruction -> value = 0;
        }

        //(word << (64 - (lsb + width)))  >> (64 - width)//shl

      //  printf("op: %d, rA: %d, rB: %d, rC: %d, value: %d\n",   instruction -> opcode,   instruction -> rA, instruction->rB, instruction->rC, instruction->value);


        if (instruction -> opcode == HALT) {
            FREE(instruction);
            break;
        }

        execute(instruction, segments, ids, registers, &counter);

        if (instruction -> opcode != LOADP) {
            counter++;
        }

        FREE(instruction);
    }

    free(registers);
    free_sequences(segments, ids);
}
