#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"
#define BUFF_SIZE 8192
#define rot_shift ('Z'-'A' + 1)/2

char buff[BUFF_SIZE+1];
char* process_arr;

struct startEnd {
    int tread_id;
    int start;
    int end;
};

void *rot13_pthread(void *arg) {
    int i;
  	struct startEnd *limits = (struct startEnd *) arg;
    for (i = limits->start; i < limits->end; i++) {
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
  	pthread_exit(NULL);
}

void rot13(FILE *fin, FILE *fout, int n, int argc, char *argv[]) {
    int read = 0, i, numtasks, rank, elements_per_processor;
    char* result_arr;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    long cores = atoi(argv[2]);
    pthread_t threads[cores];
    long id;
    int r;
    void *status;
    int P = (int) cores;
    int N;
    struct startEnd *limit = malloc(sizeof(struct startEnd) * P);
    do {
        
        read = fread(&buff, sizeof(char), BUFF_SIZE, fin);
        elements_per_processor = read / numtasks + (read % numtasks != 0);
        process_arr = calloc (elements_per_processor, sizeof(char));
        MPI_Scatter(buff, elements_per_processor, MPI_CHAR, process_arr, elements_per_processor, MPI_CHAR, 0, MPI_COMM_WORLD);
        N = elements_per_processor;
        
        for (id = 0; id < cores; id++) {
            limit[id].tread_id = id;
            limit[id].start = id * (double)N / P;
            if ((id + 1) * (double)N / P < N)
            limit[id].end = (id + 1) * (double)N / P;
            else
            limit[id].end = N;
        }

        for (id = 0; id < cores; id++) {
            r = pthread_create(&threads[id], NULL, rot13_pthread, &limit[id]);
            if (r) {
                printf("Eroare la crearea thread-ului %ld\n", id);
                exit(-1);
            }
        }

        for (id = 0; id < cores; id++) {
            
            r = pthread_join(threads[id], &status);

            if (r) {
                printf("Eroare la asteptarea thread-ului %ld\n", id);
                exit(-1);
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
    strcat(output_file_name, "/mpi_pthreads.txt");

    FILE *fin = fopen(input_file_name, "r");
    fseek(fin, 0, SEEK_END);
    int n = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    FILE *fout = fopen(output_file_name, "w");
    rot13(fin, fout, n, argc, argv);
    fclose(fin);
    fclose(fout);
}
