#include "task_tick.h"
#include "driver_uart.h"
#include <string.h>
#include "task.h"
#include "task_outbound.h"


#define TASK_STACK_SIZE_		(512)
#define TX_MSG_BUFFER_SIZE_		(300)
/********************** internal task ***************************************/

MsgTick_t tick;

static void task_tick_(void* argument)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(100);


    char tx_buffer[TX_MSG_BUFFER_SIZE_];

    while (1)
    {
    	msg_tick_create(&tick, xLastWakeTime);
    	if (0 == msg_tick_write(tx_buffer, &tick)){
    		//driver_uart_tx((uint8_t*)tx_buffer, strlen(tx_buffer));
    		task_outbound_notify_pending();
    		vTaskDelayUntil(&xLastWakeTime, xPeriod);
    	}

    }
}

/********************** public API ******************************************/

void task_tick_init(void)
{
    BaseType_t status;

    status = xTaskCreate(
        task_tick_,
        "task_tick",
		TASK_STACK_SIZE_,
        NULL,
        tskIDLE_PRIORITY+2,
        NULL
    );

    while (pdPASS != status)
    {
        // manejo de error (opcional: log, assert, etc.)
    }
}
