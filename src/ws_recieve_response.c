#include "headers/ws_recieve_response.h"

int if_opcode_handled(ws_opcode_t opcode) {
  switch (opcode) {
  case WS_OP_CONTINUATION:
    return 0;
  case WS_OP_TEXT:
    return 0;
  case WS_OP_CLOSE:
    return 0;
  case WS_OP_PING:
    return 0;
  case WS_OP_PONG:
    return 0;
  }
  return -1;
}

ws_opcode_t ws_recieve_response(const char *message_response,
                                uint8_t *peer_message) {

  uint64_t payload_len = 0;
  uint8_t opcode = message_response[0] & 0x0F;
  LOG_INFO("%d", opcode);

  if (if_opcode_handled(opcode) == -1) {
    LOG_ERROR("Incorrect Opcode");
    return -1;
  }
  // handle opcode
  uint8_t payload_bytes = message_response[1] & 0x7F;
  uint64_t offset = 2;

  if (payload_bytes <= 125) {
    payload_len = payload_bytes;
  } else if (payload_bytes == 126) {
    payload_len = 0;
    uint8_t second_byte = message_response[offset];
    uint8_t third_byte = message_response[offset + 1];
    payload_len = (second_byte << 8) | third_byte;
    offset += 2;
  } else if (payload_bytes == 127) {
    payload_len = 0;
    offset += 2;
    for (int i = 0; i != 8; i++) {
      payload_len = message_response[i + offset] | (payload_len << 8);
    }
    offset += 8;
  }

  uint8_t mask_keys[4];

  for (int i = 0; i < 4; i++) {
    mask_keys[i] = message_response[offset + i];
  }
  offset += 4;

  for (uint64_t j = 0; j < payload_len; j++) {
    peer_message[j] = message_response[offset + j] ^ mask_keys[j % 4];
  }

  return opcode;
}
