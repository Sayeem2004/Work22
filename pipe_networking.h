#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef NETWORKING_H
#define NETWORKING_H
#define ACK "HOLA"
#define WKP "mario"

#define HANDSHAKE_BUFFER_SIZE 10
#define BUFFER_SIZE 1000

void print_error(int err, char *msg);
char *start_server_handshake(int *from_client);
void reset_server_handshake(int from_client);
int finish_server_handshake(char *name, int read_file);
int client_handshake(int *to_server);

#endif
