/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.
 *
 * Any additional functions and unit tests written for the lab go
 * here.
 *
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>
#include <math.h>



typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
    Um_instruction instruction = 0x0;
    instruction = Bitpack_newu(instruction, 3, 0, rc);
    instruction = Bitpack_newu(instruction, 3, 3, rb);
    instruction = Bitpack_newu(instruction, 3, 6, ra);
    instruction = Bitpack_newu(instruction, 4, 28, op);
    return instruction;
 }

Um_instruction loadval(unsigned ra, unsigned val)
{
    Um_instruction instruction = 0x0;  //word, width, lsb, value
    instruction = Bitpack_newu(instruction, 25, 0, val);
    instruction = Bitpack_newu(instruction, 3, 25, ra);
    instruction = Bitpack_newu(instruction, 4, 28, 13);
    return instruction;
}

/* Wrapper functions for each of the instructions */
static inline Um_instruction halt(void)
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c)
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction input(Um_register a, Um_register b, Um_register c)
{
        return three_register(IN, a, b, c);
}

static inline Um_instruction mult(Um_register a, Um_register b, Um_register c)
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction bnand(Um_register a, Um_register b, Um_register c)
{
        return three_register(NAND, a, b, c);
}

static inline Um_instruction division(Um_register a, Um_register b, Um_register c)
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction map(Um_register a, Um_register b, Um_register c)
{
        return three_register(ACTIVATE, a, b, c);
}

static inline Um_instruction unmap(Um_register a, Um_register b, Um_register c)
{
        return three_register(INACTIVATE, a, b, c);
}

static inline Um_instruction sload(Um_register a, Um_register b, Um_register c)
{
        return three_register(SLOAD, a, b, c);
}

static inline Um_instruction sstore(Um_register a, Um_register b, Um_register c)
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction cmov(Um_register a, Um_register b, Um_register c)
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction loadp(Um_register a, Um_register b, Um_register c)
{
        return three_register(LOADP, a, b, c);
}

static inline Um_instruction invalid(Um_register a, Um_register b, Um_register c)
{
        return three_register(15, a, b, c);
}


Um_instruction output(Um_register c)
{
    return three_register(OUT, 0, 0, c);
}

/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }

}


/*##################### Tests for output #######################*/

// Test for output
void build_LV_output_halt(Seq_T stream)
{
        append(stream, loadval(r3, 97));
        append(stream, output(r3));
        append(stream, halt());
        append(stream, output(r3));

}

//Prints "howdy" out
void howdy(Seq_T stream)
{
        append(stream, loadval(r3, 72));
        append(stream, output(r3));
        append(stream, loadval(r3, 111));
        append(stream, output(r3));
        append(stream, loadval(r3, 119));
        append(stream, output(r3));
        append(stream, loadval(r3, 100));
        append(stream, output(r3));
        append(stream, loadval(r3, 121));
        append(stream, output(r3));
        append(stream, loadval(r3, 33));
        append(stream, output(r3));
        append(stream, loadval(r3, 10));
        append(stream, output(r3));
        append(stream, halt());
}

/* Prints characters 0 - 255*/
void print_all_chars(Seq_T stream) {
        for(int i = 0; i <= 255; i++){
                append(stream, loadval(r3, i));
                append(stream, output(r3));
        }
        append(stream, halt());
}

/* Prints character that cannot be printed */
void error(Seq_T stream)
{
        append(stream, loadval(r3, 500));
        append(stream, output(r3));
}


/* Tests for add */
void build_add(Seq_T stream)
{
        //Add 6 and 9
        append(stream, loadval(r2, 500));
        append(stream, loadval(r3, 9));
        append(stream, add(r1, r2, r3));

        //Add 300 and 450
        append(stream, loadval(r2, 300));
        append(stream, loadval(r3, 450));
        append(stream, add(r1, r2, r3));

        //Add -4 and 9
        append(stream, loadval(r2, 0x1fffffc));
        append(stream, loadval(r3, 9));
        append(stream, add(r1, r2, r3));

        //Add 2^25 and 2^25  (will not mod)
        append(stream, loadval(r2, pow(2, 25) - 1));
        append(stream, loadval(r3, pow(2, 25) - 1));
        append(stream, add(r1, r2, r3));

        //Mult 2^25 by 2^6, add 129
        append(stream, loadval(r2, pow(2, 25) - 1));
        append(stream, loadval(r3, pow(2, 7)));
        append(stream, mult(r5, r2, r3));
        append(stream, loadval(r6, 129));
        append(stream, add(r7, r5, r6));
        append(stream, halt());
}


/* Tests for add */
void build_mult(Seq_T stream)
{
        //Mult 6 and 9
        append(stream, loadval(r2, 6));
        append(stream, loadval(r3, 9));
        append(stream, mult(r1, r2, r3));

        //Mult 300 and 40
        append(stream, loadval(r2, 300));
        append(stream, loadval(r3, 40));
        append(stream, mult(r1, r2, r3));

        //Mult 0 and 232
        append(stream, loadval(r2, 0));
        append(stream, loadval(r3, 232));
        append(stream, mult(r1, r2, r3));

        //Mult 2^32 - 1 and 2 ^ 10
        append(stream, loadval(r2, pow(2, 25) - 1));
        append(stream, loadval(r3, pow(2, 10)));
        append(stream, mult(r1, r2, r3));

        append(stream, halt());

}


/* Tests for div */
void build_div(Seq_T stream)
{
        //Div 20 by 4
        append(stream, loadval(r2, 20));
        append(stream, loadval(r3, 4));
        append(stream, division(r1, r2, r3));

        //Div 5 by 3
        append(stream, loadval(r2, 5));
        append(stream, loadval(r3, 3));
        append(stream, division(r1, r2, r3));

        //Div 6 by 10
        append(stream, loadval(r2, 6));
        append(stream, loadval(r3, 10));
        append(stream, division(r1, r2, r3));

        //Div 2^32 by 2300
        append(stream, loadval(r2, pow(2, 25) - 1));
        append(stream, loadval(r3, 2300));
        append(stream, division(r1, r2, r3));
        append(stream, halt());
}

/* ##################### Tests for CMOV #######################*/
void build_cmov(Seq_T stream)
{
        //Move should occur
        append(stream, loadval(r1, 100)); //RB
        append(stream, loadval(r2, 20)); //RC
        append(stream, loadval(r3, 4)); //RB
        append(stream, cmov(r1, r3, r2)); ///a, b, c

         //Move should not occur
        append(stream, loadval(r1, 100)); //RB
        append(stream, loadval(r2, 0)); //RC
        append(stream, loadval(r3, 4)); //RB
        append(stream, cmov(r1, r3, r2)); ///a, b, c

        append(stream, halt());

}

/* ########################### Test input ####################*/
void build_input(Seq_T stream)
{
        for(int i = 0; i < 5; i++) {
                append(stream, input(r1, r2, r3)); //store to r3
                append(stream, output(r3));
        }
        append(stream, loadval(r3, pow(2, 10)));
        append(stream, mult(r1, r2, r3));
        append(stream, halt());

}

void build_input_file(Seq_T stream)
{
        for(int i = 0; i < 3482; i++) {
                append(stream, input(r1, r2, r3)); //store to r3
                append(stream, output(r3));
        }
        append(stream, input(r1, r2, r3));
        append(stream, halt());

}

/* ########################## Test for BNAND ##################*/
void build_bnand(Seq_T stream)
{
        append(stream, loadval(r1, 43));
        append(stream, loadval(r2, 4));
        append(stream, bnand(r3, r2, r1));

        append(stream, loadval(r2, 50));
        append(stream, loadval(r2, 100));
        append(stream, bnand(r3, r2, r1));
        append(stream, halt());

}

/*######################## Test for map unmapping ##################*/

void build_load_store(Seq_T stream)
{

        //map seg 1
        append(stream, loadval(r3, 2));
        append(stream, map(r1, r2, r3));

        //store seg 1, index 0
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 0));
        append(stream, loadval(r3, 105));
        append(stream, sstore(r1, r2, r3));  //m[a][b] = [c]

        //load [a] = m[b][c]
        append(stream, loadval(r2, 1));
        append(stream, loadval(r3, 0));
        append(stream, sload(r1, r2, r3));  //[a] = m[b][c]
        append(stream, output(r1));

}


/*######################### Outputs text #################*/
void build_print_hello(Seq_T stream) {
        append(stream, loadval(r3, 104));
        append(stream, output(r3));
        append(stream, loadval(r3, 101));
        append(stream, output(r3));
        append(stream, loadval(r3, 108));
        append(stream, output(r3));
        append(stream, loadval(r3, 108));
        append(stream, output(r3));
        append(stream, loadval(r3, 111));
        append(stream, output(r3));
        append(stream, loadval(r3, 10));
        append(stream, output(r3));

        //Load program at seg 1
        append(stream, loadval(r6, 0));
        append(stream, loadval(r2, 1));
        append(stream, loadp(r4, r2, r6));
}


/*
     0: [0xd6000068] r3 := 104;
     1: [0xa0000003] output r3;
     2: [0xd6000065] r3 := 101;
     3: [0xa0000003] output r3;
     4: [0xd600006c] r3 := 108;
     5: [0xa0000003] output r3;
     6: [0xd600006c] r3 := 108;
     7: [0xa0000003] output r3;
     8: [0xd600006f] r3 := 111;
     9: [0xa0000003] output r3;
    10: [0xd600000a] r3 := 10;
    11: [0xa0000003] output r3;
*/

void load_p(Seq_T stream) {

        // map segment
        append(stream, loadval(r3, 2)); //r3 = rC
        append(stream, map(r1, r2, r3)); // rA, rB = id, rC = size

        // create instruction 1
        // instruction load h will be in r4
        append(stream, loadval(r4, 2081)); // r4 = 2081
        append(stream, loadval(r5, 71887)); // r5 = 71887
        append(stream, mult(r6, r4, r5)); // r6 = r4* r5
        append(stream, loadval(r5, 24));
        append(stream, mult(r4, r6, r5));

        // load into segment 1, 0
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 0));
        append(stream, sstore(r1, r2, r4)); //m[a][b] = [c]

        // create instruction 2
        // output h
        append(stream, loadval(r4, 14197)); // r4 = 2081
        append(stream, loadval(r5, 17189)); // r5 = 71887
        append(stream, mult(r6, r4, r5)); // r6 = r4* r5
        append(stream, loadval(r5, 11));
        append(stream, mult(r4, r6, r5));

        // load into segment 1, 1
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 1));
        append(stream, sstore(r1, r2, r4)); //m[a][b] = [c]

        // load program
        append(stream, loadval(r7, 0));
        append(stream, loadp(r1, r2, r7)); // load m[rB], program counter = rC
        append(stream, halt());
}

/* Instructions that should result in a failure */
void fail_1(Seq_T stream) {

        //Print 'a', load program @ seq 0, set counter at 10
        append(stream, loadval(r3, 97));
        append(stream, output(r3));
        append(stream, loadval(r3, 10));
        append(stream, output(r3));
        append(stream, loadval(r2, 0));
        append(stream, loadval(r6, 10));
        append(stream, loadp(r4, r2, r6));
}

/* Invalid instruction */
void fail_2(Seq_T stream) {
        //output 'a', and then an invalid instruction
        append(stream, loadval(r3, 97));
        append(stream, output(r3));
        append(stream, invalid(r1, r2, r3));
        append(stream, halt());
}

/* Segmented load/store refers ot unmapped segment */
void fail_3(Seq_T stream) {
        append(stream, loadval(r3, 97));
        append(stream, output(r3));
        append(stream, loadval(r3, 10));
        append(stream, output(r3));
        append(stream, loadval(r2, 1));
        append(stream, loadval(r3, 0));
        append(stream, sload(r1, r2, r3)); //a = m[b][c]
        append(stream, loadval(r4, 105));
        append(stream, sstore(r2, r3, r4));  //m[a][b] = [c]
}

/* Segmented load / sotre refers outside the bounds of a mapped segment */
void fail_4(Seq_T stream) {
        append(stream, loadval(r3, 5));
        append(stream, map(r1, r2, r3)); //r2 = id, r3 = size .
        append(stream, loadval(r2, 1));
        append(stream, loadval(r3, 6));
        append(stream, sload(r1, r2, r3)); //a = m[b][c]
        append(stream, loadval(r4, 105));
        append(stream, sstore(r2, r3, r4));  //m[a][b] = [c]
}

/* Attempt to unmap segment 0 */
void fail_5(Seq_T stream) {
        append(stream, loadval(r3, 97));
        append(stream, output(r3));
        append(stream, loadval(r3, 10));
        append(stream, output(r3));
        append(stream, loadval(r3, 0));
        append(stream, unmap(r1, r2, r3));
}

/* Attempt to unmap segment not mapped */
void fail_6(Seq_T stream) {
        append(stream, loadval(r3, 1));
        append(stream, unmap(r1, r2, r3));
}

/* Attempt to divide by 0 */
void fail_7(Seq_T stream) {
        append(stream, loadval(r2, 5));
        append(stream, loadval(r3, 0));
        append(stream, division(r1, r2, r3));
        append(stream, output(r1));
}


/*######################## Instruction sets to submit *#############*/
void fact(Seq_T stream) {
        append(stream, loadval(r1, 1)); //holds result
        append(stream, loadval(r2, 1)); //multiplier
        append(stream, mult(r1, r1, r2));
        append(stream, loadval(r3, 1)); //accumulator
        append(stream, add(r2, r2, r3));

        //Check if r2 is 6
        append(stream, loadval(r4, 6));
        append(stream, bnand(r5, r2, r2));
        append(stream, bnand(r6, r4, r4));
        append(stream, bnand(r5, r5, r6));
        append(stream, bnand(r6, r2, r4));
        append(stream, bnand(r5, r5, r6));
        append(stream, bnand(r5, r5, r5));

        //If r2 is not 5, jump  (if r5 is not 0, terminate program)
        append(stream, loadval(r4, 17)); //For going back to the start
        append(stream, loadval(r6, 2)); //For going back to the start
        append(stream, cmov(r4, r6, r5));
        append(stream, loadval(r6, 0));
        append(stream, loadp(0, r6, r4));
        append(stream, output(r1));
        append(stream, halt());


}

//Program that reverses a string passed in
void reverse(Seq_T stream) {

        //Map segment of length 100
        append(stream, loadval(r3, 100));
        append(stream, map(r1, r2, r3));
        append(stream, loadval(r7, 0)); //holds num of chars
        append(stream, loadval(r6, 1)); //accumulator

        //Read input
        append(stream, input(0, 0, r1)); //load input to r1

       //Check if EOF is gotten
        append(stream, bnand(r2, r1, r1));

        //If not EOF, continue reading
        append(stream, loadval(r3, 15)); //jump print by default
        append(stream, loadval(r4, 11)); //jump to sstore
        append(stream, cmov(r3, r4, r2));
        append(stream, loadval(r5, 0));
        append(stream, loadp(0, r5, r3)); //jump to output command

        //Segment load
        append(stream, sstore(r6, r7, r1));
        append(stream, add(r7, r7, r6));
        append(stream, loadval(r4, 4));
        append(stream, loadp(0, r5, r4)); //jump to input command
        append(stream, halt());

        //Instead of the halt command above, we actually want to print the string
        ///TODO: how to subtract!!!

}

/* A program that reads accepts two input characters and checks if they are
the same */
void equal(Seq_T stream) {

      //Read in one char, if EOF, exit
      append(stream, loadval(r6, 73)); //jumps to halt by defualt
      append(stream, loadval(r7, 0)); //which program
      append(stream, input(0, 0, r1)); //load input to 1
      append(stream, bnand(r2, r1, r1));
      append(stream, loadval(r4, 7)); //jump to read second character by defualt
      append(stream, cmov(r6, r4, r2)); //if not EOF, r6 = 7
      append(stream, loadp(0, r7, r6)); //jump to output command
      append(stream, input(0, 0, r2)); //load input to 2
      append(stream, bnand(r3, r2, r2));
      append(stream, loadval(r6, 73)); //jumps to halt by defualt
      append(stream, loadval(r4, 13)); //jump test for equality
      append(stream, cmov(r6, r4, r3));
      append(stream, loadp(0, r7, r6)); //jump to output command
      append(stream, bnand(r3, r1, r1));
      append(stream, bnand(r4, r2, r2));
      append(stream, bnand(r5, r3, r4));
      append(stream, bnand(r6, r1, r2));
      append(stream, bnand(r5, r5, r6));
      append(stream, bnand(r5, r5, r5));
      append(stream, loadval(r6, 23)); //jumps to equals
      append(stream, loadval(r4, 45)); //jump to not equal
      append(stream, cmov(r6, r4, r5)); //if not equal
      append(stream, loadp(0, r7, r6)); //jump to output command

      append(stream, output(r1));
      append(stream, loadval(r3, 32));
      append(stream, output(r3));
      append(stream, loadval(r3, 101));
      append(stream, output(r3));
      append(stream, loadval(r3, 113));
      append(stream, output(r3));
      append(stream, loadval(r3, 117));
      append(stream, output(r3));
      append(stream, loadval(r3, 97));
      append(stream, output(r3));
      append(stream, loadval(r3, 108));
      append(stream, output(r3));
      append(stream, loadval(r3, 115));
      append(stream, output(r3));
      append(stream, loadval(r3, 32));
      append(stream, output(r3));
      append(stream, output(r2));
      append(stream, loadval(r3, 10));
      append(stream, output(r3));

      //jump to halt
      append(stream, loadval(r6, 73)); //jumps to halt
      append(stream, loadp(0, r7, r6)); //jump to halt



      append(stream, output(r1));
      append(stream, loadval(r3, 32));
      append(stream, output(r3));
      append(stream, loadval(r3, 110));
      append(stream, output(r3));
      append(stream, loadval(r3, 111));
      append(stream, output(r3));
      append(stream, loadval(r3, 116));
      append(stream, output(r3));
      append(stream, loadval(r3, 32));
      append(stream, output(r3));
      append(stream, loadval(r3, 101));
      append(stream, output(r3));
      append(stream, loadval(r3, 113));
      append(stream, output(r3));
      append(stream, loadval(r3, 117));
      append(stream, output(r3));
      append(stream, loadval(r3, 97));
      append(stream, output(r3));
      append(stream, loadval(r3, 108));
      append(stream, output(r3));
      append(stream, loadval(r3, 115));
      append(stream, output(r3));
      append(stream, loadval(r3, 32));
      append(stream, output(r3));
      append(stream, output(r2));
      append(stream, loadval(r3, 10));
      append(stream, output(r3));
      append(stream, halt());






}

void test_xnor(Seq_T stream) {
        append(stream, loadval(r4, 6));
        append(stream, loadval(r2, 6));
        append(stream, bnand(r5, r2, r2));
        append(stream, bnand(r6, r4, r4));
        append(stream, bnand(r5, r5, r6));
        append(stream, bnand(r6, r2, r4));
        append(stream, bnand(r5, r5, r6));
        append(stream, output(r5));
}

void fifty_mil(Seq_T stream) {

        append(stream, loadval(r1, 0));
        for(int i = 0; i < 49999999; i++){
                append(stream, bnand(r1, r1, r1));
        }
}

void map_test(Seq_T stream)
{

        //Test mapping segments
        for(int i = 0; i < 10; i++) {
                append(stream, loadval(r3, i));
                append(stream, map(r1, r2, r3));
                append(stream, output(r2));
        }

        // //Unmap segments 7, 3, 5, and 2
        // int arr[] = {7, 3, 5, 2};
        // for(int i = 0; i < 4; i++) {
        //         append(stream, loadval(r3, arr[i]));
        //         append(stream, unmap(r1, r2, r3));
        // }

        // //Should have been assigned 7, 3, 5, 2
        // append(stream, loadval(r3, 5)); //sizes of 5 here
        // for(int i = 0; i < 4; i++) {
        //         append(stream, map(r1, r2, r3));
        //         append(stream, output(r2));
        // }

        // //Map two more
        // for(int i = 0; i < 4; i++) {
        //         append(stream, loadval(r3, 6));
        //         append(stream, map(r1, r2, r3));
        // }
        //
        //
        // //Mapping and unmapping back to back
        //  for(int i = 0; i < 5; i++) {
        //         append(stream, loadval(r3, 6));
        //         append(stream, map(r1, r2, r3));
        //         append(stream, unmap(r1, r2, r3));
        // }
        append(stream, halt());
}
