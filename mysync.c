#include "mysync.h"

OPTIONS flags;
HASHTABLE *files;
int ndirectories = 0;
char **directories;
HASHTABLE *sync_files;

void usage()
{
    // printf("- USAGE\n");
}

int sync_index(FILES file) // Returns the index of the sync array, given a file
{
    for (int i = 0; i < ndirectories; i++)
    {
        if (strcmp(directories[i], file.directory) == 0)
        {
            return i;
        }
    }
    return -1;
}


void copyFiles(char *sourceFilePath, char *destFilePath){

   FILE *sourceFile = fopen(sourceFilePath, "rb");
    if (sourceFile == NULL) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    FILE *destinationFile = fopen(destFilePath, "wb");
    if (destinationFile == NULL) {
        perror("Error opening destination file");
        fclose(sourceFile);
        exit(EXIT_FAILURE);
    }

    char buffer[4096];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0){
        fwrite(buffer, 1, bytesRead, destinationFile);
    }

    fclose(sourceFile);
    fclose(destinationFile);
}
char *concatStrings(const char *str1, const char *str2) {
    size_t totalLength = strlen(str1) + strlen(str2) + 1;
    char *result = (char *)malloc(totalLength);

    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

void DOTHETHING(FILELIST *sync_files) {


    // extern FILELIST *sync_files; 
    // convert all the instructions stored in the array into actual copied files ("things" - josh - "and directories")
    // iterate through each directory and then iterate through the list of instructions in the sync file
    // Instructions: Create new file, modify existing file, creating directories and putting files in directories
    // while opendir(next_name) != NULL... keep opening .. upon failure start iterativly creating directories... 
    // and where necessary create/update file

    // sync_files[j] = (FILELIST){.file = analysis[latest_index].file, .new = new, .next = &temp};
    // where j is each directory to be synced

    if (sync_files == NULL) {
        printf("sync_files is not initialized or is an empty list.");
        return;
    }

    printf("----------- START -----------\n"); // printf("Directory: %d", 1); // How to work with Sub Directories?
    FILELIST *current = sync_files;

    // printf("NAME: %s",current[0].file.name);
    // current = current->next;    
    // printf("NAME: %s",current[0].file.name);
    // current = current->next;  

   
    for (int j = 0; j < ndirectories; j++) {

        while (current != NULL){
            printf("NAME: %s",current->file.name);

            if (current->new){
                char *str1 = concatStrings(concatStrings(sync_files->file.directory, "/"), sync_files[j].file.pathname) ;
                char *str2 = concatStrings(concatStrings(directories[j], "/"), sync_files[j].file.directory);

                printf("str1: %s",(char *)str1);
                printf("str2: %s",(char *)str2);
                
                //copyFiles(str1, str2);     // (source, destination)
            }

            else{

            }
            current = current->next;
        }
    }
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
        char *path = malloc(sizeof(char) * (strlen(dirname) + 1 + strlen(entry->d_name) + 1));
        CHECK_ALLOC(path);
        sprintf(path, "%s/%s", dirname, entry->d_name);
        stat(path, &fileStat);
        if (S_ISDIR(fileStat.st_mode)) // Current item is sub-directory
        {
            printf("SDIR: %s\n", entry->d_name);
        }
        else // Current item is a file
        {
            FILES newfile;
            newfile.directory = (char *)dirname;
            newfile.mtime = fileStat.st_mtime;
            newfile.pathname = path + strlen(dirname) + 1;
            newfile.name = strdup(entry->d_name);
            hashtable_add(files, newfile);
            printf("FILE: %s\n", hashtable_view(files, path + strlen(dirname) + 1)->file.name);
        }
        // free(path); // check this out
    }

    closedir(dir);
}

void analyse_files()
{
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
                        printf("TESTSYNC: %s\n", sync_files[j]->file.name);
                    }
                }
                // 2. More than 1 entry i.e. find most up to date version and copy to every other directory
                else if (analysis_index != 0)
                {
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
                        printf("TESTSYNC: %s\n", sync_files[j]->file.name);
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

    ndirectories = 0;
    for (int i = optind; i < argc; i++)
    {
        processDirectory(argv[i], &flags);
        ndirectories++;
    }

    create_directory_list(optind, argv);

    // Check and evaluate given directories
    analyse_files();

    // print sync_files array -> for debugging purposes
    printf("\n ------ PRINTING SYNC_FILES DEBUG ------\n");
    for (int i = 0; i < ndirectories; i++)
    {
        FILELIST *temp = sync_files[i];
        while (temp != NULL)
        {
            printf("%s: %s\n", directories[i], temp->file.name);
            temp = temp->next;
        }
    }

    DOTHETHING(sync_files);

    exit(EXIT_SUCCESS);
}
