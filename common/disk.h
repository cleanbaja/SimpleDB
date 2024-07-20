#ifndef _COMMON_DISK_H_
#define _COMMON_DISK_H_

#include <stdint.h>
#include <stdio.h>

struct dbent
{
  uint64_t hash, next;

  char value[256];
};

/*

Routine Description:

    Searches for key in the database file store.

Arguments:

    dbfile - handle to open databse file.
    key - string key to search the database for.
    value - double pointer to output string.

Return Value:

    SMDB_OK on success, error code otherwise.

*/
int
dks_get_value(FILE* dbfile, const char* key, char** value);

/*

Routine Description:

    Inserts key-value pair into the database file store.

Arguments:

    dbfile - handle to open databse file.
    key - string key to insert into the database.
    value - string value to insert into the database.

Return Value:

    SMDB_OK on success, error code otherwise.

*/
int
dks_set_value(FILE* dbfile, const char* key, const char* value);

#endif // _COMMON_DISK_H_