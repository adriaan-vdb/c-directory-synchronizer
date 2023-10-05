#include "mysync.h"

void usage(){
    printf("USAGE\n");
}

// NOTE: tag parameter refers to whether a was passed as a flag or not
void processDirectory(const char *dirname, char *tag) {
    DIR *dir;
    struct dirent *entry;

    // ------- If we wanted to get the full directory path:
    // char cwd[MAXPATHLEN]; 
    // if (getcwd(cwd, sizeof(cwd)) == NULL) {
    //     perror("Unable to get current working directory");
    //     exit(EXIT_FAILURE);
    // }
    // char fullpath[MAXPATHLEN]; 
    // sprintf(fullpath, "%s/%s", cwd, dirname);

    // printf("Directory Path: %s\n", fullpath);
    // --------

    dir = opendir(dirname);
    if (dir == NULL) {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }
    printf("Directory Name: %s\n", (char *)dirname);

    while ((entry = readdir(dir)) != NULL) {
        struct stat fileStat;
        char pathname[MAXPATHLEN]; // max path length (should dynamically allocate)

        sprintf(pathname, "%s/%s", dirname, entry->d_name);

        // Ignore "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Check if the entry is a directory (DT_DIR is defined on some systems)
        if (entry->d_type == DT_DIR) {
            printf("Sub Directory: %s\n", pathname); // sub directory found
        }

        if (stat(pathname, &fileStat) == -1) {
            perror("Error calling stat");
            continue; 
        }

        files = realloc(files, (nfiles+1)*sizeof(files[0]));
	    CHECK_ALLOC(files);		

        files[nfiles].pathname  = strdup(pathname);
        CHECK_ALLOC(files[nfiles].pathname);	

        //  REMEMBER THIS ELEMENT'S MODIFICATION TIME
        files[nfiles].mtime = fileStat.st_mtime;
        ++nfiles;

        printf("File: %s\n", pathname);
    }

closedir(dir);
}






int main(int argc, char *argv[])
{
    if (argc < 3) // If less than 2 directories are named
    {
        perror("Less than 2 directories are named.");
        exit(EXIT_FAILURE);
    }

    usage();

    // Check and evalute command line options

    int opt;
    char *tag;
    while ((opt = getopt(argc, argv, OPTLIST)) != -1)
    {
        switch (opt)
        {
        case 'a':
            // do something
            break;
        case 'i':
            // do something
            break;
        case 'n':
            // do something (also do -v)
        case 'v':
            // do something
            break;
        case 'o':
            // do something
            break;
        case 'p':
            // do something
            break;
        case 'r':
            // do something
            break;
        default:
            perror("Invalid option provided");
        }
    }


    for (int i = 1; i < argc; i++) {
        processDirectory(argv[i], *tag);
    }


    // Check and evaluate given directories

    exit(EXIT_SUCCESS);
}

