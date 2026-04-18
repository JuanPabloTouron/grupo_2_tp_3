/*
 * protocol_msg.h
 *
 *  Created on: Apr 2, 2026
 *      Author: seb
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/******************************************************************************
 * Message
 *****************************************************************************/

typedef enum
{
  MSG_TYPE_TICK = 0,
  MSG_TYPE_REQUEST = 1,
  MSG_TYPE_RESPONSE = 2,
} MsgFieldHeadType_t;

typedef struct
{
  uint32_t type;
} MsgHead_t;

typedef struct
{
  MsgHead_t head;
  uint32_t stamp;
} MsgTick_t;

typedef struct
{
  MsgHead_t head;
  uint32_t id;
  uint32_t priority;
} MsgRequest_t;

typedef struct
{
  MsgHead_t head;
  uint32_t id;
  char *frame;
} MsgResponse_t;


int msg_tick_create(MsgTick_t *msg, uint32_t stamp);
int msg_tick_write(char *buffer, MsgTick_t *msg);

int msg_request_parser(const char *buffer, MsgRequest_t *msg);

int msg_response_create(MsgResponse_t *msg, uint32_t id);
int msg_response_write(char *buffer, MsgResponse_t *msg);

/******************************************************************************
 * Protocol
 *****************************************************************************/

#define BUFFER_MAX 128

typedef enum
{
  PROTOCOL_PARSER_STATE_NO_MESSAGE,
  PROTOCOL_PARSER_STATE_START,
  PROTOCOL_PARSER_STATE_BUSY,
  PROTOCOL_PARSER_STATE_OVERFLOW,
  PROTOCOL_PARSER_STATE_END,
} ProtocolParserState_t;

typedef struct
{
  char *buffer;
  size_t max_size;
  int index;
  bool on_msg;
} ProtocolParser_t;

typedef void (*OnMsgCallback_t)(ProtocolParser_t*);

void protocol_parser_init(ProtocolParser_t *p, char *buffer, size_t max_size);

ProtocolParserState_t protocol_parser_feed(ProtocolParser_t *p, char c, OnMsgCallback_t on_msg);

#endif /* INC_PROTOCOL_H_ */
