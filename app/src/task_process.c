#include "task_process.h"
#include "protocol.h"
#include "task_outbound.h"


/********************** macros and definitions *******************************/
#define TASK_STACK_SIZE_        (512)
#define PROCESS_BATCH_DELAY_MS_ (20)

extern QueueHandle_t inbound_queue_h;

MsgRequest_t msgBuffer[MAX_MSG];
uint32_t msgCount = 0;

/********************** internal functions declaration ***********************/

/********************** internal functions definition ************************/

static bool insertMsg(MsgRequest_t nuevo) // ordena los mensajes
{
    if (msgCount >= MAX_MSG) {
    	return false;
    }

    int i = (int)msgCount - 1;

    while (i >= 0 && msgBuffer[i].priority < nuevo.priority) {
        msgBuffer[i + 1] = msgBuffer[i];
        i--;
    }

    msgBuffer[i + 1] = nuevo;
    msgCount++;

    return true;
}

bool task_process_get_next_msg(MsgRequest_t *msg)
{
	if (msg == NULL) {
		return false;
	}

	taskENTER_CRITICAL();
    if (msgCount == 0) {
    	taskEXIT_CRITICAL();
    	return false;
    }

    *msg = msgBuffer[0];
    for (int i = 1; i < msgCount; i++) {
        msgBuffer[i - 1] = msgBuffer[i];
    }

    msgCount--;
    taskEXIT_CRITICAL();

    return true;
}

/********************** internal task ***************************************/

static void task_process_(void* argument)
{

    while (1)
    {
    	MsgRequest_t newMsg;
    	uint32_t inserted_count = 0;

		if (pdPASS == xQueueReceive(inbound_queue_h, &newMsg, portMAX_DELAY))
		{
			vTaskDelay(pdMS_TO_TICKS(PROCESS_BATCH_DELAY_MS_)); //

			do
			{
				bool inserted;

				taskENTER_CRITICAL();
				inserted = insertMsg(newMsg);
				taskEXIT_CRITICAL();

				if (inserted)
				{
					inserted_count++;
				}
			}
			while (pdPASS == xQueueReceive(inbound_queue_h, &newMsg, 0));

			if (inserted_count > 0)
			{
				task_outbound_notify_pending();
			}
		}
    }
}

/********************** public API ******************************************/

void task_process_init(void)
{
    BaseType_t status;


    status = xTaskCreate(
        task_process_,
        "task_process",
		TASK_STACK_SIZE_,
        NULL,
        tskIDLE_PRIORITY,
        NULL
    );
    while (pdPASS != status)
    {
        // manejo de error (opcional: log, assert, etc.)
    }
}
