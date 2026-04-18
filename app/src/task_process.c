#include "task_process.h"
#include "protocol.h"
#include "linked_list.h"


/********************** macros and definitions *******************************/
#define TASK_STACK_SIZE_        (512)
#define PROCESS_BATCH_DELAY_MS_ (10)

extern QueueHandle_t inbound_queue_h;
extern QueueHandle_t outbound_queue_h;


linked_list_t orderedList;
/********************** internal functions declaration ***********************/

/********************** internal functions definition ************************/

/********************** internal task ***************************************/

static void task_process_(void* argument)
{
    MsgRequest_t request;
    linked_list_node_t node;


    while (1)
    {
		if (pdPASS == xQueueReceive(inbound_queue_h, &request, portMAX_DELAY))
		{

			linked_list_node_init(&node,&request);
			//linked_list_node_add(&orderedList, &node);
			linked_list_node_ordered_add(&orderedList, &node);
			vTaskDelay((TickType_t)(PROCESS_BATCH_DELAY_MS_ / portTICK_PERIOD_MS));
		}
    }
}

/********************** public API ******************************************/

void task_process_init(task_process_args_t *args)
{
    BaseType_t status;


    status = xTaskCreate(
        task_process_,
        "task_process",
		TASK_STACK_SIZE_,
        (void*)args,
        tskIDLE_PRIORITY,
        NULL
    );

    linked_list_init(&orderedList);


    while (pdPASS != status)
    {
        // manejo de error (opcional: log, assert, etc.)
    }
}
