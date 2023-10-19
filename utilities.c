#include "mysync.h"

// Copies a file from one path to another
void copy_files(char *sourceFilePath, char *destFilePath)
{
    // Attempts to open the source file
    FILE *sourceFile = fopen(sourceFilePath, "rb");
    if (sourceFile == NULL)
    {
        fprintf(stderr, "Error opening source file '%s' : %s\n", sourceFilePath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Attempts to open the destination file
    FILE *destinationFile = fopen(destFilePath, "wb");
    if (destinationFile == NULL)
    {
        fprintf(stderr, "Error opening destination file '%s' : %s\n", destFilePath, strerror(errno));
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

// Checks if a filename matches with any of the globs in a list of strings
bool in_list(char *filename, LIST *patterns)
{
    // Iterates through all the patterns in a given list
    while (patterns != NULL)
    {
        // Converts the pattern (which may be a glob) into a regex pattern
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

            // Check if the given filename matches with the current pattern being inspected
            if (regexec(&regex, filename, 0, NULL, 0) == 0)
            {
                return true;
            }
            regfree(&regex);
        }
        free(regexpattern);
        patterns = patterns->next; // Set pointer to point to the next pattern
    }
    return false;
}