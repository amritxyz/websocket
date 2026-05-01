/*
 * server.c
 *
 * Usage: ./server <port>
 *
 * Copyright (C) 2026 amritxyz
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

#include <asm-generic/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "types.h"

#define BUFFER_SIZE	256
#define BACKLOG		5

static void
die(const char *msg)
{
	perror(msg);
	exit(1);
}

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		return 1;
	}

	i32 port_no = atoi(argv[1]);

	for (i32 fd = 0; fd < 3; fd++)
		if (fcntl(fd, F_GETFD) == -1)
			open("/dev/null", O_RDWR);

	/* Listening socket */
	i32 listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
		die("[ERROR] opening socket");

	/* Immediate reuse of port after restart */
	i32 opt = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family	= AF_INET;
	server_address.sin_addr.s_addr	= INADDR_ANY;
	server_address.sin_port		= htons(port_no);

	if (bind(listen_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
		die("[ERROR] on binding");

	/* Listen */
	listen(listen_fd, BACKLOG);
	printf("[INFO] Server listening on port %d ...\n", port_no);

	for (;;) {
		struct sockaddr_in cli_address;
		socklen_t client = sizeof(cli_address);

		i32 connfd = accept(listen_fd,
				   (struct sockaddr *)&cli_address,
				   &client);

		if (connfd < 0)
			die("[ERROR] on accept");

		printf("Client connected.\n");

		/* Message loop */
		char buffer[BUFFER_SIZE];
		i32 n;

		for (;;) {
			memset(buffer, 0, BUFFER_SIZE);
			n = read(connfd, buffer, BUFFER_SIZE - 1);

			if (n <= 0)
				break;

			/* strip trailig white space */
			buffer[strcspn(buffer, "\r\n")] = '\0';
			printf("[INFO] client: %s\n", buffer);

			/* decide reply */
			const char *reply;
			if (strcmp(buffer, "quit") == 0) {
				reply = "Bye.";
			} else {
				reply = buffer;
			}
			
			n = write(connfd, reply, strlen(reply));
			if (n < 0)
				die("[ERROR] writing to socket");

			if (strcmp(reply, "Bye.") == 0)
				break;
		}

		close(connfd);
		printf("[INFO] client disconnected.\n");
	}

	close(listen_fd);
	return 0;
}
