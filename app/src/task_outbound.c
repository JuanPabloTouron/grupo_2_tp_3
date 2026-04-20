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
#define TX_BUFFER_TIME			(25)
#define MAX_MSG_PER_TICK        (3)
/********************** internal data declaration ****************************/
static TaskHandle_t task_outbound_h;
/********************** internal task ***************************************/
extern MsgTick_t tick;

/*
 * Esta tarea procesa los mensajes ordenados según la prioridad, genera las respuestas correspondientes y las envía
 * Además esta tarea recibe los ticks para ser enviados periodicamente
 */

static void task_outbound_(void* argument)
{
    task_outbound_args_t *pargs = (task_outbound_args_t*)argument;
    (void)pargs;


    char tx_buffer[TX_MSG_BUFFER_SIZE_];

    MsgTick_t lastTick = {0};
    MsgTick_t localTick = {0};

	MsgRequest_t next;

	MsgResponse_t response;

	uint32_t msgsSent = 0;

    while (1)
    {
    	(void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    	taskENTER_CRITICAL();
    	localTick = tick;
    	taskEXIT_CRITICAL();

    	if(localTick.stamp != lastTick.stamp){
    		msg_tick_write(tx_buffer, &localTick);
    		driver_uart_tx((uint8_t*)tx_buffer, strlen(tx_buffer));
    		lastTick = localTick;
    		msgsSent = 0;
    	}

    	/*
    	 * limitamos la cantidad de mensajes que se pueden mandar entre ticks teniendo en cuenta el tiempo que demora en enviarse un MSG-RESPONSE
    	 * estimamos que debería demorar alrededor de 25ms en enviarse por lo que limitamos la cantidad de mensajes a 3.
    	 */

    	while(msgsSent < MAX_MSG_PER_TICK)
    	{
    	    if(getNextMsg(&next)){
    	        msg_response_create(&response, next.id);

    	        if (0 == msg_response_write(tx_buffer, &response))
    	        {
    	            size_t msg_len = strlen(tx_buffer);
    	            driver_uart_tx((uint8_t*)tx_buffer, msg_len);
    	            msgsSent++;
    	        }
    	    }
    	    else
    	    {
    	        break;
    	    }
    	}
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
        // manejo de errore
    }
}

void notify_pending(void)
{
	if (task_outbound_h != NULL)
	{
		(void)xTaskNotifyGive(task_outbound_h);
	}
}
