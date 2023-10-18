#include "mysync.h"

// Initialises variables declared in header file
OPTIONS flags;
HASHTABLE *files;
int ndirectories = 0;
char **directories;

HASHTABLE *sync_files; // An a array of lists of files, where each element in the array represents a directory
// The lists of files represent instructions - where each file either has
// 1. New = true; the current directory doesn't contain an older version of this file (a file needs to be created)
// 2. New = false; the current directory contains an older version of this file (the file needs to be updated)

// Provides synopsis of program usage for appropriate use
void usage()
{
    printf("Usage:  ./mysync  [options]  directory1  directory2  [directory3  ...]\n");
}

// Index of the directory of a file; used to organise sync_files[]
int sync_index(FILES file)
{
    // Iterates through list of directories and returns index when list matches the name of the file's directory
    for (int i = 0; i < ndirectories; i++)
    {
        if (strcmp(directories[i], file.directory) == 0)
        {
            return i;
        }
    }
    fprintf(stderr, "Directory '%s' could not be found in list of given directories \n", file.directory);
    exit(EXIT_FAILURE);
}

// Copies a file from one path to another
void copy_files(char *sourceFilePath, char *destFilePath)
{
    // Attempts to open the source file
    FILE *sourceFile = fopen(sourceFilePath, "rb");
    if (sourceFile == NULL)
    {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    // Attempts to open the destination file
    FILE *destinationFile = fopen(destFilePath, "wb");
    if (destinationFile == NULL)
    {
        perror("Error opening destination file");
        fclose(sourceFile);
        exit(EXIT_FAILURE);
    }

    char buffer[4096]; // Stores each line to be copied to new file
    size_t bytesRead;  // Stores number of bytes read

    // Iteratively copies each line of source file to destination file
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0)
    {
        fwrite(buffer, 1, bytesRead, destinationFile);
    }

    fclose(sourceFile);
    fclose(destinationFile);
}

// Concatenates str1+str2 and returns the string
char *concat_strings(const char *str1, const char *str2)
{
    // Calculates and allocates appropriate amount of memory for concatenated string
    int totalLength = strlen(str1) + strlen(str2) + 1;
    char *result = (char *)malloc(totalLength * sizeof(char));

    // Performs and returns concatenation (this allows concatenation to be chained)
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

// Follows the instructions in sync_files[], to sync the directories
void synchronise_directories(HASHTABLE *sync_files)
{
    if (sync_files == NULL)
    {
        fprintf(stderr, "No instructions for syncing - sync_files array is empty \n");
        exit(EXIT_FAILURE);
    }

    if (flags.v)
    {
        printf("\n----------- SYNC COMMENCING -----------\n"); // printf("Directory: %d", 1); // How to work with Sub Directories?
    }

    // Synchronises each directory (one at a time), following the instructions provided by sync_files[]
    for (int j = 0; j < ndirectories; j++)
    {
        // Points to the current (initially, the first) instruction/file item of the current directory
        FILELIST *current = sync_files[j];

        // Iterates through the list of instructions and executes each individually
        while (current != NULL)
        {
            if (flags.v)
            {
                printf("\n");
            }

            // Checks if a file is needing to be created or updated, and works accordingly
            if (current->new) // File needs to be created
            {
                // Takes appropriate action for syncing files that are within subdirectories that currently don't exist in current directory
                if (strchr(current->file.pathname, '/') != NULL)
                {
                    // 1. Try to open subdirectories (until they don't exist)
                    DIR *dir;
                    char *tempstring = malloc(strlen(current->file.pathname) * sizeof(char));
                    CHECK_ALLOC(tempstring);
                    tempstring = strdup(current->file.pathname);
                    tempstring[strlen(current->file.pathname) - strlen(current->file.name) - 1] = '\0';
                    // Tempstring split into tokens so subdirectories are sequentially opened
                    // e.g. tries to open Subdirectory1, then Subdirectory1/Subdirectory2, ... until the subdirectory doesn't exist
                    char *token = strtok(tempstring, "/");
                    char *dirname = concat_strings(directories[j], concat_strings("/", token));
                    while (token != NULL && (dir = opendir(dirname)) != NULL)
                    {
                        printf("OPENING DIRECTORY: %s \n", dirname);
                        token = strtok(NULL, "/");
                        if (token != NULL)
                        {
                            dirname = concat_strings(dirname, concat_strings("/", token));
                        }
                    }

                    // 2. Create any remaining subdirectories
                    // Creates new subdirectories until the full "chain of subdirectories" is finished
                    // e.g. creates Subdirectory1/Subdirectory2/Subdirectory3, Subdirectory1/Subdirectory2/Subdirectory3/Subdirectory4, ...
                    while (token != NULL)
                    {
                        mkdir(dirname, 0777);
                        printf("MAKING DIRECTORY: %s \n", dirname);
                        token = strtok(NULL, "/");
                        if (token != NULL)
                        {
                            dirname = concat_strings(dirname, concat_strings("/", token));
                        }
                        mkdir(dirname, 0777);
                    }
                }

                // Constructs the pathname of the source and destination files
                char *source = concat_strings(concat_strings(current->file.directory, "/"), current->file.pathname);
                char *destination = concat_strings(concat_strings(directories[j], "/"), current->file.pathname);

                if (flags.v)
                {
                    printf("-- %s \n - copied from: %s\n", (char *)destination, (char *)source);
                }

                copy_files(source, destination); // Copies the source file into the destination file

                if (flags.p)
                {
                    mode_t new_mode = current->file.permissions;
                    chmod(destination, new_mode);

                    struct utimbuf new_times;
                    new_times.modtime = current->file.mtime;
                    utime(destination, &new_times);

                    if (flags.v)
                    {
                        printf("  - REVERTING MOD TIME: %s", ctime(&new_times.modtime));
                        printf("  - REVERTING PERMISSIONS: %o\n", new_mode);
                    }
                }
            }

            else // File needs to be updated (an older version already exists)
            {
                // Constructs the pathname of the source and destination files
                char *source = concat_strings(concat_strings(current->file.directory, "/"), current->file.pathname);
                char *destination = concat_strings(concat_strings(directories[j], "/"), current->file.pathname);

                if (flags.v)
                {
                    printf("-- %s \n - updated from: %s\n", (char *)destination, (char *)source);
                }

                copy_files(source, destination); // Copies the source file into the destination file

                if (flags.p)
                {
                    mode_t new_mode = current->file.permissions;
                    chmod(destination, new_mode);

                    struct utimbuf new_times;
                    new_times.modtime = current->file.mtime;
                    utime(destination, &new_times);

                    if (flags.v)
                    {
                        printf("  - REVERTING MOD TIME: %s", ctime(&new_times.modtime));
                        printf("  - REVERTING PERMISSIONS: %o\n", new_mode);
                    }
                }

                // why does this have to be a different operation, seems to have the same functionality to me
            }
            current = current->next;
        }
    }
}

bool in_list(char *filename, LIST *patterns)
{
    while (patterns != NULL)
    {
        // do stuff
        char *globpattern = patterns->string;
        char *regexpattern = glob2regex(globpattern);
        if (regexpattern)
        {
            regex_t regex;
            int err = regcomp(&regex, regexpattern, 0);
            if (err != 0)
            {
                perror("Couldn't compute regcomp of glob pattern");
            }
            if (regexec(&regex, filename, 0, NULL, 0) == 0)
            {
                if (flags.v)
                {
                    printf("IGNORING: %s", filename);
                }
                return true;
            }
            regfree(&regex);
        }
        free(regexpattern);
        patterns = patterns->next;
    }
    return false;
}

void process_directory(char *dirname, OPTIONS *flags, char *rootdirectoryname)
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
        perror("Unable to open directory\n");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        struct stat fileStat;

        // Check if current entry is a file or directory (note that d_type doesn't work on windows)
        char *path = malloc(sizeof(char) * (strlen(dirname) + 1 + strlen(entry->d_name) + 1));
        CHECK_ALLOC(path);
        sprintf(path, "%s/%s", dirname, entry->d_name);
        stat(path, &fileStat);
        if (S_ISDIR(fileStat.st_mode)) // Current item is sub-directory
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                if (flags->v)
                {
                    printf("- Skipping subdirectory: '%s'\n", entry->d_name);
                }
                continue;
            }
            if (flags->v)
            {
                printf(" -- recursively scanning subdirectory: '%s'\n", entry->d_name);
            }

            // printf("\t");

            if (flags->r)
            {
                if (rootdirectoryname == NULL)
                {
                    process_directory(concat_strings(dirname, concat_strings("/", entry->d_name)), flags, dirname);
                }
                else
                {
                    process_directory(concat_strings(dirname, concat_strings("/", entry->d_name)), flags, rootdirectoryname);
                }
            }
        }
        else // Current item is a file
        {
            // Check if -a flag present
            if (!flags->a && entry->d_name[0] == '.')
            {
                printf("- Skipping (no tag a): '%s' \n", entry->d_name);
                continue;
            }
            else if (flags->o && flags->i && in_list(entry->d_name, flags->i_patterns))
            {
                // continue (skip iteration) if file is in ignore list at all
                if (flags->v)
                {
                    printf("- Skipping (in ignore list): '%s'\n", entry->d_name);
                }
                continue;
            }
            else if (flags->i && in_list(entry->d_name, flags->i_patterns))
            {
                // continue (skip iteration) if file is in the ignore list
                if (flags->v)
                {
                    printf("- Skipping (in ignore list): '%s'\n", entry->d_name);
                }
                continue;
            }
            else if (flags->o && !in_list(entry->d_name, flags->o_patterns))
            {
                // continue (skip iteration) if file not in the only list
                if (flags->v)
                {
                    printf("- Skipping (not in only list): '%s'\n", entry->d_name);
                }
                continue;
            }

            FILES newfile;
            if (rootdirectoryname == NULL)
            {
                newfile.directory = (char *)dirname;
                newfile.pathname = path + strlen(dirname) + 1;
            }
            else
            {
                newfile.directory = (char *)rootdirectoryname;
                newfile.pathname = path + strlen(rootdirectoryname) + 1;
            }
            newfile.permissions = fileStat.st_mode;
            newfile.mtime = fileStat.st_mtime;
            newfile.name = strdup(entry->d_name);

            hashtable_add(files, newfile);
            // if (rootdirectoryname == NULL)
            // {
            //     printf("FILE: %s\n", hashtable_view(files, path + strlen(dirname) + 1)->file.name);
            // }
            // else
            // {
            //     printf("FILE: %s\n", hashtable_view(files, path + strlen(rootdirectoryname) + 1)->file.name);
            // }
        }
        // free(path); // check this out
    }

    closedir(dir);
}

void analyse_files()
{
    if (flags.v)
    {
        printf("\n----------- FILE ANALYSIS COMMENCING -----------\n"); // printf("Directory: %d", 1); // How to work with Sub Directories?
    }
    sync_files = calloc(ndirectories, sizeof(FILELIST *)); // creates array with a linked list of files for each directory
    for (int i = 0; i < HASHTABLE_SIZE; i++)
    {
        // For each "row" in the hashtable, view the row and account for any hashtable encoding collisions
        if (files[i] != NULL) // Current hashtable row contains files (isn't empty)
        {
            FILELIST *entry = files[i];
            while (entry != NULL) // Analyse each file collision (e.g. all files with relative path Folder1/File1)
            {
                char *relative_location = malloc(sizeof(char) * strlen(entry->file.pathname));
                relative_location = entry->file.pathname;
                FILELIST *analysis = NULL;
                int analysis_index = 0;
                while (entry != NULL && strcmp(relative_location, entry->file.pathname) == 0) // Store entries with same relative path together
                {
                    analysis = realloc(analysis, (analysis_index + 1) * sizeof(FILELIST));
                    CHECK_ALLOC(analysis)
                    analysis[analysis_index] = *entry;
                    analysis_index++;
                    entry = entry->next;
                }
                // Analyse each entry - two cases
                // 1. Only 1 entry (just copy over to every other directory)
                if (analysis_index == 1)
                {
                    if (flags.v)
                    {
                        printf("FILE to be created: %s\n", (char *)relative_location);
                    }
                    for (int j = 0; j < ndirectories; j++)
                    {
                        if (sync_index(analysis[0].file) == j)
                            continue; // Doesn't add anything instruction to directory containing the up to date file

                        // Adds file to other directories to be synced in
                        FILELIST *new_file = malloc(sizeof(FILELIST));
                        new_file->file = analysis[0].file;
                        new_file->new = true;
                        new_file->next = sync_files[j];
                        sync_files[j] = new_file;
                    }
                }
                // 2. More than 1 entry i.e. find most up to date version and copy to every other directory
                else if (analysis_index != 0)
                {
                    if (flags.v)
                    {
                        printf("Latest Version: %s\n", (char *)relative_location);
                    }
                    // Find the most up to date version (latest modification time)
                    int latest_index = 0; // Stores the index of the most up to date file version
                    for (int j = 1; j < analysis_index; j++)
                    {
                        if (difftime(analysis[j].file.mtime, analysis[latest_index].file.mtime) > 0)
                        {
                            latest_index = j;
                        }
                    }
                    for (int j = 0; j < ndirectories; j++)
                    {
                        if (sync_index(analysis[latest_index].file) == j)
                            continue; // Doesn't add anything instruction to directory containing the up to date file
                        // Check if the directory is having if a previous version of the file exists in the current directory
                        bool new = true;
                        for (int x = 0; x < analysis_index; x++)
                        {
                            if (sync_index(analysis[x].file) == j)
                            {
                                new = false;
                            }
                        }
                        FILELIST *new_file = malloc(sizeof(FILELIST));
                        new_file->file = analysis[latest_index].file;
                        new_file->new = new;
                        new_file->next = sync_files[j];
                        sync_files[j] = new_file;
                    }
                }
                else
                {
                    perror("No files to analyse within current hashtable entry");
                }
            }
        }
    }
}

void create_directory_list(int optind, char **argv)
{
    // Computes the maximum length of the given directory names
    int max_length = 0; // Used to evaluate the maximum directory name length
    for (int i = 0; i < ndirectories; i++)
    {
        if (strlen(argv[i + optind]) > max_length)
        {
            max_length = strlen(argv[i + optind]);
        }
    }
    directories = calloc(ndirectories, sizeof(char) * (max_length + 1));
    for (int i = 0; i < ndirectories; i++)
    {
        directories[i] = argv[optind + i];
    }
}

int main(int argc, char **argv)
{
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
            flags.i_patterns = list_add(flags.i_patterns, optarg);
            // need to evaluate given pattern/s
            break;
        case 'n':
            flags.n = true;
        case 'v':
            flags.v = true;
            break;
        case 'o':
            flags.o = true;
            flags.o_patterns = list_add(flags.o_patterns, optarg);
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

    if (argc - optind + 1 < 3) // If less than 2 directories are named
    {
        usage();
        perror("Less than 2 directories are named.");
        exit(EXIT_FAILURE);
    }

    if (flags.v)
    {
        printf("\n----------- SCAN COMMENCING -----------\n"); // printf("Directory: %d", 1); // How to work with Sub Directories?
    }

    files = hashtable_new();

    ndirectories = 0;
    for (int i = optind; i < argc; i++)
    {
        if (flags.v)
        {
            printf("\nscanning toplevel: '%s'\n", argv[i]);
        }
        process_directory(argv[i], &flags, NULL);
        ndirectories++;
    }

    create_directory_list(optind, argv);

    // Check and evaluate given directories
    analyse_files();

    // print sync_files array -> for debugging purposes
    // printf("\n ------ PRINTING SYNC_FILES DEBUG ------\n");
    // for (int i = 0; i < ndirectories; i++)
    // {
    //     FILELIST *temp = sync_files[i];
    //     while (temp != NULL)
    //     {
    //         printf("%s needs %s from %s\n", directories[i], temp->file.pathname, temp->file.directory);
    //         temp = temp->next;
    //     }
    // }

    synchronise_directories(sync_files);

    exit(EXIT_SUCCESS);
}
