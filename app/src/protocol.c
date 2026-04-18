/*
 * decode_msg.c
 *
 *  Created on: Apr 2, 2026
 *      Author: seb
 */

#include "protocol.h"

/******************************************************************************
 * Message
 *****************************************************************************/

static int msg_head_create_(
    MsgHead_t *head,
    uint32_t type)
{
  if (NULL == head)
  {
    return 1;
  }

  head->type = type;

  return 0;
}

static int msg_head_write_(char *buffer, MsgHead_t *head)
{
  if (NULL == buffer || NULL == head)
  {
    return 1;
  }

  buffer[0] = '\0';
  int ret = sprintf(
      buffer,
      "{type:%d",
      head->type);

  if (ret < 0)
  {
    return 2;
  }

  return 0;
}

int msg_tick_create(
    MsgTick_t *msg,
    uint32_t stamp)
{
  if (NULL == msg)
  {
    return 1;
  }

  int ret = msg_head_create_(&msg->head, MSG_TYPE_TICK);
  if (0 != ret)
  {
    return 2;
  }
  msg->stamp = stamp;

  return 0;
}

int msg_tick_write(char *buffer, MsgTick_t *msg)
{
  if (NULL == buffer || NULL == msg)
  {
    return 1;
  }

  int ret;
  ret = msg_head_write_(buffer, &msg->head);
  if (0 != ret)
  {
    return 2;
  }

  size_t head_len = strlen(buffer);
  ret = sprintf(
      buffer + head_len,
      ",stamp:%d}",
      msg->stamp);

  if (ret < 0)
  {
    return 3;
  }

  return 0;
}


int msg_request_parser(const char *buffer, MsgRequest_t *msg)
{
  if (NULL == buffer || NULL == msg)
  {
    return 1;
  }

  int fields = sscanf(
      buffer,
      "{type:%u,id:%u,priority:%u}",
      &msg->head.type,
      &msg->id,
      &msg->priority);

  if (3 != fields)
  {
    return 2;
  }

  return 0;
}

int msg_response_create(
    MsgResponse_t *msg,
    uint32_t id)
{
  if (NULL == msg)
  {
    return 1;
  }

  int ret = msg_head_create_(&msg->head, MSG_TYPE_RESPONSE);
  if (0 != ret)
  {
    return 2;
  }
  msg->id = id;

  return 0;
}

int msg_response_write(char *buffer, MsgResponse_t *msg)
{
  if (NULL == buffer || NULL == msg)
  {
    return 1;
  }

  char *pwrite = buffer;

  int ret;
  ret = msg_head_write_(pwrite, &msg->head);
  if (0 != ret)
  {
    return 2;
  }
  size_t str_len;
  str_len = strlen(buffer);
  pwrite += str_len;

  str_len = sprintf(pwrite, ",id:%d,frame:", msg->id);
  if (str_len < 0)
  {
    return 3;
  }
  pwrite += str_len;

  memset(pwrite , '0', 256);
  pwrite += 256;

  str_len = sprintf(pwrite, "}");
  if (str_len < 0)
  {
    return 4;
  }

  return 0;
}

/******************************************************************************
 * Protocol
 *****************************************************************************/

static inline void protocol_parser_reset_(ProtocolParser_t *p)
{
  p->index = 0;
  p->on_msg = false;
}

void protocol_parser_init(ProtocolParser_t *p, char *buffer, size_t max_size)
{
  p->buffer = buffer;
  p->max_size = max_size;
  protocol_parser_reset_(p);
}

ProtocolParserState_t protocol_parser_feed(ProtocolParser_t *p, char c, OnMsgCallback_t on_msg)
{
  if (c == '{')
  {
    p->index = 0;
    p->buffer[p->index] = c;
    p->index++;
    p->on_msg = true;
    return PROTOCOL_PARSER_STATE_START;
  }

  if (p->on_msg)
  {
    if ((p->max_size - 1) <= p->index)
    {
      protocol_parser_reset_(p);
      return PROTOCOL_PARSER_STATE_OVERFLOW;
    }

    p->buffer[p->index] = c;
    p->index++;

    if (c == '}')
    {
      p->buffer[p->index] = '\0';
      protocol_parser_reset_(p);
      if (NULL != on_msg)
      {
        on_msg(p);
      }
      return PROTOCOL_PARSER_STATE_END;
    }

    return PROTOCOL_PARSER_STATE_BUSY;
  }

  return PROTOCOL_PARSER_STATE_NO_MESSAGE;
}

