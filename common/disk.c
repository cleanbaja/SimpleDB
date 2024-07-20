#include <smdb.h>

#include <string.h>
#include <stdlib.h>

#include "disk.h"
#include "xxhash.h"

#define STATIC_SEED 0xFA415231A2B4AE12

int dks_get_value(FILE *dbfile, const char *key, char **value) {
  struct dbent dbent;

  memset(&dbent, 0, sizeof(struct dbent));

  XXH64_hash_t hash = XXH64(key, strlen(key), STATIC_SEED);

  do {
    if (fseek(dbfile, dbent.next, SEEK_SET) < 0)
      return SMDB_FILESYSTEM_ERR;
    
    if (fread(&dbent, sizeof(struct dbent), 1, dbfile) == 0)
      return SMDB_FILESYSTEM_ERR;
  } while (hash != dbent.hash && dbent.next != 0);

  if (hash != dbent.hash)
    return SMDB_INVALID_ARGS;

  *value = malloc(strlen(dbent.value) + 1);
  memcpy(*value, dbent.value, strlen(dbent.value));

  return SMDB_OK;
}

int dks_set_value(FILE *dbfile, const char *key, const char *value) {
  struct dbent dbent, old_dbent;
  size_t filesize = 0;
  
  memset(&dbent, 0, sizeof(struct dbent));

  dbent.hash = XXH64(key, strlen(key), STATIC_SEED);
  memcpy(dbent.value, value, strlen(value));

  if (fseek(dbfile, 0, SEEK_END) < 0)
    return SMDB_FILESYSTEM_ERR;

  filesize = ftell(dbfile);

  if (fwrite(&dbent, sizeof(struct dbent), 1, dbfile) == 0)
    return SMDB_FILESYSTEM_ERR;

  // link it into the linked list, if needed.
  if (filesize == 0)
    return SMDB_OK;

  if (fseek(dbfile, filesize - sizeof(struct dbent), SEEK_SET) < 0)
    return SMDB_FILESYSTEM_ERR;

  if (fread(&old_dbent, sizeof(struct dbent), 1, dbfile) == 0)
    return SMDB_FILESYSTEM_ERR;

  old_dbent.next = filesize;

  if (fwrite(&old_dbent, sizeof(struct dbent), 1, dbfile) == 0)
    return SMDB_FILESYSTEM_ERR;
  
  return SMDB_OK;
}