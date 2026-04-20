#ifndef TASK_TICK_H_
#define TASK_TICK_H_

#include "cmsis_os.h"
#include <stdint.h>
#include "protocol.h"

extern MsgTick_t tick;

typedef struct
{
  char *msg;
  uint32_t period_ms;
} task_tick_args_t;

void task_tick_init(void);
bool enough_time_before_tick(TickType_t time);

#endif /* TASK_TICK_H_ */
