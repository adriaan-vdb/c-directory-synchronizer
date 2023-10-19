#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <time.h>
#include <dirent.h>
#include <utime.h>
#include <regex.h>
#include <errno.h>

#define HASHTABLE_SIZE 997  // Size of the hashtable used to store files
#define OPTLIST "ai:no:prv" // Valid options provided to the program

// Checks if the memory allocation of a pointer was successful
#define CHECK_ALLOC(p)      \
    if (p == NULL)          \
    {                       \
        perror(__func__);   \
        exit(EXIT_FAILURE); \
    }

// Element of list of strings data structure
typedef struct _list
{
    char *string;       // Stores the data in the list element
    struct _list *next; // Pointer to the next list element
} LIST;

// Used to store the value of various options provided to the program
typedef struct
{
    bool a;           // True if -a option was present
    bool i;           // True if -i option was present
    bool n;           // True if -n option was present
    bool o;           // True if -o option was present
    bool p;           // True if -p option was present
    bool r;           // True if -r option was present
    bool v;           // True if -v option was present
    LIST *i_patterns; // Stores the pattern/s relevant to the -i option
    LIST *o_patterns; // Stores the pattern/s relevant to the -o option
} OPTIONS;

extern OPTIONS flags; // Stores the value of various options provided to the program

// Stores the information about a particular file
typedef struct
{
    char *pathname;     // Relative pathname of a file (excludes the top-level directory)
    char *name;         // Name of the file
    char *directory;    // Top-level directory of the file
    mode_t permissions; // File permissions
    time_t mtime;       // File modification time
} FILES;

// Element of list of files data structure
typedef struct _hashlist
{
    FILES file;             // Stores the data in the list
    struct _hashlist *next; // Stores the next list element
    bool new;               // True if the file needs to be added to the directory (rather than modified); not in use for hashtable
} FILELIST;

// Defines the type HASHTABLE as a pointer to a filelist
typedef FILELIST *HASHTABLE;

extern HASHTABLE *files;      // Hashtable of files (sorts all the files from all directories by pathname)
extern int ndirectories;      // Number of directories
extern HASHTABLE *sync_files; // Used to keep track of the files that need to be synced in each directory
extern char **directories;    // Stores list of directories

// ---------- FUNCTIONS ----------
extern char *glob2regex(char *glob);                                                   // Converts a glob into regex form so it can be compared
extern LIST *list_new(void);                                                           //  Create a new, empty list
extern LIST *list_add(LIST *list, char *newstring);                                    //  Add a new string item into the existing list
extern bool list_find(LIST *list, char *wanted);                                       //  Determine if a required item is stored in a list
extern HASHTABLE *hashtable_new(void);                                                 // Create a new, empty hastable
extern void hashtable_add(HASHTABLE *hashtable, FILES file);                           // Add a new file item into the existing hashtable
extern FILELIST *hashtable_view(HASHTABLE *hashtable, char *pathname);                 // View the file item with the given pathname
extern int sync_index(FILES file);                                                     // Index of the directory of a file; used to organise sync_files[]
extern void copy_files(char *sourceFilePath, char *destFilePath);                      // Copies a file from one path to another
extern char *concat_strings(const char *str1, const char *str2);                       // Concatenates str1+str2 and returns the string
extern void analyse_files();                                                           // Processes the hashtable into instructions for syncing
extern void process_directory(char *dirname, OPTIONS *flags, char *rootdirectoryname); // Scans each directory and stores files in a hashtable
extern void synchronise_directories();                                                 // Follows the instructions in sync_files[], to sync the directories
extern bool in_list(char *filename, LIST *patterns);                                   // Checks if a filename matches with any of the globs in a list of strings
extern void create_directory_list(int optind, char **argv);                            // Creates a list of the provided directories
extern void usage();                                                                   // Provides synopsis of program usage for appropriate use
