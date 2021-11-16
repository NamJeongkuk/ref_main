/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BufferedUart_Scb5.h"
#include "Event_Synchronous.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "Gpio_Psoc6.h"
#include "uassert.h"
#include "utils.h"
#include "cy_pdl.h"

#define BASE_PERIPHERAL (SCB5)

enum
{
   ReceiveBufferSize = CY_SCB_FIFO_SIZE,
   RefillTxBufferThreshold = CY_SCB_FIFO_SIZE - 1,
};

static struct
{
   I_BufferedUart_t interface;
   Event_Synchronous_t receiveEvent;
   Event_SingleSubscriberSynchronous_t transmitEvent;
   bool errorOccurred;
   struct
   {
      bool inProgress;
      const uint8_t *data;
      uint16_t bytesRemaining;
   } transmit;
} instance;

static cy_stc_scb_uart_context_t uartContext;

static void LoadHardwareTransmitBuffer(void)
{
   uint16_t bytesSent = (uint16_t)Cy_SCB_UART_PutArray(
      BASE_PERIPHERAL,
      (uint8_t *)instance.transmit.data,
      instance.transmit.bytesRemaining);

   uassert(bytesSent <= instance.transmit.bytesRemaining);
   instance.transmit.bytesRemaining -= bytesSent;
}

static void Transmit(I_BufferedUart_t *_instance, const uint8_t *data, const uint16_t byteCount)
{
   IGNORE(_instance);
   uassert(!instance.transmit.inProgress);

   instance.transmit.inProgress = true;
   instance.transmit.data = data;
   instance.transmit.bytesRemaining = byteCount;

   LoadHardwareTransmitBuffer();
}

static I_Event_t *GetOnReceiveEvent(I_BufferedUart_t *_instance)
{
   IGNORE_ARG(_instance);
   return &instance.receiveEvent.interface;
}

static I_Event_t *GetOnTransmitEvent(I_BufferedUart_t *_instance)
{
   IGNORE_ARG(_instance);
   return &instance.transmitEvent.interface;
}

static void ProcessBytesReceived(void)
{
   uint8_t localData[ReceiveBufferSize];

   uint32_t numRead = Cy_SCB_UART_GetArray(BASE_PERIPHERAL, (void *)&localData, ReceiveBufferSize);

   if(numRead > 0)
   {
      BufferedUartOnReceiveArgs_t args = { localData, numRead };
      Event_Synchronous_Publish(&instance.receiveEvent, &args);
   }
}

static bool ThereAreBytesInTheSoftwareTxBuffer(void)
{
   return instance.transmit.bytesRemaining > 0;
}

static bool TheHardwareBufferHasSpace(void)
{
   return Cy_SCB_GetNumInTxFifo(BASE_PERIPHERAL) <= RefillTxBufferThreshold;
}

static bool TheHardwareBufferIsEmpty(void)
{
   return Cy_SCB_GetNumInTxFifo(BASE_PERIPHERAL) == 0;
}

static void Run(I_BufferedUart_t *_instance)
{
   IGNORE(_instance);

   ProcessBytesReceived();

   if(instance.transmit.inProgress)
   {
      if(ThereAreBytesInTheSoftwareTxBuffer() && TheHardwareBufferHasSpace())
      {
         LoadHardwareTransmitBuffer();
      }
      else if(TheHardwareBufferIsEmpty())
      {
         instance.transmit.inProgress = false;
         Event_SingleSubscriberSynchronous_Publish(&instance.transmitEvent, NULL);
      }
   }
}

static const I_BufferedUart_Api_t uartApi = {
   .Transmit = Transmit,
   .GetOnReceiveEvent = GetOnReceiveEvent,
   .GetOnTransmitCompleteEvent = GetOnTransmitEvent,
   .Run = Run
};

static const cy_stc_gpio_pin_config_t rxPinConfig = {
   .outVal = 1,
   .driveMode = CY_GPIO_DM_HIGHZ,
   .hsiom = P5_0_SCB5_UART_RX,
   .intEdge = CY_GPIO_INTR_DISABLE,
   .intMask = 0UL,
   .vtrip = CY_GPIO_VTRIP_CMOS,
   .slewRate = CY_GPIO_SLEW_FAST,
   .driveSel = CY_GPIO_DRIVE_1_2,
   .vregEn = 0UL,
   .ibufMode = 0UL,
   .vtripSel = 0UL,
   .vrefSel = 0UL,
   .vohSel = 0UL,
};

static const cy_stc_gpio_pin_config_t txPinConfig = {
   .outVal = 1,
   .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
   .hsiom = P5_1_SCB5_UART_TX,
   .intEdge = CY_GPIO_INTR_DISABLE,
   .intMask = 0UL,
   .vtrip = CY_GPIO_VTRIP_CMOS,
   .slewRate = CY_GPIO_SLEW_FAST,
   .driveSel = CY_GPIO_DRIVE_1_2,
   .vregEn = 0UL,
   .ibufMode = 0UL,
   .vtripSel = 0UL,
   .vrefSel = 0UL,
   .vohSel = 0UL,
};

static const cy_stc_scb_uart_config_t uartConfig = {
   .uartMode = CY_SCB_UART_STANDARD,
   .enableMutliProcessorMode = false,
   .smartCardRetryOnNack = false,
   .irdaInvertRx = false,
   .irdaEnableLowPowerReceiver = false,
   .oversample = 8,
   .enableMsbFirst = false,
   .dataWidth = 8UL,
   .parity = CY_SCB_UART_PARITY_NONE,
   .stopBits = CY_SCB_UART_STOP_BITS_1,
   .enableInputFilter = false,
   .breakWidth = 11UL,
   .dropOnFrameError = false,
   .dropOnParityError = false,
   .receiverAddress = 0x0UL,
   .receiverAddressMask = 0x0UL,
   .acceptAddrInFifo = false,
   .enableCts = false,
   .ctsPolarity = CY_SCB_UART_ACTIVE_LOW,
   .rtsRxFifoLevel = 0UL,
   .rtsPolarity = CY_SCB_UART_ACTIVE_LOW,
   .rxFifoTriggerLevel = 0UL,
   .rxFifoIntEnableMask = 0UL,
   .txFifoTriggerLevel = 127UL,
   .txFifoIntEnableMask = 0UL,
};

static void Initialize(void)
{
   Gpio_Psoc6_ConfigurePin(5, 0, &rxPinConfig);
   Gpio_Psoc6_ConfigurePin(5, 1, &txPinConfig);

   Cy_SysClk_PeriphDisableDivider(CY_SYSCLK_DIV_16_5_BIT, 0U);
   Cy_SysClk_PeriphSetFracDivider(CY_SYSCLK_DIV_16_5_BIT, 0U, 54U, 8U);
   Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_16_5_BIT, 0U);
   Cy_SysClk_PeriphAssignDivider(PCLK_SCB5_CLOCK, CY_SYSCLK_DIV_16_5_BIT, 0U);

   cy_en_scb_uart_status_t status = Cy_SCB_UART_Init(BASE_PERIPHERAL, &uartConfig, &uartContext);
   uassert(status == CY_SCB_UART_SUCCESS);

   Cy_SCB_UART_Enable(BASE_PERIPHERAL);
}

I_BufferedUart_t *BufferedUart_Scb5_Init(void)
{
   instance.interface.api = &uartApi;

   Event_Synchronous_Init(&instance.receiveEvent);
   Event_SingleSubscriberSynchronous_Init(&instance.transmitEvent);

   Initialize();

   return &instance.interface;
}
