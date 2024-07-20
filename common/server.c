#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "disk.h"
#include "protocol.h"

#define MAX_FDS 128

struct pollfd fds[MAX_FDS];
int sockfd, fdcnt = 0;
FILE* dbfile;
char* store;

static void
usage()
/*

Routine Description:

    Prints usage description to the console.

Arguments:

    None.

Return Value:

    None.

*/
{
  puts("USAGE: smdb-server [-p socket path] [-f db path] [-h]");
}

static void
fd_add(int fd)
/*

Routine Description:

    Adds a file descriptor to the list of
    file descriptors to be polled.

Arguments:

    fd - file descriptor to add.

Return Value:

    None.

*/
{
  fds[fdcnt].fd = fd;
  fds[fdcnt].events = POLLIN;
  fdcnt++;
}

static void
fd_del(int index)
/*

Routine Description:

    Removes a file descriptor from the polling list.

Arguments:

    index - index of the file descriptor to remove.

Return Value:

    None.

*/
{
  close(fds[index].fd);
  fds[index].fd = -1;

  // reclaim all the unused fds into a linear array.
  for (int i = 0; i < fdcnt; i++) {
    if (fds[i].fd == -1) {
      for (int j = i; j < fdcnt - 1; j++) {
        fds[j].fd = fds[j + 1].fd;
      }

      i--;
      fdcnt--;
    }
  }
}

static int
handle_packet(struct packet* pkt, int fd)
/*

Routine Description:

    Processes a client request packet.

Arguments:

    pkt - packet to process.
    fd - file descriptor of the client.

Return Value:

    SMDB_OK if the packet was processed successfully.
    Otherwise, a UNIX error will be returned.

*/
{
  struct packet result;
  char* value;
  int status;

  value = NULL;

  memset(&result, 0, sizeof(struct packet));

  if (pkt->type == PACKET_GET) {
    result.type = PACKET_GET;

    if ((result.status = dks_get_value(dbfile, pkt->key, &value)) == 0)
      strcpy(result.value, value);
  } else {
    result.type = PACKET_SET;
    result.status = dks_set_value(dbfile, pkt->key, pkt->value);
  }

  status = send(fd, &result, sizeof(result), 0);

  if (value != NULL)
    free(value);

  return status;
}

static void
serve()
/*

Routine Description:

    Internal server runloop.

Arguments:

    None.

Return Value:

    None.

*/
{
  struct packet pkt;
  int status, new_conn, connoff;
  int on, cur;

  fd_add(sockfd);
  on = 1;

  do {
    // 1 sec timeout
    status = poll(fds, MAX_FDS, 1000 * 60);

    if (status < 0) {
      perror("poll");
      return;
    }

    if (status == 0) {
      puts("err: connection timed out.");
      return;
    }

    cur = fdcnt;
    for (int i = 0; i < cur; i++) {
      if (fds[i].revents == 0)
        continue;

      if (fds[i].fd == sockfd) {
        // accept all incoming connections.
        do {
          new_conn = accept(sockfd, NULL, NULL);

          if (new_conn < 0 && errno != EWOULDBLOCK) {
            perror("accept");
            on = 0;

            break;
          }

          fd_add(new_conn);
        } while (new_conn != -1);
      } else {
        connoff = 0;

        // drain the incoming packets.
        while (1) {
          status = recv(fds[i].fd, &pkt, sizeof(pkt), 0);
          if (status < 0 && errno != EWOULDBLOCK) {
            perror("recv");
            connoff = 1;

            break;
          }

          if (status == 0) {
            connoff = 1;
            break;
          }

          if (handle_packet(&pkt, fds[i].fd) < 0) {
            connoff = 1;
            break;
          }
        }

        // close the connection if needed.
        if (connoff)
          fd_del(i);
      }
    }
  } while (on);

  for (int i = 0; i < fdcnt; i++) {
    if (fds[i].fd >= 0)
      close(fds[i].fd);
  }
}

int
main(int argc, char** argv)
{
  struct sockaddr_un addr;
  char *sockpath, *diskpath;
  int status, c;

  sockpath = NULL;
  diskpath = NULL;

  while ((c = getopt(argc, argv, "hp:f:")) != -1) {
    switch (c) {
      case 'h':
        usage();
        return 0;
      case 'p':
        sockpath = optarg;
        break;
      case 'f':
        diskpath = optarg;
        break;
      default:
        usage();
        return 1;
    }
  }

  if (sockpath == NULL)
    sockpath = "/var/run/smdb.sock";

  if (diskpath == NULL)
    diskpath = "/var/run/smdb.db";

  // delete the socket, so we don't have bind errors.
  (void)remove(sockpath);

  dbfile = fopen(diskpath, "a+");
  if (dbfile == NULL)
    return 1;

  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("socket");
    return 1;
  }

  status = fcntl(sockfd, F_GETFL, 0);

  if (status < 0) {
    perror("fcntl");
    return 1;
  }

  // set socket as non-blocking.
  status = fcntl(sockfd, F_SETFL, status | O_NONBLOCK);

  if (status < 0) {
    perror("fcntl");
    return 0;
  }

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, sockpath);

  status = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

  if (status < 0) {
    perror("bind");
    return 1;
  }

  status = listen(sockfd, 3);

  if (status < 0) {
    perror("listen");
    return 1;
  }

  serve();

  close(sockfd);
  return 0;
}