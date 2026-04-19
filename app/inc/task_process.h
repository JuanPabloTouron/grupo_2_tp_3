#ifndef TASK_PROCESS_H_
#define TASK_PROCESS_H_

#define MAX_MSG (50)


#include "cmsis_os.h"
#include "protocol.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
  char *msg;
  uint32_t period_ms;
} task_process_args_t;

void task_process_init(void);

extern MsgRequest_t msgBuffer[MAX_MSG];

bool task_process_get_next_msg(MsgRequest_t *msg);


#endif /* TASK_PROCESS_H_ */
