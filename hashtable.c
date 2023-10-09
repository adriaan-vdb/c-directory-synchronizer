#include "mysync.h"

//  RESEARCH SHOWS THAT USING PRIME-NUMBERS CAN IMPROVE PERFORMANCE
//  c.f.  https://www.quora.com/Why-should-the-size-of-a-hash-table-be-a-prime-number
#define HASHTABLE_SIZE 997

//  --------------------------------------------------------------------

//  FUNCTION hash_string() ACCEPTS A STRING PARAMETER,
//  AND RETURNS AN UNSIGNED 32-BIT INTEGER AS ITS RESULT
//
//  see:  https://en.cppreference.com/w/c/types/integer

uint32_t hash_string(char *string)
{
    uint32_t hash = 0;

    while (*string != '\0')
    {
        hash = hash * 33 + *string;
        ++string;
    }
    return hash;
}

//  ALLOCATE AND INITIALISE SPACE FOR A NEW HASHTABLE (AN ARRAY OF LISTS)
HASHTABLE *hashtable_new(void)
{
    HASHTABLE *new = calloc(HASHTABLE_SIZE, sizeof(HASHLIST *));

    CHECK_ALLOC(new);
    return new;
}

//  ADD A NEW STRING TO A GIVEN HASHTABLE
void hashtable_add(HASHTABLE *hashtable, FILES file)
{
    uint32_t h = hash_string(file.pathname) % HASHTABLE_SIZE; // choose list

    HASHLIST *new = calloc(1, sizeof(HASHLIST));
    CHECK_ALLOC(new);
    new->file = file;
    new->next = hashtable[h];
    hashtable[h] = new;
}

HASHLIST *hashtable_view(HASHTABLE *hashtable, char *pathname)
{
    uint32_t h = hash_string(pathname) % HASHTABLE_SIZE; // choose list
    return hashtable[h];
}
