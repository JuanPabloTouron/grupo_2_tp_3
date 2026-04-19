/********************** inclusions *******************************************/
#include "task_outbound.h"
#include "driver_uart.h"
#include "task_process.h"
#include "task_tick.h"
#include <string.h>

/********************** macros and definitions *******************************/
#define TX_MSG_BUFFER_SIZE_		(300)
#define TASK_STACK_SIZE_		(512)
#define TX_RETRY_DELAY_MS_		(1)

/********************** internal data declaration ****************************/
static TaskHandle_t task_outbound_h;
/********************** internal task ***************************************/

extern MsgTick_t tick;

static void uart_tx_all_(uint8_t *buffer, size_t size)
{
	size_t sent = 0;

	while (sent < size)
	{
		size_t written = driver_uart_tx(buffer + sent, size - sent);

		if (written > 0)
		{
			sent += written;
		}
		else
		{
			vTaskDelay(pdMS_TO_TICKS(TX_RETRY_DELAY_MS_));
		}
	}
}


static void task_outbound_(void* argument)
{
    task_outbound_args_t *pargs = (task_outbound_args_t*)argument;
    (void)pargs;


    char tx_buffer[TX_MSG_BUFFER_SIZE_];

    MsgTick_t lastTick = {0};
    MsgTick_t localTick = {0};

	MsgRequest_t next;

	MsgResponse_t response;

    while (1)
    {
    	(void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    	taskENTER_CRITICAL();
    	localTick = tick;
    	taskEXIT_CRITICAL();

    	if(localTick.stamp != lastTick.stamp){
    		msg_tick_write(tx_buffer, &tick);
    		uart_tx_all_((uint8_t*)tx_buffer, strlen(tx_buffer));
    		lastTick = tick;
    	}

    	do{
    		if(task_process_get_next_msg(&next)){
    			msg_response_create(&response, next.id);

    			if (0 == msg_response_write(tx_buffer, &response))
    			{
    				size_t msg_len = strlen(tx_buffer);
    				uart_tx_all_((uint8_t*)tx_buffer, msg_len);
    			}
    		}



    	}while(tick.stamp == lastTick.stamp);

    }
}


/********************** public API ******************************************/

void task_outbound_init(void)
{
    BaseType_t status;

    status = xTaskCreate(
        task_outbound_,
        "task_outbound",
        TASK_STACK_SIZE_,
        NULL,
        tskIDLE_PRIORITY,
        &task_outbound_h
    );

    while (pdPASS != status)
    {
        // manejo de error (opcional: log, assert, etc.)
    }
}

void task_outbound_notify_pending(void)
{
	if (task_outbound_h != NULL)
	{
		(void)xTaskNotifyGive(task_outbound_h);
	}
}
