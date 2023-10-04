#include "mysync.h"

int main(int argc, char *argv[])
{
    if (argc < 3) // If less than 2 directories are named
    {
        perror("Less than 2 directories are named.");
        exit(EXIT_FAILURE);
    }

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
        case 'v':
            // do something
            break;
        default:
            perror("Invalid option provided");
        }
    }

    // Check and evaluate given directories

    exit(EXIT_SUCCESS);
}