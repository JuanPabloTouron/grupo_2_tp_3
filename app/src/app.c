/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include "main.h"
#include "cmsis_os.h"
#include "dwt.h"
#include "board.h"
#include "driver_uart.h"
#include "linked_list.h"
#include "protocol.h"
#include "task_inbound.h"
#include "task_tick.h"
#include "task_outbound.h"
#include "task_process.h"

/********************** macros and definitions *******************************/

#define TASK_READER_PERIOD_MS_  (5000)
#define MSG_TICK_               ("tick")
#define RB_BUFFER_SIZE_         (16)
#define INBOUND_QUEUE_SIZE_		(10)

/********************** internal data declaration ****************************/

char frame_[256] =  "0000000000000000000000000000000000000000000000000000000000000000"
					"0000000000000000000000000000000000000000000000000000000000000000"
					"0000000000000000000000000000000000000000000000000000000000000000"
					"0000000000000000000000000000000000000000000000000000000000000000";

typedef struct
{
  char *msg;
  uint32_t period_ms;
} task_write_args_t;

typedef struct
{
	uint32_t type;
	uint32_t stamp; //REVISAR TIPO
}msg_tick_t;

typedef struct
{
	uint32_t type;
	uint32_t id;
	uint32_t priority;
}msg_request_t;



/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

linked_list_t ordered_list;
QueueHandle_t inbound_queue_h;



static char msgprocess_[] = "Task Process\n";
static char msgoutbound_[] = "Task Outbound\n";
static char msginbound_[] = "Task Inbound\n";
static char msgtick_[] = "Tick\n";



static task_inbound_args_t inbound = {
	    .msg = msginbound_,
	    .period_ms = 1000
	};

static task_process_args_t process = {
	    .msg = msgprocess_,
	    .period_ms = 2000
	};

static task_outbound_args_t outbound = {
		.msg = msgoutbound_,
	    .period_ms = 3000
	};

static task_tick_args_t tick = {
	    .msg = msgtick_,
	    .period_ms = 500
	};
/********************** external data definition *****************************/

/********************** internal functions definition ************************/


/********************** external functions definition ************************/

void app_init(void)
{
  driver_uart_init();

  linked_list_init(&ordered_list);

  inbound_queue_h = xQueueCreate(INBOUND_QUEUE_SIZE_,sizeof(MsgRequest_t));

  task_inbound_init(&inbound);
  task_outbound_init(&outbound);
  task_process_init(&process);
  /* task_tick_init(&tick); */


  cycle_counter_init();
}

/********************** end of file ******************************************/
