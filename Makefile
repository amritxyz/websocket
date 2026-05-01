# Makefile — builds both server and client

CC      = cc
CFLAGS  = -Wall -Werror -Wextra
SRC_DIR = src
OBJ_DIR = build

SRCS    = $(SRC_DIR)/server.c $(SRC_DIR)/client.c
OBJS    = $(OBJ_DIR)/server.o $(OBJ_DIR)/client.o
BINS    = server client

.PHONY: all clean

all: $(BINS)

server: $(OBJ_DIR)/server.o
	$(CC) $^ -o $@

client: $(OBJ_DIR)/client.o
	$(CC) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/types.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BINS)
