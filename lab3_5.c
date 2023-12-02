#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

enum Status {
    SUCCESS,
    WRONG_NAME_OF_CATALOG,
    ERROR_OF_OPENNING_FILE,
    ERROR_GETTING_INFO,
};

enum Status show_files(const char *dirname) {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(dirname);

    if (dir == NULL){
        return WRONG_NAME_OF_CATALOG;
    } 

    while ((entry = readdir(dir)) != NULL) {
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        struct stat info;
        if (stat(path, &info) == -1) {
            closedir(dir);
            return ERROR_GETTING_INFO;
        }

        printf("%-20s| ", entry->d_name);
        if (S_ISDIR(info.st_mode)){
            printf("Directory");
        }
        else if (S_ISLNK(info.st_mode)){
            printf("%9s", "LINK");
        } 
        else if (S_ISREG(info.st_mode)){
            printf("%9s", "FILE");
        } 
        else{
            printf("Unknown type\n");
        } 
        printf("| First disk address of a file : %d\n", info.st_dev);
    }

    closedir(dir);
    return SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Use : %s <some catalogs>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        printf("Information about catalog %s:\n", argv[i]);
        enum Status status = show_files(argv[i]);
        
        switch (status) {
            case SUCCESS:
                break;
            case WRONG_NAME_OF_CATALOG:
                printf("Wrong name of catalog or there is no such directory.\n");
                break;
            case ERROR_OF_OPENNING_FILE:
                printf("Cannot open the catalog.\n");
                break;
            case ERROR_GETTING_INFO:
                printf("Error getting file information.\n");
                break;
            default:
                printf("Unknown error.\n");
        }
        printf("\n\n");
    }

    return 0;
}