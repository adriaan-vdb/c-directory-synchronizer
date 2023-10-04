#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define OPTLIST "ai:no:prv"

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

struct {
    
}