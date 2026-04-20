#include "task_process.h"
#include "protocol.h"
#include "task_outbound.h"


/********************** macros and definitions *******************************/
#define TASK_STACK_SIZE_        (512)
#define PROCESS_BATCH_DELAY_MS_ (10)

extern QueueHandle_t inbound_queue_h;

MsgRequest_t msgBuffer[MAX_MSG];
uint32_t msgCount = 0;

SemaphoreHandle_t listMutex;

/********************** internal functions declaration ***********************/

/********************** internal functions definition ************************/

static bool insertMsg(MsgRequest_t new) // ordena los mensajes
{
    if (msgCount >= MAX_MSG) {
    	return false;
    }

    uint32_t i = msgCount;

    while (i > 0 && msgBuffer[i - 1].priority < new.priority) {
        msgBuffer[i] = msgBuffer[i - 1];
        i--;
    }

    msgBuffer[i] = new;
    msgCount++;

    return true;
}

bool getNextMsg(MsgRequest_t *msg) // al extraer el mensaje de mayor prioridad hay que reacomodar la lista
{
	if (msg == NULL) {
		return false;
	}

	/*
	 * msgBuffer y msgCount son compartidos por task_process y task_outbound.
	 * se protegen con un mutex para evitar que una tarea modifique la lista
	 * mientras la otra la esta recorriendo o actualizando
	 */

	xSemaphoreTake(listMutex,portMAX_DELAY);
    if (msgCount == 0) {
    	xSemaphoreGive(listMutex);
    	return false;
    }

    *msg = msgBuffer[0];
    for (uint32_t i = 1; i < msgCount; i++) {
        msgBuffer[i - 1] = msgBuffer[i];
    }

    msgCount--;
    xSemaphoreGive(listMutex);

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

				/*
				 * msgBuffer y msgCount son compartidos por task_process y task_outbound
				 * se protegen con un mutex para evitar que una tarea modifique la lista
				 * mientras la otra la esta recorriendo o actualizando.
				 */

				xSemaphoreTake(listMutex,portMAX_DELAY);
				inserted = insertMsg(newMsg);
				xSemaphoreGive(listMutex);

				if (inserted)
				{
					inserted_count++;
				}

			}while (pdPASS == xQueueReceive(inbound_queue_h, &newMsg, 0));

			if (inserted_count > 0) // si se inserto al menos un mensaje despertamos a task_outbound
			{
				notify_pending();
			}
		}
    }
}

/********************** public API ******************************************/

void task_process_init(void)
{
    BaseType_t status;

    listMutex = xSemaphoreCreateMutex();

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
