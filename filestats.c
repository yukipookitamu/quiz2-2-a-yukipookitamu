#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>

// Function to convert numeric mode to string
char *mode_to_str(mode_t mode) {

    // printf("Mode: %d\n", mode);

    static char str[10]; // Enough space for access mode string + null terminator

    // check each permission bit and build string
    if (mode & S_IRUSR) str[0] = 'r'; else str[0] = '-';
    if (mode & S_IWUSR) str[1] = 'w'; else str[1] = '-';
    if (mode & S_IXUSR) str[2] = 'x'; else str[2] = '-';
    if (mode & S_IRGRP) str[3] = 'r'; else str[3] = '-';
    if (mode & S_IWGRP) str[4] = 'w'; else str[4] = '-';
    if (mode & S_IXGRP) str[5] = 'x'; else str[5] = '-';
    if (mode & S_IROTH) str[6] = 'r'; else str[6] = '-';
    if (mode & S_IWOTH) str[7] = 'w'; else str[7] = '-';
    if (mode & S_IXOTH) str[8] = 'x'; else str[8] = '-';
    str[9] = '\0';

    return str;
}

void print_file_info(const char* filename) {
    struct stat file_stat;
    if (lstat(filename, &file_stat) == -1) {
        perror("lstat");
        return;
    }

    printf("File: %s\n", filename);
    printf("inode number: %ld\n", (long)file_stat.st_ino);
    printf("number of links: %ld\n", (long)file_stat.st_nlink);
    printf("User ID of owner: %d\n", file_stat.st_uid);
    printf("Group ID of owner: %d\n", file_stat.st_gid);
    printf("Size in bytes: %ld\n", (long)file_stat.st_size);

    char access_time[26];
    strftime(access_time, 25, "%a %b %e %H:%M:%S %G", localtime(&file_stat.st_atime));
    printf("Last access: %s\n", access_time);

    char modify_time[26];
    strftime(modify_time, 25, "%a %b %e %H:%M:%S %G", localtime(&file_stat.st_mtime));
    printf("Last modification: %s\n", modify_time);

    char change_time[26];
    strftime(change_time, 25, "%a %b %e %H:%M:%S %G", localtime(&file_stat.st_ctime));
    printf("Last status change: %s\n", change_time);

    printf("Number of blocks allocated: %ld\n", file_stat.st_blocks);

    const unsigned int access_mode_octal = (unsigned int) file_stat.st_mode;
    char *access_mode_str = mode_to_str(file_stat.st_mode);
    printf("Access mode in octal: %o\n", access_mode_octal);
    printf("Access mode as string: %s\n", access_mode_str);

    printf("\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: filestats directory_name\n");
        return 1;
    }

    const char* directory_path = argv[1];
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(directory_path)) == NULL) {
        fprintf(stderr, "can't open %s\n", directory_path);
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        char full_path[PATH_MAX];
        snprintf(full_path, PATH_MAX, "%s/%s", directory_path, entry->d_name);

        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            print_file_info(full_path);
        }
    }

    closedir(dir);
    return 0;
}