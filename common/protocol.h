#ifndef _COMMON_PROTOCOL_H_
#define _COMMON_PROTOCOL_H_

#include <stdint.h>

enum {
  PACKET_GET,
  PACKET_SET
};

struct packet {
  uint8_t type;
  uint8_t status;

  uint8_t key[256];
  uint8_t value[256];
};

void ptl_command_get(struct packet *pkt, const char *key);
void ptl_command_set(struct packet *pkt, const char *key, const char *value);

#endif // _COMMON_PROTOCOL_H_