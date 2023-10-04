#include "mysync.h"

void usage(){
    printf("USAGE\n");
}


void openDir(){

    // check directories exist

    // open and read each directory, locating all regular files in the directory. 
    // (At this stage, limit your project to support just two directories, and to just print each filename as it is found)
    

    // Obtain list of files to be updated
    // recurse through file system (onky for -r)
    // Else: only consider top level directories.
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

    // Check and evaluate given directories

    exit(EXIT_SUCCESS);
}

