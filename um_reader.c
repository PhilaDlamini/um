/*
*                       um_reader.c
*
*
*   Summary: um_reader.c is the implementation for um_reader.h. um_reader.c
*            holds the function definition for reader() which reads
*            through the provided .um file.
*
*   Authors: vmccab01 and pdlami01
*/

#include <assert.h>

#include "um_reader.h"
#include "bitpack.h"

const int num_chars = 4;
const int bits = 8;
const int lsb = 24;


/*
* Name: reader
* Summary: reads through the provided .um file and reads the codewords into
*          the created segment 0.
* Input: fp is a valid non null file pointer, bytes is the size of the file
*        provided.
* Output: Returns a sequence (segment 0)
* Side Effects: Allocates memory for segment 0
* Error Conditions: CRE if not enough memory to create segment 0
*/
uint32_t *reader(FILE *fp, int bytes)
{
    int curr_instruction = 0;
    int num_instructions = bytes / 4;
    uint32_t *seg0 = malloc(num_instructions * sizeof(*seg0));


    assert(seg0 != NULL);
    uint32_t word;
    while (curr_instruction < num_instructions) {
        word = 0;
        for (int i = 0; i < 4; i++) {
            unsigned char c = getc(fp);
            word = Bitpack_newu(word, 8, 24 - (i * 8), c);
        }
        seg0[curr_instruction++] = word;
        
    }




    return seg0;
}
