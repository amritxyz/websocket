/*
 * client.c
 *
 * Usage: ./client <hostname> <port>
 *
 * Copyright (C) 2024 amritxyz
 * https://amritxyz.github.io
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* TODO
 * > Implement connection with multiple clients rather then only one client
 * > Implement minimal web interface if possible in future */

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "types.h"

#define BUFFER_SIZE 256

static void
die(const char *msg)
{
	perror(msg);
	exit(0);
}

int
main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
		return 1;
	}

	i32 port_no = atoi(argv[2]);

	/* Resolve host */
	struct hostent *server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr, "[ERROR] no such host: %s\n", argv[1]);
		return 1;
	}

	/* Create socket */
	i32 sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0)
		die("[ERROR] opening socket");

	/* Connect */
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);
	server_address.sin_port = htons(port_no);

	if (connect(sock_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
		die("[ERROR] connecting");

	printf("Connected to %s:%d\n", argv[1], port_no);

	/* message loop */
	char buffer[256];
	i32 n;

	for (;;) {
		printf("You: ");
		fflush(stdout);

		memset(buffer, 0, BUFFER_SIZE);
		if (fgets(buffer, BUFFER_SIZE - 1, stdin) == NULL)
			break;

		/* send (including newline so server can delimit) */
		n = write(sock_fd, buffer, BUFFER_SIZE - 1);
		memset(buffer, 0, BUFFER_SIZE);
		n = read(sock_fd, buffer, BUFFER_SIZE - 1);
		if (n < 0)
			die("[ERROR] reading from socket");

		printf("Server: %s\n", buffer);

		if (strcmp(buffer, "Bye") == 0)
			break;
	}

	close(sock_fd);
	printf("Disconnected.\n");
	return 0;
}
