#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_FDS 128

int sockfd;

void usage() {
  puts("USAGE: smdb-server [-p path to socket] [-h]");
}

void serve() {
  struct pollfd fds[MAX_FDS];
  char buffer[512];
  int status, new_conn, connoff;
  int on, cur, fdcnt, compress;

  memset(fds, 0, sizeof(fds));

  fds[0].fd = sockfd;
  fds[0].events = POLLIN;
  on = fdcnt = 1;
  
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

          fds[fdcnt].fd = new_conn;
          fds[fdcnt].events = POLLIN;
          fdcnt++;
        } while (new_conn != -1); 
      } else {
        connoff = 0;

        // drain the incoming packets.
        do {
          status = recv(fds[i].fd, buffer, sizeof(buffer), 0);
          if (status < 0 && errno != EWOULDBLOCK) {
            perror("recv");
            connoff = 1;

            break;
          }

          if (status == 0) {
            connoff = 1;
            break;
          }

          // handle packet here
          break;
        } while (1);

        // close the connection if needed.
        if (connoff) {
          close(fds[i].fd);
          fds[i].fd = -1;

          compress = 1;
        }
      }
    }

    if (compress) {
      compress = 0;

      // compress all the unused fds into a linear array.
      for (int i = 0; i < fdcnt; i++) {
        if (fds[i].fd == -1) {
          for(int j = i; j < fdcnt - 1; j++) {
            fds[j].fd = fds[j + 1].fd;
          }
          
          i--;
          fdcnt--;
        }
      }
    }
  } while (on);

  for (int i = 0; i < fdcnt; i++) {
    if(fds[i].fd >= 0)
      close(fds[i].fd);
  }
}

int main(int argc, char **argv) {
  struct sockaddr_un addr;
  char *sockpath;
  int status, c;

  sockpath = NULL;

  while ((c = getopt(argc, argv, "hp:")) != -1) {
    switch (c) {
      case 'h':
        usage();
        return 0;
      case 'p':
        sockpath = optarg;
        break;
      default:
        usage();
        return 1;
    }
  }

  if (sockpath == NULL)
    sockpath = "/var/run/smdb.sock";

  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("socket");
    return 1;
  }

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, sockpath);

  status = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));

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