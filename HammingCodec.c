/**
 * @file HammingCodec.c
 * @brief Use [8,4] Hamming Code to encode your data
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "fec.h"

/**
 * @brief process stuff from an input stream, output to another stream.
 */
void process(FILE * input, FILE * output);

/**
 * @brief process stuff from a file.
 */
int process_file(char *filename);

/**
 * @brief print out help statements
 */
void print_help();

/**
 * @brief decode mode flag indicator
 */
static int DECODE = 0;

/**
 * @brief file position indicator
 */
long FILE_POS = 0;

int main(int argc, char *const *argv)
{
    int c;
    int main_loop = 0;

    while ((c = getopt(argc, argv, "dh")) != -1) {
        switch (c) {
            case 'd':
                DECODE = 1;
                break;
            case 'h':
                print_help();
                break;
        }
    }

    int index;
    int r = 0;
    for (index = optind; index < argc; index++) {
        main_loop = 1;
        if ((r = process_file(argv[index])) != 0) {
            fprintf(stderr, "Error occured while processing file %s.\n",
                    argv[index]);
        };
    }
    if (!main_loop) {
        process(stdin, stdout);
        if (ferror(stdin) || ferror(stdout)) {
            fprintf(stderr, "Error occured while processing the file.\n");
            exit(1);
        }
    }

    exit(r);
}

int process_file(char *filename)
{
    int c;
    FILE *input;
    FILE *tmp;

    if ((input = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Cannot open input file %s: %s\n",
                filename, strerror(errno));
        return 2;
    }

    if ((tmp = tmpfile()) == NULL) {
        fprintf(stderr, "Could not create the temporary file: %s\n",
                strerror(errno));
        return 2;
    }

    /* The actual processing function */
    process(input, tmp);

    if (ferror(input) || ferror(tmp)) {
        fprintf(stderr, "Error occured while processing the file.\n");
        return 2;
    }

    if (fclose(input)) {
        fprintf(stderr, "Could not close the input stream: %s\n",
                strerror(errno));
        return 2;
    }

    /* Not very safe, but hey, YOLO */
    char *new_filename;
    if (!DECODE) {
        new_filename = (char*) malloc(strlen(filename) + 5);
        strcpy(new_filename, filename);
        strcat(new_filename, ".fec");
    } else {
        int old_strlen = strlen(filename);
        new_filename = (char*) malloc(old_strlen);
        strncpy(new_filename, filename, old_strlen - 3);
        new_filename[old_strlen - 4] = '\0';
    }

    if ((input = fopen(new_filename, "w")) == NULL) {
        fprintf(stderr, "Cannot open input file %s: %s\n",
                new_filename, strerror(errno));
        return 2;
    }

    /* actual copying process */
    rewind(tmp);
    while ((c = fgetc(tmp)) != EOF) {
        fputc(c, input);
    }

    if (ferror(input) || ferror(tmp)) {
        fprintf(stderr, "Error occured while processing the file.\n");
        return 1;
    }

    if (fclose(tmp)) {
        fprintf(stderr, "Could not close the temporary stream: %s\n",
                strerror(errno));
        return 1;
    }

    return 0;
}

void process(FILE * input, FILE * output)
{
    int c;
    uint8_t byteA, byteB;
    /* Loop through all characters */
    while ((c = fgetc(input)) != EOF) {
        /* ------------------ Put stuff to the output -----------------------*/
        if (!DECODE) {
            encode(c, &byteA, &byteB);
            fputc(byteA, output);
            fputc(byteB, output);
        } else {
            FILE_POS = ftell(input);
            int d = fgetc(input);
            if (d == EOF) {
                fprintf(stderr, "Unexpectedly reached the end of file!\n");
                return;
            } else {
                fputc(decode(c ,d), output);
            }
        }
    }
}

void print_help()
{
    puts("Usage:\tfec [options...] [input files...]");
    puts("Encode/decode your file using [8, 4] extended binary Hamming Code");
    puts("");
    puts("Options");
    puts("\t-d\t\tdecode a file");
    puts("\t-h\t\tshow this help text and exit");
    puts("");
    puts("In encoding mode the output filename is the input file name");
    puts("appended with \".fec\"");
    puts("");
    puts("In decoding mode, the \".fec\" extension is removed, and the");
    puts("existing file will be overwritten!");
    exit(0);
}
