#ifndef TASK_PROCESS_H_
#define TASK_PROCESS_H_



#include "cmsis_os.h"
#include <stdint.h>

typedef struct
{
  char *msg;
  uint32_t period_ms;
} task_process_args_t;

void task_process_init(task_process_args_t *args);

#endif /* TASK_PROCESS_H_ */
