#ifndef TASK_OUTBOUND_H_
#define TASK_OUTBOUND_H_

#include "cmsis_os.h"
#include <stdint.h>

typedef struct
{
  char *msg;
  uint32_t period_ms;
} task_outbound_args_t;

void task_outbound_init(void);
void notify_pending(void);

#endif /* TASK_OUTBOUND_H_ */
