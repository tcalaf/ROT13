#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define BUFF_SIZE 8192
#define rot_shift ('Z'-'A' + 1)/2

char buff[BUFF_SIZE+1];

struct startEnd {
    int tread_id;
    int start;
    int end;
};

void *rot13(void *arg) {
    int i;
  	struct startEnd *limits = (struct startEnd *) arg;
    for (i = limits->start; i < limits->end; i++) {
        if (buff[i] >= 'A' && buff[i] <= 'Z') {
            // Uppercase letters (A-Z)
            if (buff[i] <= 'M') {
                buff[i] += rot_shift;
            } else {
                buff[i] -= rot_shift;
            }
        } else if (buff[i] >= 'a' && buff[i] <= 'z') {
            // Lowercase letters (a-z)
            if (buff[i] <= 'm') {
                buff[i] += rot_shift;
            } else {
                buff[i] -= rot_shift;
            }
        }
    }
  	pthread_exit(NULL);
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
    strcat(output_file_name, "/pthreads.txt");

    long cores = atoi(argv[2]);
    pthread_t threads[cores];
    long id;
    int r, i;
    void *status;

    FILE *fin = fopen(input_file_name, "r");
    fseek(fin, 0, SEEK_END);
    int n = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    FILE *fout = fopen(output_file_name, "w");
    int P = (int) cores;
    int N;
    struct startEnd *limit = malloc(sizeof(struct startEnd) * P);

    do {
        N = fread(&buff, sizeof(char), BUFF_SIZE, fin);
        
        for (id = 0; id < cores; id++) {
            limit[id].tread_id = id;
            limit[id].start = id * (double)N / P;
            if ((id + 1) * (double)N / P < N)
            limit[id].end = (id + 1) * (double)N / P;
            else
            limit[id].end = N;
        }

        for (id = 0; id < cores; id++) {
            r = pthread_create(&threads[id], NULL, rot13, &limit[id]);
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

        fprintf(fout, "%.*s", N, buff);
    } while (N == BUFF_SIZE);
    
    fclose(fin);
    fclose(fout);
    free(limit);
}