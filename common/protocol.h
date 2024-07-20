#ifndef _COMMON_PROTOCOL_H_
#define _COMMON_PROTOCOL_H_

#include <stdint.h>

enum
{
  PACKET_GET,
  PACKET_SET
};

struct packet
{
  uint8_t type;
  uint8_t status;

  uint8_t key[256];
  uint8_t value[256];
};

/*

Routine Description:

    Builds a client packet for requesting a value.

Arguments:

    pkt - pointer to input packet to populate.
    key - string key to request value for.

Return Value:

    None.

*/
void
ptl_command_get(struct packet* pkt, const char* key);

/*

Routine Description:

    Builds a client packet for setting a value.

Arguments:

    pkt - pointer to input packet to populate.
    key - string key to set into database.
    value - string value to set into database.

Return Value:

    None.

*/
void
ptl_command_set(struct packet* pkt, const char* key, const char* value);

#endif // _COMMON_PROTOCOL_H_