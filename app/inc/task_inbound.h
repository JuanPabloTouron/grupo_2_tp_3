#ifndef TASK_INBOUND_H_
#define TASK_INBOUND_H_

#include "cmsis_os.h"
#include <stdint.h>

typedef struct
{
  char *msg;
  uint32_t period_ms;
} task_inbound_args_t;

void task_inbound_init(void);

#endif /* TASK_INBOUND_H_ */
