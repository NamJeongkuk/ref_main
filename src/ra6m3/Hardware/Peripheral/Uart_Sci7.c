/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Uart_Sci7.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "Gpio_Ra.h"
#include "uassert.h"
#include "utils.h"
#include "r_sci_uart.h"

static sci_uart_instance_ctrl_t uartControl;
static Event_SingleSubscriberSynchronous_t onReceiveEvent;
static Event_SingleSubscriberSynchronous_t sendCompleteEvent;
static Event_SingleSubscriberSynchronous_t errorEvent;
static I_Uart_t uart;

static void Send(I_Uart_t *instance, const uint8_t byte)
{
   IGNORE(instance);
   IGNORE(byte);

   fsp_err_t result = R_SCI_UART_Write(&uartControl, &byte, sizeof(byte));
   uassert(FSP_SUCCESS == result);
}

static I_Event_t *GetOnReceiveEvent(I_Uart_t *instance)
{
   IGNORE(instance);
   return &onReceiveEvent.interface;
}

static I_Event_t *GetOnTransmitEvent(I_Uart_t *instance)
{
   IGNORE(instance);
   return &sendCompleteEvent.interface;
}

static I_Event_t *GetOnErrorEvent(I_Uart_t *instance)
{
   IGNORE(instance);
   return &errorEvent.interface;
}

static void ReceiveComplete(uint8_t receivedByte)
{
   UartOnReceiveArgs_t args = { receivedByte };
   Event_SingleSubscriberSynchronous_Publish(&onReceiveEvent, &args);
}

static void TransmitComplete(void)
{
   Event_SingleSubscriberSynchronous_Publish(&sendCompleteEvent, NULL);
}

static void ReceiveError(void)
{
   Event_SingleSubscriberSynchronous_Publish(&errorEvent, NULL);
}

static void EventOccurred(uart_callback_args_t *args)
{
   switch(args->event)
   {
      case UART_EVENT_RX_CHAR:
         ReceiveComplete((uint8_t)args->data);
         break;

      case UART_EVENT_TX_COMPLETE:
         TransmitComplete();
         break;

      case UART_EVENT_ERR_PARITY:
      case UART_EVENT_ERR_FRAMING:
      case UART_EVENT_ERR_OVERFLOW:
      case UART_EVENT_BREAK_DETECT:
         ReceiveError();
         break;

      default:
         break;
   }
}

static baud_setting_t baudSetting = {
   .abcse = 0,
   .abcs = 0,
   .bgdm = 0,
   .cks = 0,
   .brr = 10,
   .mddr = 173,
   .brme = true
};

static const sci_uart_extended_cfg_t extendedConfiguration = {
   .clock = SCI_UART_CLOCK_INT,
   .rx_edge_start = SCI_UART_START_BIT_LOW_LEVEL,
   .noise_cancel = SCI_UART_NOISE_CANCELLATION_DISABLE,
   .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX,
   .p_baud_setting = &baudSetting,
   .uart_mode = UART_MODE_RS232,
   .ctsrts_en = SCI_UART_CTSRTS_RTS_OUTPUT,
   .flow_control_pin = (bsp_io_port_pin_t)(0xFFFFU),
};

static const uart_cfg_t configuration = {
   .channel = 7,
   .data_bits = UART_DATA_BITS_8,
   .parity = UART_PARITY_OFF,
   .stop_bits = UART_STOP_BITS_1,
   .p_callback = EventOccurred,
   .p_context = NULL,
   .p_extend = &extendedConfiguration,
   .p_transfer_tx = NULL,
   .p_transfer_rx = NULL,
   .rxi_ipl = 12,
   .txi_ipl = 12,
   .tei_ipl = 12,
   .eri_ipl = 12,
   .rxi_irq = VECTOR_NUMBER_SCI7_RXI,
   .txi_irq = VECTOR_NUMBER_SCI7_TXI,
   .tei_irq = VECTOR_NUMBER_SCI7_TEI,
   .eri_irq = VECTOR_NUMBER_SCI7_ERI
};

static const I_Uart_Api_t api = { Send, GetOnReceiveEvent, GetOnTransmitEvent, GetOnErrorEvent };

I_Uart_t *Uart_Sci7_Init(void)
{
   Event_SingleSubscriberSynchronous_Init(&onReceiveEvent);
   Event_SingleSubscriberSynchronous_Init(&sendCompleteEvent);
   Event_SingleSubscriberSynchronous_Init(&errorEvent);

   uart.api = &api;

   Gpio_Ra_ConfigurePin(BSP_IO_PORT_06_PIN_13, (IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_SCI1_3_5_7_9));
   Gpio_Ra_ConfigurePin(BSP_IO_PORT_06_PIN_14, (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_SCI1_3_5_7_9));

   fsp_err_t result = R_SCI_UART_Open(&uartControl, &configuration);
   uassert(FSP_SUCCESS == result);

   return &uart;
}
