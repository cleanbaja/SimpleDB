#include <smdb.h>

#include <stdlib.h>
#include <string.h>

#include "protocol.h"

void ptl_command_get(struct packet *pkt, const char *key) {
  int len;
  
  pkt->type = PACKET_GET;

  len = strlen(key) > 255 ? 255 : strlen(key);
  memcpy(pkt->key, key, len);
}

void ptl_command_set(struct packet *pkt, const char *key, const char *value) {
  int len;

  pkt->type = PACKET_SET;

  len = strlen(key) > 255 ? 255 : strlen(key);
  memcpy(pkt->key, key, len);
  
  len = strlen(value) > 255 ? 255 : strlen(value);
  memcpy(pkt->value, value, len);
}