#ifndef WS_RECIEVE_RESPONSE
#define WS_RECIEVE_RESPONSE

#include "message.h"
#include <stdint.h>
int if_opcode_handled(ws_opcode_t opcode);
ws_opcode_t ws_recieve_response(const char *message_response,
                                uint8_t *peer_message);
#endif
