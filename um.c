/*
*                       um.c
*
*   
*   Summary: um.c is file that holds our main.
*
*   Authors: vmccab01 and pdlami01
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h>


#include "seq.h"
#include "um_reader.h"
#include "execute.h"

int main(int argc, char *argv[]) {

    if (argc == 2) {

        struct stat buf;
        stat(argv[1], &buf);
        FILE *fp = fopen(argv[1], "rb");

        if(fp == NULL) {
            fprintf(stderr, "Could not open %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        
        Seq_T seg0 = reader(fp, buf.st_size);
        um(seg0);

        fclose(fp);
        return EXIT_SUCCESS;
    }
    
    fprintf(stderr, "Usage: ./um <program.um> \n");
    exit(EXIT_FAILURE);
}