#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"
#define BUFF_SIZE 8192
#define rot_shift ('Z'-'A' + 1)/2

char buff[BUFF_SIZE+1];

void rot13(FILE *fin, FILE *fout, int n, int argc, char *argv[]) {
    int read = 0, i, numtasks, rank, elements_per_processor;
    char* process_arr;
    char* result_arr;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Status status;
    do {
        memset(buff, 0, BUFF_SIZE+1);
        read = fread(&buff, sizeof(char), BUFF_SIZE, fin);
        elements_per_processor = read / numtasks + (read % numtasks != 0);
        process_arr = calloc (elements_per_processor, sizeof(char));
        MPI_Scatter(buff, elements_per_processor, MPI_CHAR, process_arr, elements_per_processor, MPI_CHAR, 0, MPI_COMM_WORLD);
        for (i = 0; i < elements_per_processor; i++) {
            if (process_arr[i] >= 'A' && process_arr[i] <= 'Z') {
                // Uppercase letters (A-Z)
                if (process_arr[i] <= 'M') {
                    process_arr[i] += rot_shift;
                } else {
                    process_arr[i] -= rot_shift;
                }
            } else if (process_arr[i] >= 'a' && process_arr[i] <= 'z') {
                // Lowercase letters (a-z)
                if (process_arr[i] <= 'm') {
                    process_arr[i] += rot_shift;
                } else {
                    process_arr[i] -= rot_shift;
                }
            }
        }
        if (rank == 0) {
            result_arr = calloc (read, sizeof(char));
        }
 
        MPI_Gather(process_arr, elements_per_processor, MPI_CHAR, result_arr, elements_per_processor, MPI_CHAR, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            fprintf(fout, "%.*s", read, result_arr);
        }
        if (rank == 0) {
            free(result_arr);
        }
        free(process_arr);
        
    } while (read == BUFF_SIZE);
    MPI_Finalize();
}

int main (int argc, char *argv[])
{
    char input_file_name[100] = "in/in";
    strcat(input_file_name, argv[1]);
    strcat(input_file_name, ".txt");

    char output_file_name[100] = "out/in";
    strcat(output_file_name, argv[1]);
    strcat(output_file_name, "/P");
    strcat(output_file_name, argv[2]);
    strcat(output_file_name, "/mpi.txt");

    FILE *fin = fopen(input_file_name, "r");
    fseek(fin, 0, SEEK_END);
    int n = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    FILE *fout = fopen(output_file_name, "w");
    rot13(fin, fout, n, argc, argv);
    fclose(fin);
    fclose(fout);
}
