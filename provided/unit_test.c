

#include <stdlib.h> 
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <seq.h>
#include <mem.h>
#include <assert.h>

#include "um_reader.h"
#include "unpack.h"
#include "execute.h"

void test_reader();
void test_unpack();
void print_all_chars();
void test_unmap();
void test_load_p();

int main() {
   // test_reader();
   // test_unpack();
   // print_all_chars();
   test_load_p();
}

void test_load_p() {
    struct stat buf;
    stat("umbin/load/load_p.um", &buf);
    FILE *fp = fopen("umbin/load/load_p.um", "rb");
    Seq_T seg0 = reader(fp, buf.st_size);
    fclose(fp);
    Seq_T segments = Seq_new(1);
    Seq_addhi(segments, seg0); 

    //Manually create segment 1
    FILE *fp1 = fopen("umbin/load/print_hello.um", "rb");
    struct stat buf2;
    stat("umbin/load/print_hello.um", &buf2);
    Seq_T seg1 = reader(fp1, buf2.st_size); 

    for(int i = 0; i < Seq_length(seg1); i++) {
        int code = (int) (uintptr_t) Seq_get(seg1, i);
      //  printf("%x\n", code);
       (void) code;
    }
    Seq_addhi(segments, seg1);
    
    Seq_T ids = Seq_new(1);
    int counter = 0;
    uint32_t *registers = malloc(8 * sizeof(uint32_t));
    assert(registers != NULL);

    while (counter < Seq_length(Seq_get(segments, 0))) {
        uint32_t word = (uint32_t) (uintptr_t) Seq_get(Seq_get(segments, 0), counter);
        Instruction instruction = unpack(word);

        if (instruction -> opcode == HALT) {
            break;
        }

        execute(instruction, segments, ids, registers, &counter);
        
        if (instruction -> opcode != LOADP) {
            counter++;
        }
        FREE(instruction);

    }

    (void) registers;
    (void) counter;
    (void) ids;


}

void test_unmap() 
{
    char *file = "umbin/other/hello.um";
    struct stat buf;
    stat(file, &buf);
    FILE *fp = fopen(file, "rb");
    Seq_T seg0 = reader(fp, buf.st_size);
    Seq_T segments = Seq_new(1);
    Seq_addhi(segments, seg0); 

    //Holds the ids
    Seq_T ids = Seq_new(1);

    //Map these new segments 
    uint32_t rB = 0;
    map(1, segments, &rB, 5); // new_id, segments, *rB, rC
    map(2, segments, &rB, 0); // new_id, segments, *rB, rC
    map(3, segments, &rB, 100); // new_id, segments, *rB, rC
    map(4, segments, &rB, 34); // new_id, segments, *rB, rC

    //1. Test unmapping one thing
    unmap(ids, segments, 1); //ids, segments, ids
    assert(Seq_length(ids) == 1);
    uint32_t id0 = (uint32_t) (uintptr_t) Seq_get(ids, 0);
    assert(id0 == 1);

    //2. Test unmappping everything 
    for(int i = 2; i <= 4; i++) {
        unmap(ids, segments, i); 
    }

    assert(Seq_length(ids) == 4);
    for(int i = 0; i < 4; i++) {
        uint32_t id = (uint32_t) (uintptr_t) Seq_get(ids, i);
        printf("%u ", id);
    }
    printf("\n");

    //3. Test unmapping twice
    unmap(ids, segments, 1); 

    //4. Test mapping segment 0
    unmap(ids, segments, 0); 

    //5. Test remapping segment 1
    map(1, segments, &rB, 5); // new_id, segments, *rB, rC
    Seq_T new_map = (Seq_T) Seq_get(segments, 1);
    assert(Seq_length(new_map) == 5);



    
}


void test_map() {

    char *file = "umbin/other/hello.um";
    struct stat buf;
    stat(file, &buf);
    FILE *fp = fopen(file, "rb");
    Seq_T seg0 = reader(fp, buf.st_size);
    Seq_T segments = Seq_new(1);
    Seq_addhi(segments, seg0); 

    //1. Test case mapping new segments
    uint32_t rB = 0;
    map(1, segments, &rB, 5); // new_id, segments, *rB, rC
    assert(Seq_length(segments) == 2);
    Seq_T new_map = (Seq_T) Seq_get(segments, 1);
    assert(Seq_length(new_map) == 5);
    uint32_t value = (uint32_t) (uintptr_t) Seq_get(new_map, 3);
    assert(value == 0);
    assert(rB == 1);;

    map(2, segments, &rB, 0); // new_id, segments, *rB, rC
    assert(Seq_length(segments) == 3);
    Seq_T new_map2 = (Seq_T) Seq_get(segments, 2);
    assert(Seq_length(new_map2) == 0);
    assert(rB == 2);

    map(3, segments, &rB, 100); // new_id, segments, *rB, rC
    assert(Seq_length(segments) == 4);
    Seq_T new_map3 = (Seq_T) Seq_get(segments, 3);
    assert(Seq_length(new_map3) == 100);
    assert(rB == 3);

    //2. Try 

}

void test_reader()
{
    char *file = "umbin/hello.um";
    struct stat buf;
    stat(file, &buf);
    int file_size = buf.st_size/4;
    FILE *fp = fopen(file, "rb");
    Seq_T seg0 = reader(fp, buf.st_size);

    printf("num instructions: %d\n", Seq_length(seg0));
    for(int i = 0; i < file_size; i++) {
        int value = (int) (uintptr_t) Seq_get(seg0, i);
        printf("%x\n", (int) value);
    }
    (void) seg0;
}


void test_unpack()
{

    char *file = "umbin/input/input_2.txt";
    struct stat buf;
    stat(file, &buf);
    printf("File size: %ld", buf.st_size);
    // int file_size = buf.st_size/4;
    // FILE *fp = fopen(file, "rb");
    // Seq_T seg0 = reader(fp, buf.st_size);

    // printf("num instructions: %d\n", Seq_length(seg0));
    // for(int i = 0; i < file_size; i++) {
    //     int value = (int) (uintptr_t) Seq_get(seg0, i);
    //     Instruction instructions = unpack(value);
    //     printf("opcode: %d rA: %d rB: %d rC: %d value: %d\n",
    //             (int) instructions -> opcode, (int) instructions -> rA, 
    //             (int) instructions -> rB, (int) instructions -> rC,
    //             (int) instructions -> value);
    // }
    

}

void print_all_chars() {
    for(int i = 0; i <= 255; i++){
        printf("%c", i);
    }
}
