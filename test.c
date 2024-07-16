#include <smdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  smdb_t *db;
  int status;
  char inbuffer[512];
  
  db = smdb_init(NULL);

  if (db == NULL) {
    puts("err: failed to connect to SimpleDB.");
    exit(1);
  }

  if ((status = smdb_set(db, "Hello", "World")) != SMDB_OK) {
    printf("err: failed to set key. (%s)\n", smdb_err_to_str(status));
    goto err;
  }

  if ((status = smdb_get(db, "Hello", inbuffer, 512)) != SMDB_OK) {
    printf("err: failed to get value. (%s)\n", smdb_err_to_str(status));
    goto err;
  }

  if (strcmp(inbuffer, "World") != 0) {
    puts("err: strings don't match!");
    goto err;
  }
  
err:
  smdb_destroy(db);
}