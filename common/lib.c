#include <smdb.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>

static const char *errcodes[] = {
    [0] = "SMDB_OK",
    [1] = "SMDB_NETWORK_ERR",
    [2] = "SMDB_INVALID_ARGS",
    [3] = "SMDB_KEY_EXISTS",
};

const char *smdb_err_to_str(int err) { return errcodes[abs(err)]; }

smdb_t *smdb_init(const char *socket_path) {
  const char *path;
  struct sockaddr_un addr;
  smdb_t *db;

  path = socket_path ? socket_path : "/var/run/smdb.sock";
  db = (smdb_t *)malloc(sizeof(smdb_t));

  if (db == NULL)
    return NULL;

  if ((db->socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    goto err;

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, path);

  if (connect(db->socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    goto err;

  return db;

err:
  if (db->socket_fd > 0) {
    close(db->socket_fd);
  }

  free(db);
  return NULL;
}

void smdb_destroy(smdb_t *ctx) {
  if (ctx != NULL) {
    close(ctx->socket_fd);
    free(ctx);
  }
}

int smdb_get(smdb_t *ctx, const char *key, char *buffer, size_t buflen) {
  if (ctx == NULL || key == NULL || buffer == NULL || buflen == 0)
    return SMDB_INVALID_ARGS;

  return SMDB_NETWORK_ERR;
}

int smdb_set(smdb_t *ctx, const char *key, const char *value) {
  if (ctx == NULL || key == NULL || value == NULL)
    return SMDB_INVALID_ARGS;

  return SMDB_NETWORK_ERR;
}