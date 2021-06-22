/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Uart_Scb2.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "Gpio_Psoc6.h"
#include "uassert.h"
#include "utils.h"
#include "cy_pdl.h"

#define BASE_PERIPHERAL (SCB2)

enum
{
   BaudRate = 19200,
   Oversample = 8,
};

static struct
{
   I_Uart_t interface;
   Event_SingleSubscriberSynchronous_t receiveEvent;
   Event_SingleSubscriberSynchronous_t transmitEvent;
   Event_SingleSubscriberSynchronous_t errorEvent;
   uint8_t receivedByte;
} instance;

static cy_stc_scb_uart_context_t uartContext;

static const cy_stc_gpio_pin_config_t rxPinConfig = {
   .outVal = 1,
   .driveMode = CY_GPIO_DM_HIGHZ,
   .hsiom = P3_0_SCB2_UART_RX,
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
   .hsiom = P3_1_SCB2_UART_TX,
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
   .oversample = Oversample,
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
   .rxFifoIntEnableMask = (CY_SCB_UART_RX_ERR_FRAME | CY_SCB_UART_RX_ERR_PARITY | CY_SCB_UART_RX_TRIGGER | CY_SCB_UART_RX_FULL | CY_SCB_UART_RX_OVERFLOW | CY_SCB_UART_RX_UNDERFLOW),
   .txFifoTriggerLevel = 63UL,
   .txFifoIntEnableMask = CY_SCB_UART_TX_DONE
};

static const cy_stc_sysint_t uartInterruptConfig = {
   .intrSrc = scb_2_interrupt_IRQn,
   .intrPriority = 3u,
};

static void Send(I_Uart_t *instance, uint8_t byte)
{
   IGNORE(instance);

   // This may be called from RX ISR, in which case TX has not been handled
   // Calling Interrupt again allows previous TX to be handled
   Cy_SCB_UART_Interrupt(BASE_PERIPHERAL, &uartContext);

   Cy_SCB_UART_Transmit(BASE_PERIPHERAL, &byte, 1, &uartContext);
}

static I_Event_t *GetOnReceiveEvent(I_Uart_t *_instance)
{
   IGNORE(_instance);
   return &instance.receiveEvent.interface;
}

static I_Event_t *GetOnTransmitEvent(I_Uart_t *_instance)
{
   IGNORE(_instance);
   return &instance.transmitEvent.interface;
}

static I_Event_t *GetOnErrorEvent(I_Uart_t *_instance)
{
   IGNORE(_instance);
   return &instance.errorEvent.interface;
}

static const I_Uart_Api_t api = { Send, GetOnReceiveEvent, GetOnTransmitEvent, GetOnErrorEvent };

static void TransmitComplete(void)
{
   Event_SingleSubscriberSynchronous_Publish(&instance.transmitEvent, NULL);
}

static void ReceiveComplete(void)
{
   UartOnReceiveArgs_t args = { instance.receivedByte };
   Event_SingleSubscriberSynchronous_Publish(&instance.receiveEvent, &args);
}

static void ErrorOccurred(void)
{
   Event_SingleSubscriberSynchronous_Publish(&instance.errorEvent, NULL);
}

static void Callback(uint32_t event)
{
   switch(event)
   {
      case CY_SCB_UART_TRANSMIT_DONE_EVENT:
         TransmitComplete();
         break;

      case CY_SCB_UART_RECEIVE_DONE_EVENT:
         Cy_SCB_ClearRxInterrupt(BASE_PERIPHERAL, CY_SCB_RX_INTR_LEVEL);
         ReceiveComplete();
         Cy_SCB_UART_Receive(BASE_PERIPHERAL, &instance.receivedByte, 1, &uartContext);
         break;

      case CY_SCB_UART_RECEIVE_ERR_EVENT:
      case CY_SCB_UART_TRANSMIT_ERR_EVENT:
         ErrorOccurred();
         break;
   }
}

static void Interrupt(void)
{
   Cy_SCB_UART_Interrupt(BASE_PERIPHERAL, &uartContext);
}

static void InitializeUart(void)
{
   Cy_SysInt_Init(&uartInterruptConfig, Interrupt);
   NVIC_EnableIRQ(uartInterruptConfig.intrSrc);

   cy_en_scb_uart_status_t status = Cy_SCB_UART_Init(BASE_PERIPHERAL, &uartConfig, &uartContext);
   uassert(status == CY_SCB_UART_SUCCESS);

   Cy_SCB_UART_RegisterCallback(BASE_PERIPHERAL, Callback, &uartContext);
   Cy_SCB_UART_Enable(BASE_PERIPHERAL);
   Cy_SCB_UART_Receive(BASE_PERIPHERAL, &instance.receivedByte, 1, &uartContext);
}

static void Initialize(void)
{
   Gpio_Psoc6_ConfigurePin(3, 0, &rxPinConfig);
   Gpio_Psoc6_ConfigurePin(3, 1, &txPinConfig);

   uint32_t scbClkHz = BaudRate * Oversample;
   double exactDividerValue = ((double)cy_PeriClkFreqHz / (double)scbClkHz);
   uint16_t divInt = (uint16_t)exactDividerValue;
   uint16_t divFrac = (uint16_t)((exactDividerValue - divInt) * 32.0);

   Cy_SysClk_PeriphDisableDivider(CY_SYSCLK_DIV_16_5_BIT, 1);
   Cy_SysClk_PeriphSetFracDivider(CY_SYSCLK_DIV_16_5_BIT, 1, divInt, divFrac);
   Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_16_5_BIT, 1);
   Cy_SysClk_PeriphAssignDivider(PCLK_SCB6_CLOCK, CY_SYSCLK_DIV_16_5_BIT, 1);

   InitializeUart();
}

I_Uart_t *Uart_Scb2_Init(void)
{
   instance.interface.api = &api;

   Event_SingleSubscriberSynchronous_Init(&instance.receiveEvent);
   Event_SingleSubscriberSynchronous_Init(&instance.transmitEvent);
   Event_SingleSubscriberSynchronous_Init(&instance.errorEvent);

   Initialize();

   return &instance.interface;
}
