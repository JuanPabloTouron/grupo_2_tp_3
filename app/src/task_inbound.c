#include "task_inbound.h"
#include "driver_uart.h"
#include "protocol.h"
#include "queue.h"

/********************** macros and definitions *******************************/

#define RX_READ_BUFFER_SIZE_    (128)
#define RX_PARSER_BUFFER_SIZE_  (BUFFER_MAX)
#define RX_POLL_DELAY_MS_       (1)
#define TASK_STACK_SIZE_        (1024)

/********************** external data declaration ****************************/

extern QueueHandle_t inbound_queue_h;

/********************** internal functions declaration ***********************/

static void on_msg(ProtocolParser_t *parser);
static void queue_request(MsgRequest_t *request);

/********************** internal functions definition ************************/

static void on_msg(ProtocolParser_t *parser)
{
    MsgRequest_t request;


    if (msg_request_parser(parser->buffer, &request) != 0) // si msg_request_parser() devuelve algo que no sea 0 algo salio mal
    {
        return;
    }

    if (request.head.type != MSG_TYPE_REQUEST) // valida que efectivamente sea un request
    {
        return;
    }

    queue_request(&request); // envía a la queue
}

static void queue_request(MsgRequest_t *request)
{
    if (pdPASS == xQueueSend(inbound_queue_h, request, 0))
    {
        return;
    }

    MsgRequest_t discardedRequest; // si la cola se llena  de mensajes, sacamos el mas viejo y cargamos uno nuevo
    (void)xQueueReceive(inbound_queue_h, &discardedRequest, 0);
    (void)xQueueSend(inbound_queue_h, request, 0);
}

/********************** internal task ***************************************/

/*
 * Esta tarea se encarga de recibir mensajes, parsearlos y entregarlos a una queue para ser procesados y ordenados
 */

static void task_inbound_(void* argument)
{

	uint8_t rx_buffer[RX_READ_BUFFER_SIZE_];
    char parser_buffer[RX_PARSER_BUFFER_SIZE_];
    ProtocolParser_t parser;

    protocol_parser_init(&parser, parser_buffer, sizeof(parser_buffer));

    while (1)
    {
        size_t rx_len = driver_uart_rx(rx_buffer, sizeof(rx_buffer));

        for (size_t i = 0; i < rx_len; i++)
        {
            (void)protocol_parser_feed(&parser, (char)rx_buffer[i], on_msg);
        }

        vTaskDelay((TickType_t)(RX_POLL_DELAY_MS_ / portTICK_PERIOD_MS)); // 1ms
    }
}

/********************** public API ******************************************/

void task_inbound_init(void)
{
    BaseType_t status;

    status = xTaskCreate(
        task_inbound_,
        "task_inbound",
        TASK_STACK_SIZE_,
        NULL,
        tskIDLE_PRIORITY+1,
        NULL
    );

    while (pdPASS != status)
    {
        // manejo de error (opcional: log, assert, etc.)
    }
}
