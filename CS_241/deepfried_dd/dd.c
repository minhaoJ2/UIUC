/**
 * deepfried_dd
 * CS 241 - Fall 2020
 * lab partner: hangel2, jingz15, minhaoj2
 */
#include "format.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

#define MAX_FILE_NAME_SIZE 100

static struct timespec start;
static int print_status_report_flag;

void sigusr_handler(int sig) {
    print_status_report_flag = 1;
}

int main(int argc, char **argv) {
    char *input_file = NULL, *output_file = NULL;
    size_t block_size = 512;
    int total_block = 0, skip_block_input = 0, skip_block_output = 0;

    // signal handle related
    print_status_report_flag = 0;
    signal(SIGUSR1, sigusr_handler);

    int opt;
    while ((opt = getopt(argc, argv, "i:o:b:c:p:k:")) != -1) {
        switch (opt)
        {
        case 'i':
            input_file = optarg;
            break;
        case 'o':
            output_file = optarg;
            break;
        case 'b':
            block_size = (size_t) atoi(optarg);
            break;
        case 'c':
            total_block = atoi(optarg);
            break;
        case 'p':
            skip_block_input = atoi(optarg);
            break;
        case 'k':
            skip_block_output = atoi(optarg);
            break;
        default:
            exit(1);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    /*
    set input and output stream
     */
    FILE *input = stdin, *output = stdout;
    if (input_file != NULL) {
        input = fopen(input_file, "r");
        if (!input) {
            print_invalid_input(input_file);
            exit(1);
        }
    }
    if (output_file != NULL) {
        output=fopen(output_file,"a+");
        if (!output){
            fclose(input);
            print_invalid_output(output_file);
            exit(1);
        }
        fclose(output);
        output = fopen(output_file, "r+");
        if (!output) {
            fclose(input);
            print_invalid_output(output_file);
            exit(1);
        }
    }

    if (skip_block_input != 0) {
        fseek(input, skip_block_input * block_size, SEEK_SET);
    }
    if (skip_block_output != 0) {
        fseek(output, skip_block_output * block_size, SEEK_SET);
    }

    int full_in_count = 0, partial_in_count = 0, total_bytes = 0;
    int full_out_count = 0, partial_out_count = 0;

    if (total_block != 0) {
        for (int i = 0; i < total_block; i++) {
            if (print_status_report_flag) {
                struct timespec curr;
                clock_gettime(CLOCK_MONOTONIC, &curr);
                double dur = curr.tv_sec - start.tv_sec + (curr.tv_nsec - start.tv_nsec) / 1000000000.0;
                print_status_report(full_in_count, partial_in_count, full_out_count, partial_out_count, total_bytes, dur);
                print_status_report_flag = 0;
            }
            char buffer[block_size];
            size_t bytesread = fread(buffer, 1, block_size, input);
            size_t byteswrite = fwrite(buffer, 1, bytesread, output);

            if (bytesread == block_size)
                full_in_count++;
            if (bytesread != 0 && bytesread != block_size)
                partial_in_count++;
            if (byteswrite == block_size)
                full_out_count++;
            if (byteswrite != 0 && byteswrite != block_size)
                partial_out_count++;
            total_bytes += byteswrite;
        }
    } else {
        while (!feof(input)) {
            if (print_status_report_flag) {
                struct timespec curr;
                clock_gettime(CLOCK_MONOTONIC, &curr);
                double dur = curr.tv_sec - start.tv_sec + (curr.tv_nsec - start.tv_nsec) / 1000000000.0;
                print_status_report(full_in_count, partial_in_count, full_out_count, partial_out_count, total_bytes, dur);
                print_status_report_flag = 0;
            }
            char buffer[block_size];
            size_t bytesread = fread(buffer, 1, block_size, input);
            size_t byteswrite = fwrite(buffer, 1, bytesread, output);
            if (bytesread == block_size)
                full_in_count++;
            if (bytesread != 0 && bytesread != block_size)
                partial_in_count++;
            if (byteswrite == block_size)
                full_out_count++;
            if (byteswrite != 0 && byteswrite != block_size)
                partial_out_count++;
            total_bytes += byteswrite;
        }
    }

    // TODO: to print out the information

    fclose(input);
    fclose(output);
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    double dur = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    print_status_report(full_in_count, partial_in_count, full_out_count, partial_out_count, total_bytes, dur);

    return 0;
}