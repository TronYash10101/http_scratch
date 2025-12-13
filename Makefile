CC = clang
CFLAGS = -Isrc/headers -Isrc/utils/headers -ggdb3 -O0 -fno-omit-frame-pointer -Wall -Wextra 

TARGET = bin/tcp_server

SRC = \
    src/tcp_server.c \
    src/parser.c \
    src/get_request.c \
    src/response.c \
    src/utils/leading_whitespace.c \
    src/utils/lower_string.c

# Transform src/xxx.c → builds/src/xxx.o
OBJ = $(SRC:src/%.c=builds/%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

# Pattern rule to handle nested folders
builds/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)

