#include <stdio.h>
#include <string.h>
#include <time.h>
#define BUFF_SIZE 8192
#define rot_shift ('Z'-'A' + 1)/2

char buff[BUFF_SIZE+1];

void rot13(FILE *fin, FILE *fout, int n) {
    int read, i;
    do {
        read = fread(&buff, sizeof(char), BUFF_SIZE, fin);
        for (i=0; i<read; i++) {
        
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
        fprintf(fout, "%.*s", read, buff);
    } while (read == BUFF_SIZE);
}

int main (int argc, char *argv[])
{
    char input_file_name[100] = "in/in";
    strcat(input_file_name, argv[1]);
    strcat(input_file_name, ".txt");

    char output_file_name[100] = "out/in";
    strcat(output_file_name, argv[1]);
    strcat(output_file_name, "/serial.txt");
    
    FILE *fin = fopen(input_file_name, "r");
    fseek(fin, 0, SEEK_END);
    int n = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    FILE *fout = fopen(output_file_name, "w");
    rot13(fin, fout, n);
    fclose(fin);
    fclose(fout);
    return 0;
}
