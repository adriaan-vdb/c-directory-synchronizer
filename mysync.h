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


#define HASHTABLE_SIZE 997
#define OPTLIST "ai:no:prv"
#define CHECK_ALLOC(p)      \
    if (p == NULL)          \
    {                       \
        perror(__func__);   \
        exit(EXIT_FAILURE); \
    }

typedef struct _list
{
    char *string;
    struct _list *next;
} LIST;

typedef struct
{
    bool a;
    bool i;
    bool n;
    bool o;
    bool p;
    bool r;
    bool v;
    LIST *i_patterns;
    LIST *o_patterns;
} OPTIONS;

extern OPTIONS flags;

typedef struct
{
    char *pathname;
    char *name;
    char *directory;
    mode_t permissions;
    time_t mtime;
} FILES;

typedef struct _hashlist
{
    FILES file;
    struct _hashlist *next;
    bool new; // True if the file needs to be added to the directory (rather than modified); not in use for hashtable data structure
} FILELIST;

typedef FILELIST *HASHTABLE;

extern HASHTABLE *files;
extern int ndirectories;
extern HASHTABLE *sync_files; // Used to keep track of the files that need to be synced in each directory

extern char **directories; // Stores list of directories

// struct NOTE_dirent
// {
//     ino_t d_ino;             // Inode number
//     off_t d_off;             // Offset to the next dirent structure
//     unsigned short d_reclen; // Length of this record
//     unsigned char d_type;    // Type of file (e.g., DT_REG for regular file, DT_DIR for directory)
//     char d_name[];           // Null-terminated filename
// };

//  'CREATE' A NEW, EMPTY LIST
extern LIST *list_new(void);

//  ADD A NEW (STRING) ITEM TO AN EXISTING LIST
extern LIST *list_add(LIST *list, char *newstring);

//  DETERMINE IF A REQUIRED ITEM (A STRING) IS STORED IN A GIVEN LIST
extern bool list_find(LIST *list, char *wanted);

extern HASHTABLE *hashtable_new(void);

extern void hashtable_add(HASHTABLE *hashtable, FILES file);

extern FILELIST *hashtable_view(HASHTABLE *hashtable, char *pathname);
