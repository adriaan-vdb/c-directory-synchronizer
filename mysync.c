#include "mysync.h"

void usage()
{
    // printf("- USAGE\n");
}

void processDirectory(const char *dirname, OPTIONS *flags)
{
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
    if (dir == NULL)
    {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }
    printf("Directory Name: %s\n", (char *)dirname);

    while ((entry = readdir(dir)) != NULL)
    {
        struct stat fileStat;

        // Check if -a flag present
        if (!flags->a && entry->d_name[0] == '.')
        {
            continue;
        }

        // Check if current entry is a file or directory (note that d_type doesn't work on windows)
        struct stat teststat;
        char path[1024];
        sprintf(path, "%s/%s", dirname, entry->d_name);
        stat(path, &teststat);

        if (S_ISDIR(teststat.st_mode)) // Current item is sub-directory
        {
            printf("SDIR: %s\n", entry->d_name);
        }
        else // Current item is a file
        {
            FILES newfile;
            newfile = (FILES){.pathname = path, .name = entry->d_name, .mtime = fileStat.st_mtime};
            printf("FILE: %s\n", newfile.name);
            hashtable_add(files, newfile);
        }
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

    // Initialise -i and -o lists
    flags.i_patterns = list_new();
    flags.o_patterns = list_new();

    while ((opt = getopt(argc, argv, OPTLIST)) != -1)
    {
        switch (opt)
        {
        case 'a':
            flags.a = true;
            break;
        case 'i':
            flags.i = true;
            list_add(flags.i_patterns, optarg);
            // need to evaluate given pattern/s
            break;
        case 'n':
            flags.n = true;
        case 'v':
            flags.v = true;
            break;
        case 'o':
            flags.o = true;
            list_add(flags.o_patterns, optarg);
            // need to evaluate given pattern/s
            break;
        case 'p':
            flags.p = true;
            break;
        case 'r':
            flags.r = true;
            break;
        default:
            perror("Invalid option provided");
        }
    }

    files = hashtable_new();

    for (int i = optind; i < argc; i++)
    {
        processDirectory(argv[i], &flags);
    }

    // Check and evaluate given directories

    exit(EXIT_SUCCESS);
}
