#ifndef TASK_TICK_H_
#define TASK_TICK_H_

#include "cmsis_os.h"
#include <stdint.h>

typedef struct
{
  char *msg;
  uint32_t period_ms;
} task_tick_args_t;

void task_tick_init(task_tick_args_t *args);

#endif /* TASK_TICK_H_ */
