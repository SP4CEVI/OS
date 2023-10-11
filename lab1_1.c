#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Enter the path to the file.\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "wb");
    if (file == NULL) {
        printf("Could not open the file for writingи.\n");
        return 2;
    }

    unsigned char bytes[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    size_t len = sizeof(bytes) / sizeof(bytes[0]);

    fwrite(bytes, sizeof(unsigned char), len, file);
    fclose(file);



    file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("Could not open the file for reading.\n");
        return 3;
    }

    unsigned char buffer;
    while (fread(&buffer, sizeof(unsigned char), 1, file) == 1) {
        printf("File contents (bytes):\n");
        printf("%d\n", buffer);

        printf("Structure FILE:\n");
        
        printf("\t_flag: 0x%x\n", file->_flag);
        printf("\t_cnt: 0x%x\n", file->_cnt);
        printf("\t_ptr: %p\n", file->_ptr);
        printf("\t_tmpfname: %p\n", file->_tmpfname);
        printf("\t_base: %p\n", file->_base);
        printf("\t_file: %d\n", file->_file);
        printf("\t_bufsiz: 0x%x\n", file->_bufsiz);
        printf("\t_charbuf: %ld\n", file->_charbuf);
    }

    fclose(file);

    file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("Could not open the file for reading.\n");
        return 3;
    }

    if (fseek(file, 3, SEEK_SET) != 0) {
        printf("Failed to execute fseek.\n");
        fclose(file);
        return 4;
    }

    unsigned char buffer_long[4];
    if (fread(buffer_long, sizeof(unsigned char), 4, file) != 4) {
        printf("Failed to execute fread.\n");
        fclose(file);
        return 5;
    }

    printf("File contents (bytes):\n");
    for (int i = 0; i < 4; i += 1) {
        printf("%u ", buffer_long[i]);
    }
    printf("\n");

    fclose(file);

    return 0;
}