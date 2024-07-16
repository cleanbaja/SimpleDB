#ifndef _SMDB_H_
#define _SMDB_H_

#include <stddef.h>

enum {
  SMDB_OK = 0,
  SMDB_NETWORK_ERR = -1,
  SMDB_INVALID_ARGS = -2,
  SMDB_KEY_EXISTS = -3,
};

struct smdb_context {
  int socket_fd;
};

typedef struct smdb_context smdb_t;

/*

Routine Description:

    Creates a context object for further API requests, establishing
    a connection to the database and allocating structures.

Arguments:

    [OPTIONAL] socket_path - path to the UNIX socket which the server
    is listening on. If not specified, the default path is used.

Return Value:

    Active connection, or NULL if a connection could not be established.

*/
smdb_t *smdb_init(const char *socket_path);

/*

Routine Description:

    Retrieves the value associated with the key from the database.

Arguments:

    ctx - valid context object
    key - string key to retrieve the value for.
    buffer - pointer to a buffer to store the retrieved value.
    buflen - length of buffer.

Return Value:

    SMDB_OK if the value was retrieved successfully. Otherwise, the
    function will return the specific error code.

*/
int smdb_get(smdb_t *ctx, const char *key, char *buffer, size_t buflen);

/*

Routine Description:

    Adds a key-value pair into the database.

Arguments:

    ctx - valid context object
    key - string key to insert into the dartabase.
    value - string value to insert into the database.

Return Value:

    SMDB_OK if the value was retrieved successfully. Otherwise, the
    function will return the specific error code.

*/
int smdb_set(smdb_t *ctx, const char *key, const char *value);

/*

Routine Description:

    Destorys the context object, freeing all associated resources and
    closing the connection to the database.

Arguments:

    ctx - connection object to destroy.

Return Value:

    None.

*/
void smdb_destroy(smdb_t *ctx);

/*

Routine Description:

    Converts error codes into human readable strings.

Arguments:

    err - error code to convert.

Return Value:

    String which contains the human readable error code message.

*/
const char *smdb_err_to_str(int err);


#endif // _SMDB_H_