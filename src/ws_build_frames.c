#include "headers/ws_build_frames.h"

size_t ws_build_frame(uint8_t opcode, uint64_t payload_len,
                      const uint8_t *payload, uint8_t frame[]) {
  size_t pos = 0;

  uint8_t first_byte = 0x80 | (opcode & 0x0F);
  frame[pos++] = first_byte;

  if (payload_len <= 125) {

    uint8_t second_byte = payload_len & 0x7F;
    frame[pos++] = second_byte;

  } else if (payload_len <= 65535) {

    uint8_t second_byte = 126;
    frame[pos++] = second_byte;

    uint8_t third_byte = (payload_len >> 8) & 0xFF;
    uint8_t fourth_byte = payload_len & 0xFF;

    frame[pos++] = third_byte;
    frame[pos++] = fourth_byte;

  } else {

    uint8_t second_byte = 127;
    frame[pos++] = second_byte;

    frame[pos++] = (payload_len >> 56) & 0xFF;
    frame[pos++] = (payload_len >> 48) & 0xFF;
    frame[pos++] = (payload_len >> 40) & 0xFF;
    frame[pos++] = (payload_len >> 32) & 0xFF;
    frame[pos++] = (payload_len >> 24) & 0xFF;
    frame[pos++] = (payload_len >> 16) & 0xFF;
    frame[pos++] = (payload_len >> 8) & 0xFF;
    frame[pos++] = payload_len & 0xFF;
  }

  for (uint64_t i = 0; i < payload_len; i++) {
    frame[pos++] = payload[i];
  }

  return pos;
}
