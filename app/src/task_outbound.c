/********************** inclusions *******************************************/
#include "task_outbound.h"
#include "driver_uart.h"
#include "protocol.h"
#include "queue.h"
#include "linked_list.h"
#include <string.h>

/********************** macros and definitions *******************************/
#define OUTBOUND_QUEUE_SIZE_	(10)
#define TX_MSG_BUFFER_SIZE_		(300)
#define TASK_STACK_SIZE_		(512)

/********************** internal data declaration ****************************/
QueueHandle_t outbound_queue_h;
extern linked_list_t orderedList;
/********************** internal task ***************************************/

static void task_outbound_(void* argument)
{
	linked_list_node_t* node;
    task_outbound_args_t *pargs = (task_outbound_args_t*)argument;
    (void)pargs;

    MsgResponse_t response;
    char tx_buffer[TX_MSG_BUFFER_SIZE_];

    while (1)
    {
    	if ((node = linked_list_node_remove(&orderedList))!=NULL){
    		msg_response_create(&response, node->pdata->id);

    		if (0 == msg_response_write(tx_buffer, &response))
            {
                size_t msg_len = strlen(tx_buffer);
                (void)driver_uart_tx((uint8_t*)tx_buffer, msg_len);
            }
        }
    }
}

/********************** public API ******************************************/

void task_outbound_init(task_outbound_args_t *args)
{

	outbound_queue_h = xQueueCreate(OUTBOUND_QUEUE_SIZE_,sizeof(MsgResponse_t));

    BaseType_t status;

    status = xTaskCreate(
        task_outbound_,
        "task_outbound",
        TASK_STACK_SIZE_,
        (void*)args,
        tskIDLE_PRIORITY,
        NULL
    );

    while (pdPASS != status)
    {
        // manejo de error (opcional: log, assert, etc.)
    }
}
