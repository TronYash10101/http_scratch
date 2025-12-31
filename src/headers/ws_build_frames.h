#ifndef WS_BUILD_FRAME
#define WS_BUILD_FRAME

#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>

#define BLOCK1_BIT_LEN 7
#define PAYLOAD_BIT_LEN 7

size_t ws_build_frame(uint8_t opcode, uint64_t payload_len,
                      const uint8_t *payload, uint8_t *frame);

#endif
