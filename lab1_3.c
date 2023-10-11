#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Wrong arguments. Enter paths of input and output files.\n");
        return 1;
    }

    FILE* input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        printf("Could not open the file for reading.\n");
        return 2;
    }

    FILE* output_file = fopen(argv[2], "w");
    if (output_file == NULL) {
        printf("Could not open the file for writing.\n");
        fclose(input_file);
        return 3;
    }

    unsigned char buffer[1024];
    size_t bytesRead;
    size_t bytesWritten;

    while ((bytesRead = fread(buffer, sizeof(unsigned char), 1024, input_file)) > 0) {
        bytesWritten = fwrite(buffer, sizeof(unsigned char), bytesRead, output_file);
        if (bytesWritten != bytesRead) {
            printf("Error of writing data.\n");
            fclose(input_file);
            fclose(output_file);
            return 4;
        }
    }

    fclose(input_file);
    fclose(output_file);

    return 0;
}