#include "task_tick.h"
#include "driver_uart.h"
#include <string.h>
#include "task.h"
#include "task_outbound.h"


#define TASK_STACK_SIZE_		(512)
#define TX_MSG_BUFFER_SIZE_		(300)
#define TICK_PERIOD				(100)
/********************** internal task ***************************************/

MsgTick_t tick;

bool enough_time_before_tick(TickType_t time){ //la creatividad para los nombres de las cosas en este TP fue un problema (╯°□°)╯︵ ┻━┻

	/*
	 * calcula si hay time suficiente hasta el proximo tick
	*/
	TickType_t lastTick;

	taskENTER_CRITICAL();
	lastTick = tick.stamp;
	taskEXIT_CRITICAL();

	TickType_t currentTime = xTaskGetTickCount();
	return (lastTick + TICK_PERIOD > currentTime + time);

}

static void task_tick_(void* argument)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(TICK_PERIOD);


    char tx_buffer[TX_MSG_BUFFER_SIZE_];

    while (1)
    {
    	msg_tick_create(&tick, xLastWakeTime);
    	if (0 == msg_tick_write(tx_buffer, &tick)){
    		//driver_uart_tx((uint8_t*)tx_buffer, strlen(tx_buffer));
    		notify_pending();
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
