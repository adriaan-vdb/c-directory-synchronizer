#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <dirent.h>
#include  <sys/param.h>
#include  <time.h>

#define OPTLIST "ai:no:prv"
#define	CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); }

typedef struct _list {
     char           *string;
     struct _list   *next;
} LIST;

struct {
     bool  a;
     bool  i;
     bool  n;
     bool  o;
     bool  p;
     bool  r;
     bool  v;
     struct LIST *i_patterns;
     struct LIST *o_patterns;
} options;

typedef struct {
    char        *pathname;
    time_t      mtime;
} FILES;

FILES           *files  = NULL;
int             nfiles  = 0;

struct NOTE_dirent {
    ino_t d_ino;           // Inode number
    off_t d_off;           // Offset to the next dirent structure
    unsigned short d_reclen; // Length of this record
    unsigned char d_type;  // Type of file (e.g., DT_REG for regular file, DT_DIR for directory)
    char d_name[];         // Null-terminated filename
};
