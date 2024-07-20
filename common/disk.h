#ifndef _COMMON_DISK_H_
#define _COMMON_DISK_H_

#include <stdio.h>
#include <stdint.h>

struct dbent {
  uint64_t hash, next;

  char value[256];
};

int dks_get_value(FILE *dbfile, const char *key, char **value);

int dks_set_value(FILE *dbfile, const char *key, const char *value);

#endif // _COMMON_DISK_H_