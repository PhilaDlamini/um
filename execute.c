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
// void free_sequences(Seq_T segments, Seq_T ids)
// {
//     assert(segments != NULL && ids != NULL);
//     int len = Seq_length(segments);
//     for (int i = len - 1; i >= 0; i--) {
//         Seq_T curr = (Seq_T) Seq_get(segments, i);
//         if (curr != NULL) {
//             Seq_free(&curr);
//         }
//     }
//     Seq_free(&segments);
//     Seq_free(&ids);
// }

void um(uint32_t *seg0, int count)
{

    uint32_t *registers = malloc(8 * sizeof(uint32_t));
    assert(registers != NULL);

    for (int i = 0; i < 8; i++) {
        registers[i] = 0;
    }

    uint32_t counter = 0;
    uint32_t word, opcode, rA, rB, rC, value, wordrA, wordrB, wordrC, wordVal;

    int ids_s = 0;
    int size = 1;
    int ids_cap, capacity;
    ids_cap = capacity = 10;
    uint32_t **spine = malloc(capacity * sizeof(*spine));
    uint32_t *segment_sizes = malloc(capacity * sizeof(*segment_sizes));
    uint32_t *ids = malloc(ids_cap * sizeof(*ids));
    assert(ids != NULL && spine != NULL);

    //Put segment 0
    spine[0] = seg0;
    segment_sizes[0] = count;

   // int num_instr = count;
    while (counter < segment_sizes[0]) {

        uint32_t *seg0 = spine[0];
        word = seg0[counter];
        wordrA = wordrB = wordrC = wordVal = word;

        /* unpack in place */
        opcode = word >> 28;

        if(opcode == LV) {
          value = (wordrA << 7) >> 7;
          rA = (wordVal << 4) >> 29;
          rB = 0;
          rC = 0;
        } else {
          rA = (wordrA << 23)  >> 29;
          rB = (wordrB << 26)  >> 29;
          rC = (wordrC << 29)  >> 29;
          value = 0;
        }

        //(word << (64 - (lsb + width)))  >> (64 - width)//shl

      //  printf("op: %d, rA: %d, rB: %d, rC: %d, value: %d\n",   instruction -> opcode,   instruction -> rA, instruction->rB, instruction->rC, instruction->value);

        if (opcode == HALT) {
            break;
        }

        //execute(value, rA, rB, rC, opcode, segments, ids, registers, &counter);
        switch(opcode) {
            case CMOV:
            {
                if (registers[rC] != 0) {
                  registers[rA] = registers[rB];
                }
                break;
            }
            case SLOAD:
              {
                  uint32_t *segment = spine[registers[rB]];
                  uint32_t value = segment[registers[rC]];
                  registers[rA] = value;
                  break;
              }
            case SSTORE:
            {
              uint32_t *segment = spine[registers[rA]];
              segment[registers[rB]] = registers[rC];
              break;
            }
            case ADD:
            {
              registers[rA] = registers[rB] + registers[rC];
              break;
            }
            case MUL:
              {
                registers[rA] = registers[rB] * registers[rC];
                break;
              }
            case DIV:
              {
                assert(registers[rC] != 0);
                registers[rA] = registers[rB] / registers[rC];
                break;
              }

            case NAND:
                {
                  registers[rA] = ~(registers[rB] & registers[rC]);
                break;
              }
            case ACTIVATE:
            {
              //  assert(registers[rC] != 0);

                //If surpassed load factor
                if((float)size / capacity >= 0.7) {

                  // printf("About to expand spine\n");
                  // printf("Old cap: %d, old size: %d\n", capacity, size);

                  capacity = 2 * capacity + 2;
                  uint32_t **temp1= malloc(capacity * sizeof(*temp1));
                  uint32_t *temp2 = malloc(capacity * sizeof(*temp2));
                  for(int i = 0; i < size; i++) {
                    temp1[i] = spine[i];
                    temp2[i] = segment_sizes[i];
                  }

                  free(spine);
                  free(segment_sizes);

                  spine = temp1;
                  segment_sizes = temp2;

                  //printf("new cap: %d, new size: %d\n", capacity, size);
                }

                //Assign index
                int index = -1;

                if(ids_s > 0) {
                 // printf("Re using index\n");
                  index = ids[--ids_s];
                } else {
                  index = size++;
                }

                //Initialize and insert the new segment
                uint32_t *segment = malloc(registers[rC] * sizeof(*segment));
                for(int i = 0; i < (int) registers[rC]; i++)
                  segment[i] = 0;

                segment_sizes[index] = registers[rC];
                spine[index] = segment;
                registers[rB] = index;

                break;
            }
            case INACTIVATE:
            {

              //If surpassed load factor
                if((float)ids_s / ids_cap >= 0.7) {
                  ids_cap = 2 * ids_cap + 2;
                  uint32_t *temp = malloc(ids_cap * sizeof(*temp));
                  for(int i = 0; i < ids_s; i++)
                    temp[i] = ids[i];

                  free(ids);
                  ids = temp;
                }

                ids[ids_s++] = registers[rC];
                segment_sizes[registers[rC]] = 0;
                //free in spine
                uint32_t *segment = spine[registers[rC]];
                free(segment);
                spine[registers[rC]] = NULL;
                break;
            }
            case OUT:
            {
                assert((int) registers[rC] >= 0 &&  (int) registers[rC] <= 255);
               // printf("id assigned %d\n", (int) registers[rC]);
                printf("%c", (int) registers[rC]);
                break;
            }
            case IN:
              {
                  char c = getc(stdin);
                  if (c != EOF) {
                      registers[rC] =  c;
                  } else {
                      registers[rC] = ~0;
                  }
                  break;
              }
            case LOADP:
            {
                if (registers[rB] != 0) {
                  uint32_t *segment = spine[registers[rB]];
                  //uint32_t *duplicate = sizeof(segment)/sizeof(uint32_t);
                  int dup_size = segment_sizes[registers[rB]];
                  uint32_t *duplicate = malloc(dup_size * sizeof(*duplicate));
                  assert(duplicate != NULL);

                  for(int i = 0; i < dup_size; i++){
                      duplicate[i] = segment[i];
                  }

                  uint32_t *segment0 = spine[0];
                  free(segment0);
                  spine[0] = duplicate;
                  segment_sizes[0] = segment_sizes[registers[rB]];

              //    Seq_T segB = Seq_get(segments, registers[rB]);

                  // Seq_T duplicate = Seq_new(Seq_length(segB));
                  // assert(duplicate != NULL);
                  //
                  // //Duplicate segment
                  // for(int i = 0; i < Seq_length(segB); i++) {
                  //     Seq_addhi(duplicate, Seq_get(segB, i));
                  // }
                  //
                  // //Free segment 0
                  // Seq_T seg0 = (Seq_T) Seq_get(segments, 0);
                  // Seq_free(&seg0);
                  //
                  // // loads duplicated segment to be the new segment 0
                  // Seq_put(segments, 0, duplicate);
                }
                counter = registers[rC];
                break;
            }
            case LV:
                registers[rA] = value;
                break;
        }

        if (opcode != LOADP) {
            counter++;
        }

    }

    free(registers);
    //free_sequences(segments, ids); //

    //Free everything
    for(int i = 0; i < size; i++){
      if(spine[i] != NULL) free(spine[i]);
    }
    free(spine);
    free(ids);
    free(segment_sizes);
}
