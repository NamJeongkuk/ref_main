/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BufferedUart_Sci0.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "Gpio_Ra.h"
#include "uassert.h"
#include "utils.h"
#include "r_sci_uart.h"
#include "r_dtc.h"

#define DTC_RECEIVE_BUFFER_SIZE_BYTES (50)
#define TRANSFER_BUFFER_SIZE_BYTES (50)
#define RECEIVE_REPEAT_COUNT (uint16_t)((DTC_RECEIVE_BUFFER_SIZE_BYTES << 8) | DTC_RECEIVE_BUFFER_SIZE_BYTES)

static struct
{
   I_BufferedUart_t interface;

   Event_SingleSubscriberSynchronous_t onReceiveEvent;
   Event_SingleSubscriberSynchronous_t onTransmitCompleteEvent;
   bool waitingForTransmitToComplete;
   bool transmitComplete;
   uint8_t dtcReceiveBuffer[DTC_RECEIVE_BUFFER_SIZE_BYTES];
   uint8_t dtcRingBufferTail;
} instance;

static sci_uart_instance_ctrl_t uartControl;
static dtc_instance_ctrl_t dtcTransmitControl;
static dtc_instance_ctrl_t dtcReceiveControl;

static transfer_info_t dtcTransmitInfo = {
   .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
   .repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
   .irq = TRANSFER_IRQ_END,
   .chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
   .src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
   .size = TRANSFER_SIZE_1_BYTE,
   .mode = TRANSFER_MODE_NORMAL,
   .p_dest = (void *)NULL,
   .p_src = (void const *)NULL,
   .num_blocks = 0,
   .length = 0
};

static const dtc_extended_cfg_t dtcTransmitExtendedConfig = {
   .activation_source = VECTOR_NUMBER_SCI0_TXI,
};

static const transfer_cfg_t dtcTransmitConfig = {
   .p_info = &dtcTransmitInfo,
   .p_extend = &dtcTransmitExtendedConfig,
};

static const transfer_instance_t dtcTransmitInstance = {
   .p_ctrl = &dtcTransmitControl,
   .p_cfg = &dtcTransmitConfig,
   .p_api = &g_transfer_on_dtc
};

static transfer_info_t dtcReceiveInfo = {
   .dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
   .repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
   .irq = TRANSFER_IRQ_END,
   .chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
   .src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
   .size = TRANSFER_SIZE_1_BYTE,
   .mode = TRANSFER_MODE_REPEAT,
   .p_dest = (void *)NULL,
   .p_src = (void const *)NULL,
   .num_blocks = 0,
   .length = RECEIVE_REPEAT_COUNT
};

static const dtc_extended_cfg_t dtcReceiveExtendedConfig = {
   .activation_source = VECTOR_NUMBER_SCI0_RXI,
};

static const transfer_cfg_t dtcReceiveConfig = {
   .p_info = &dtcReceiveInfo,
   .p_extend = &dtcReceiveExtendedConfig,
};

static const transfer_instance_t dtcReceiveInstance = {
   .p_ctrl = &dtcReceiveControl,
   .p_cfg = &dtcReceiveConfig,
   .p_api = &g_transfer_on_dtc
};

static void Transmit(I_BufferedUart_t *_instance, const uint8_t *data, uint16_t byteCount)
{
   IGNORE(_instance);
   IGNORE(data);
   IGNORE(byteCount);

   instance.waitingForTransmitToComplete = true;
   instance.transmitComplete = false;

   fsp_err_t result = R_SCI_UART_Write(&uartControl, data, byteCount);
   uassert(FSP_SUCCESS == result);
}

static I_Event_t *GetOnTransmitCompleteEvent(I_BufferedUart_t *_instance)
{
   IGNORE(instance);
   return &instance.onTransmitCompleteEvent.interface;
}

static I_Event_t *GetOnReceiveEvent(I_BufferedUart_t *_instance)
{
   IGNORE(_instance);
   return &instance.onReceiveEvent.interface;
}

static uint16_t GetRingBufferQueueCount(void)
{
   uint16_t capacity = sizeof(instance.dtcReceiveBuffer);
   uint8_t dtcRingBufferHead = (capacity - (dtcReceiveInfo.length & 0xFF)) % sizeof(instance.dtcReceiveBuffer);

   if(dtcRingBufferHead >= instance.dtcRingBufferTail)
   {
      return (dtcRingBufferHead - instance.dtcRingBufferTail);
   }
   else
   {
      return capacity - (instance.dtcRingBufferTail - dtcRingBufferHead);
   }
}

static void Run(I_BufferedUart_t *_instance)
{
   IGNORE_ARG(_instance);

   if(instance.waitingForTransmitToComplete && instance.transmitComplete)
   {
      instance.waitingForTransmitToComplete = false;
      Event_SingleSubscriberSynchronous_Publish(&instance.onTransmitCompleteEvent, NULL);
   }

   {
      uint8_t localData[TRANSFER_BUFFER_SIZE_BYTES];
      uint16_t queueCount = GetRingBufferQueueCount();
      uint8_t index = 0;

      while((queueCount--) && (index < TRANSFER_BUFFER_SIZE_BYTES))
      {
         localData[index++] = instance.dtcReceiveBuffer[instance.dtcRingBufferTail];
         instance.dtcRingBufferTail = (instance.dtcRingBufferTail + 1) % sizeof(instance.dtcReceiveBuffer);
      }

      if(index > 0)
      {
         BufferedUartOnReceiveArgs_t args = { localData, index };
         Event_SingleSubscriberSynchronous_Publish(&instance.onReceiveEvent, &args);
      }
   }
}

static void EventOccurred(uart_callback_args_t *args)
{
   switch(args->event)
   {
      case UART_EVENT_TX_COMPLETE:
         instance.transmitComplete = true;
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
   .channel = 0,
   .data_bits = UART_DATA_BITS_8,
   .parity = UART_PARITY_OFF,
   .stop_bits = UART_STOP_BITS_1,
   .p_callback = EventOccurred,
   .p_context = NULL,
   .p_extend = &extendedConfiguration,
   .p_transfer_tx = &dtcTransmitInstance,
   .p_transfer_rx = &dtcReceiveInstance,
   .rxi_ipl = 12,
   .txi_ipl = 12,
   .tei_ipl = 12,
   .eri_ipl = 12,
   .rxi_irq = VECTOR_NUMBER_SCI0_RXI,
   .txi_irq = VECTOR_NUMBER_SCI0_TXI,
   .tei_irq = VECTOR_NUMBER_SCI0_TEI,
   .eri_irq = VECTOR_NUMBER_SCI0_ERI
};

static const I_BufferedUart_Api_t api = {
   Transmit,
   GetOnTransmitCompleteEvent,
   GetOnReceiveEvent,
   Run
};

I_BufferedUart_t *BufferedUart_Sci0_Init(void)
{
   Event_SingleSubscriberSynchronous_Init(&instance.onReceiveEvent);
   Event_SingleSubscriberSynchronous_Init(&instance.onTransmitCompleteEvent);

   instance.interface.api = &api;

   Gpio_Ra_ConfigurePin(BSP_IO_PORT_04_PIN_10, (IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_SCI0_2_4_6_8));
   Gpio_Ra_ConfigurePin(BSP_IO_PORT_04_PIN_11, (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_SCI0_2_4_6_8));

   fsp_err_t result = R_SCI_UART_Open(&uartControl, &configuration);

   // The FSP sets the transfer mode to "Normal". We want it to be "Repeat".
   dtcReceiveInfo.mode = TRANSFER_MODE_REPEAT;
   result |= R_SCI_UART_Read(&uartControl, instance.dtcReceiveBuffer, RECEIVE_REPEAT_COUNT);

   uassert(FSP_SUCCESS == result);

   return &instance.interface;
}
