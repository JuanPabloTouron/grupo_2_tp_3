#include "task_tick.h"
#include "driver_uart.h"
#include <string.h>

/********************** internal task ***************************************/

static void task_tick_(void* argument)
{
    task_tick_args_t *pargs = (task_tick_args_t*)argument;

    while (1)
    {
        size_t msg_len = strlen(pargs->msg);
        driver_uart_tx((uint8_t*)pargs->msg, msg_len);

        vTaskDelay((TickType_t)(pargs->period_ms / portTICK_PERIOD_MS));
    }
}

/********************** public API ******************************************/

void task_tick_init(task_tick_args_t *args)
{
    BaseType_t status;

    status = xTaskCreate(
        task_tick_,
        "task_tick",
        128,
        (void*)args,
        tskIDLE_PRIORITY,
        NULL
    );

    while (pdPASS != status)
    {
        // manejo de error (opcional: log, assert, etc.)
    }
}
