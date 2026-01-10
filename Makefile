CC = clang

CFLAGS = \
	-Isrc/headers \
	-Isrc/utils/headers \
	-ggdb3 -O0 -fno-omit-frame-pointer \
	-Wall -Wextra

LDFLAGS =
LDLIBS  = -lcrypto

TARGET = bin/tcp_server

SRC = \
	src/tcp_server.c \
	src/parser.c \
	src/get_request.c \
	src/workers.c \
	src/job_queue.c \
	src/errors.c \
	src/router.c \
	src/ws_build_frames.c \
	src/ws_send_response.c \
	src/ws_recieve_response.c \
	src/response.c \
	src/post_request.c \
	src/utils/leading_whitespace.c \
	src/utils/lower_string.c

OBJ = $(SRC:src/%.c=builds/%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

builds/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)

