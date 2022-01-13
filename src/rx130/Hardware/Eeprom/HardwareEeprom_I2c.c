/*!
 * @file
 * @brief Implementation of HardwareEeprom I2C module.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "HardwareEeprom_I2c.h"
#include "iodefine.h"
#include "utils.h"
#include "Rx130Defines.h"
#include "Event_Null.h"

uint32_t delayErrors = 0;

#define DELAY(delay)                      \
   {                                      \
      volatile uint32_t dcnt;             \
      for(dcnt = 0; dcnt < delay; dcnt++) \
      {                                   \
         __asm("nop");                    \
      }                                   \
   }

#define DECREASE_COUNT_EXIT_IF_ZERO(x)                      \
   do                                                       \
   {                                                        \
      if(--x == 0)                                          \
      {                                                     \
         return HardwareEepromErrorSource_OperationTimeOut; \
      }                                                     \
   } while(0)

enum
{
   EepromEraseSize = HardwareEeprom_I2c_Size_In_Bytes,
   EepromSize = HardwareEeprom_I2c_Size_In_Bytes,
   EepromAlignmentByte = 1,
   EepromEraseBytes = 0xFF,
   EepromDeviceAddress = 0xA0u,
   EepromMaxWriteCommandWaitingCount = 10000,
   EepromMaxWaitingCount = 50000,
   EepromWaitingForNextWriteAccess = 8000,
   EepromMaxHangingCount = 10,
   EepromMaxBusIdleCount = 200,
};

static HardwareEeprom_I2c_t instance;

static bool AllBytesHaveBeenRead()
{
   return (instance._private.receiveDataCount == instance._private.totalReadBytes);
}

static bool TheNextByteIsTheLastByteToBeRead()
{
   return (instance._private.receiveDataCount == (instance._private.totalReadBytes - 1));
}

void RIIC0_RXI0(void) __attribute__((interrupt));
void RIIC0_RXI0(void)
{
   volatile uint8_t dummyRead = 0;

   /* The first byte received is the dummy byte */
   if(instance._private.receiveDataCount == 0x0)
   {
      if(TheNextByteIsTheLastByteToBeRead())
      {
         /* Write 1 as acknowledgement bit to terminate
            read sequence after reception of last byte
            0 : ACK, 1: NACK */
         RIIC0.ICMR3.BIT.ACKBT = 0x1;
      }
      /* Dummy read */
      dummyRead = RIIC0.ICDRR;
      IGNORE(dummyRead);
   }
   else
   {
      /* Check if the received number of bytes has been reached */
      if(AllBytesHaveBeenRead())
      {
         /* Disable receive end interrupts */
         RIIC0.ICIER.BIT.RIE = 0x0;

         /* Clear the STOP flag */
         RIIC0.ICSR2.BIT.STOP = 0;

         /* Issue a stop condition */
         RIIC0.ICCR2.BIT.SP = 0x1;

         /* Store the data */
         instance._private.readBuffer[instance._private.receiveDataCount - 1] = RIIC0.ICDRR;

         /* Prepare for receiving final data */
         RIIC0.ICMR3.BIT.WAIT = 0x0;
      }
      else
      {
         /* Check if the received byte is the next to last */
         if(TheNextByteIsTheLastByteToBeRead())
         {
            /* Write 1 as acknowledgement bit to terminate
            read sequence after reception of last byte */
            RIIC0.ICMR3.BIT.ACKBT = 0x1;
         }

         /* Store the data */
         instance._private.readBuffer[instance._private.receiveDataCount - 1] = RIIC0.ICDRR;
      }
   }

   /* Increment the counter */
   if(!AllBytesHaveBeenRead())
   {
      instance._private.receiveDataCount++;
   }
}

static void ConfigureEepromPins(void)
{
   SYSTEM.PRCR.WORD = U16_PRCR_ENABLE;

   MSTP_RIIC0 = 0; // Cancel the RIIC0 module clock stop mode
   MSTP_S12AD = 0; // Cancel the S12AD module clock stop mode
   PORT1.PMR.BIT.B3 = 1; // Set SDA & SCL pin for peripheral use
   PORT1.PMR.BIT.B2 = 1;

   SYSTEM.PRCR.WORD = U16_PRCR_DISABLE;

   MPC.PWPR.BIT.B0WI = 0; // enable writing PFSWE bit
   MPC.PWPR.BIT.PFSWE = 1; // enable writing PFS register

   MPC.P13PFS.BYTE = 0x0F; // Select the SDA pin function
   MPC.P12PFS.BIT.PSEL = 0x0F; // Select the SCL pin function

   MPC.PWPR.BIT.PFSWE = 0; // disable writing PFS register
   MPC.PWPR.BIT.B0WI = 1; // disable writing PFSWE bit
}

static void InitializeEepromHardware(void)
{
   volatile uint8_t hangingCount = EepromMaxHangingCount;
   volatile bool sdaWasHangingLow = false;

   RIIC0.ICCR1.BIT.ICE = 0; // SCL, SDA pins not driven
   RIIC0.ICCR1.BIT.IICRST = 1; // RIIC0 reset
   RIIC0.ICCR1.BIT.ICE = 1; // Internal reset

   /* Set the transfer clock to PCLK/8=4MHz */
   RIIC0.ICMR1.BIT.CKS = 0x3;
   /* Using 50% duty */
   /* Set the transfer bit rate to 100kbps */
   RIIC0.ICBRL.BYTE = 0xF2;
   /* Set the transfer rate to 100kbps */
   RIIC0.ICBRH.BYTE = 0xEF;

   /* Configure the bus mode register 2:
   Timeout detection uses a 16-bit timer
   Count is disable for both high and low level on the SCL0 line
   No SDA output delays used
   Output delay clock source set to internal IIC reference clock */
   RIIC0.ICMR2.BYTE = 0x00u;

   /* Hold the clock line low after each received byte in receive mode */
   RIIC0.ICMR3.BIT.WAIT = 0x1;

   /* Enable receive end interrupts */
   RIIC0.ICIER.BYTE = 0x20u;

   /* Enable writing to ACKBT bit */
   RIIC0.ICMR3.BIT.ACKWP = 0x1;

   /* Write 0 as acknowledgement bit */
   RIIC0.ICMR3.BIT.ACKBT = 0x0;

   /* Set RIIC0 RXI0 interrupt priority level to 11 */
   IPR(RIIC0, RXI0) = 11;
   /* Enable RIIC0 RXI0 interrupts */
   IEN(RIIC0, RXI0) = 0x1u;
   /* Clear RIIC0 RXI0 interrupt flag */
   IR(RIIC0, RXI0) = 0x0u;

   RIIC0.ICMR1.BIT.MTWP = 1; // Enable writing to the MST and TRS bits in ICCR2
   RIIC0.ICCR2.BIT.MST = 1; // Set master

   /* Cancel the internal reset */
   RIIC0.ICCR1.BIT.IICRST = 0x0;

   // Hanging?
   while((0 == RIIC0.ICCR1.BIT.SDAI) && (hangingCount > 0))
   {
      if(0 == RIIC0.ICCR1.BIT.CLO)
      {
         /* Generate one clock signal to release SDA */
         RIIC0.ICCR1.BIT.CLO = 1;

         hangingCount--;

         sdaWasHangingLow = true;
      }
   }

   if(sdaWasHangingLow)
   {
      /* Issue a stop condition */
      RIIC0.ICCR2.BIT.SP = 0x1;
   }
}

static HardwareEepromErrorSource_t Read_Eeprom_Hardware(
   uint16_t address,
   uint8_t *readData,
   uint8_t numBytes)
{
   uint8_t loAddr, hiAddr;

   /* Declare a status variable */
   HardwareEepromErrorSource_t error = HardwareEepromErrorSource_DeviceError;
   /* Declare a dummy variable */
   volatile uint8_t dummyRead = 0;
   volatile uint32_t u32WaitCount = EepromMaxWaitingCount;
   volatile uint32_t specialWaitCount = 4 * EepromMaxWaitingCount;

   /* Reset the data received counter */
   instance._private.receiveDataCount = 0;
   /* Copy the number of bytes to be receive from the client */
   instance._private.totalReadBytes = numBytes;
   /* Store the data pointer */
   instance._private.readBuffer = readData;

   InitializeEepromHardware();

   /* Check if the IIC bus is busy */
   while(RIIC0.ICCR2.BIT.BBSY == 1)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* Issue a start condition */
   RIIC0.ICCR2.BIT.ST = 0x1;

   /* Confirm issue of start condition */
   while((RIIC0.ICCR2.BIT.BBSY == 0) && (RIIC0.ICSR2.BIT.START == 0) && (RIIC0.ICCR2.BIT.ST == 1))
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* check if transmit buffer is empty */
   if(RIIC0.ICSR2.BIT.TDRE == 1)
   {
      /* Specify device write */
      RIIC0.ICDRT = 0xA0;
   }

   /* wait for client acknowledgement */
   while(RIIC0.ICMR3.BIT.ACKBR == 1)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* Ensure the transmit buffer is empty */
   while(RIIC0.ICSR2.BIT.TEND == 0)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   hiAddr = (uint8_t)(address >> 8);

   /* Check if transmit buffer is empty */
   if(RIIC0.ICSR2.BIT.TDRE == 1)
   {
      /* Set the client's internal memory to 0 */
      RIIC0.ICDRT = hiAddr;
   }

   /* wait for client acknowledgement */
   while(RIIC0.ICMR3.BIT.ACKBR == 1)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* Ensure the transmit buffer is empty */
   while(RIIC0.ICSR2.BIT.TEND == 0)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   loAddr = (uint8_t)(address);

   /* Check if transmit buffer is empty */
   if(RIIC0.ICSR2.BIT.TDRE == 1)
   {
      /* Set the client's internal memory to 0 */
      RIIC0.ICDRT = loAddr;
   }

   /* wait for client acknowledgement */
   while(RIIC0.ICMR3.BIT.ACKBR == 1)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* Ensure the transmit buffer is empty */
   while(RIIC0.ICSR2.BIT.TEND == 0)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* Issue a restart condition */
   RIIC0.ICCR2.BIT.RS = 0x1;

   /* Add a small delay */
   DELAY(EepromMaxBusIdleCount);

   /* Check if the IIC bus is occupied */
   while(RIIC0.ICCR2.BIT.BBSY == 0)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* Check if transmit buffer is empty */
   if(RIIC0.ICSR2.BIT.TDRE == 1)
   {
      /* Set the 'W' bit to indicate a read from device */
      RIIC0.ICDRT = (uint8_t)(EepromDeviceAddress | 0x01u);
   }

   /* wait for client acknowledgement */
   while(RIIC0.ICMR3.BIT.ACKBR == 1)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* Wait for the dummy byte's reception */
   while(RIIC0.ICSR2.BIT.RDRF == 1)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* Wait until all expected data has been
    received or until a NACKF has been detected */
   while(!AllBytesHaveBeenRead() && (RIIC0.ICSR2.BIT.NACKF == 0))
      DECREASE_COUNT_EXIT_IF_ZERO(specialWaitCount);

   /* Reception was terminated due to NACK reception */
   if(1 == RIIC0.ICSR2.BIT.NACKF)
   {
      /* Dummy read */
      dummyRead = RIIC0.ICDRR;
      IGNORE(dummyRead);
      /* Clear the STOP flag */
      RIIC0.ICSR2.BIT.STOP = 0;

      /* Issue a stop condition */
      RIIC0.ICCR2.BIT.SP = 0x1;
   }

   /* Ensure the stop condition has been issued */
   while(RIIC0.ICSR2.BIT.STOP == 0x0)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* Clear the NACKF flag */
   RIIC0.ICSR2.BIT.NACKF = 0;

   /* Clear the STOP flag */
   RIIC0.ICSR2.BIT.STOP = 0;

   /* Disable writing to ACKBT bit */
   RIIC0.ICMR3.BIT.ACKWP = 0x0;

   /* Disable receive end interrupts */
   RIIC0.ICIER.BIT.RIE = 0x0;

   /* Add a delay to guarantee IIC bus free time */
   DELAY(EepromMaxBusIdleCount);

   /* Read the status returned from the interrupt routine */
   if(AllBytesHaveBeenRead())
   {
      error = HardwareEepromErrorSource_None;
   }

   return error;
}

static HardwareEepromErrorSource_t Write_Eeprom_Hardware(
   uint16_t address,
   const uint8_t *writeData,
   uint16_t numBytes,
   bool eepromErase)
{
   /* Declare a status variable */
   HardwareEepromErrorSource_t error = HardwareEepromErrorSource_DeviceError;
   /* Declare variable to indicate status of client address transmission */
   bool clientDevAddrSent = false;
   /* Declare a count variable up to about 6ms */
   volatile uint8_t i = 0;
   volatile uint32_t u32WaitCount = EepromMaxWriteCommandWaitingCount;

   InitializeEepromHardware();

   /* Check if the IIC bus is busy */
   while(RIIC0.ICCR2.BIT.BBSY == 1)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* Issue a start condition */
   RIIC0.ICCR2.BIT.ST = 0x1u;

   /* Confirm issue of start condition */
   while((RIIC0.ICCR2.BIT.BBSY == 0) && (RIIC0.ICSR2.BIT.START == 0) && (RIIC0.ICCR2.BIT.ST == 1))
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* If no acknowledgement received or all data have been sent,
    stop transmission */
   while((RIIC0.ICSR2.BIT.NACKF == 0) && (i < numBytes))
   {
      /* Sent the client address and set the client device memory to 0 */
      if(clientDevAddrSent == false)
      {
         uint8_t loAddr, hiAddr;

         /* check if transmit buffer is empty */
         if(RIIC0.ICSR2.BIT.TDRE == 1)
         {
            /* Specify device write */
            RIIC0.ICDRT = EepromDeviceAddress;
         }

         /* Wait whilst data is being transmitted */
         while(RIIC0.ICSR2.BIT.TEND == 0)
            DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

         /* wait for client acknowledgement */
         while(RIIC0.ICMR3.BIT.ACKBR == 1)
            DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

         hiAddr = (uint8_t)(address >> 8);

         /* Check if transmit buffer is empty */
         if(RIIC0.ICSR2.BIT.TDRE == 1)
         {
            /* Set the client's internal memory to 0 */
            RIIC0.ICDRT = hiAddr;
         }
         /* Wait whilst data is being transmitted  */
         while(RIIC0.ICSR2.BIT.TEND == 0)
            DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

         /* wait for client acknowledgement */
         while(RIIC0.ICMR3.BIT.ACKBR == 1)
            DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

         loAddr = (uint8_t)address;

         /* Check if transmit buffer is empty */
         if(RIIC0.ICSR2.BIT.TDRE == 1)
         {
            /* Set the client's internal memory to 0 */
            RIIC0.ICDRT = loAddr;
         }

         /* Wait whilst data is being transmitted  */
         while(RIIC0.ICSR2.BIT.TEND == 0)
            DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

         /* wait for client acknowledgement */
         while(RIIC0.ICMR3.BIT.ACKBR == 1)
            DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

         /* Reconfigure a count variable (about 30ms) for writing data transaction sequence */
         u32WaitCount = EepromMaxWaitingCount;

         /* Set flag to ensure device address is only sent once */
         clientDevAddrSent = true;
      }
      else
      {
         /* check if transmit buffer is empty */
         if(RIIC0.ICSR2.BIT.TDRE == 1)
         {
            /* Specify device write */
            if(eepromErase)
            {
               RIIC0.ICDRT = EepromEraseBytes;
            }
            else
            {
               RIIC0.ICDRT = *writeData++;
            }
         }

         /* Ensure transmission has ended */
         while(RIIC0.ICSR2.BIT.TEND == 0)
            DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

         /* wait for client acknowledgement */
         while(RIIC0.ICMR3.BIT.ACKBR == 1)
            DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

         /* Check if the desired number of bytes sent has been reached */
         if(++i == numBytes)
         {
            /* Set the status to true if all data have been sent */
            error = HardwareEepromErrorSource_None;

            /* Exit the loop */
            break;
         }
      }

      DELAY(EepromMaxBusIdleCount);
   }

   /* Check if the client did not acknowledge any byte sent to it */
   if(RIIC0.ICSR2.BIT.NACKF == 1)
   {
      error = HardwareEepromErrorSource_DeviceError;
   }

   /* Clear the stop condition flag */
   RIIC0.ICSR2.BIT.STOP = 0;

   /* Issue a stop condition */
   RIIC0.ICCR2.BIT.SP = 0x1;

   /* Ensure the stop condition has been issued */
   while(RIIC0.ICSR2.BIT.STOP == 0x0)
      DECREASE_COUNT_EXIT_IF_ZERO(u32WaitCount);

   /* Clear the NACKF flag */
   RIIC0.ICSR2.BIT.NACKF = 0;

   /* Clear the STOP flag */
   RIIC0.ICSR2.BIT.STOP = 0;

   /* Wait for EEPROM's writing cycle time */
   DELAY(EepromMaxBusIdleCount);

   return error;
}

static void Read(I_HardwareEeprom_t *_instance, uint16_t offset, uint16_t numBytes, void *readBuffer)
{
   IGNORE(_instance);
   HardwareEepromErrorSource_t errorSource = Read_Eeprom_Hardware(offset, readBuffer, numBytes);

   HardwareEepromEventArgs_t eventArgs = {
      .mode = HardwareEepromMode_Read,
      .source = errorSource
   };
   Event_SafeSynchronous_Publish(&instance._private.readCompleteEvent, &eventArgs);
}

static void Write(I_HardwareEeprom_t *_instance, uint16_t offset, uint16_t numBytes, const void *writeBuffer)
{
   IGNORE(_instance);

   HardwareEepromErrorSource_t errorSource = Write_Eeprom_Hardware(offset, (const uint8_t *)writeBuffer, numBytes, false);

   HardwareEepromEventArgs_t eventArgs = {
      .mode = HardwareEepromMode_Write,
      .source = errorSource
   };
   Event_SafeSynchronous_Publish(&instance._private.writeCompleteEvent, &eventArgs);
}

static void Erase(I_HardwareEeprom_t *_instance)
{
   IGNORE(_instance);

   uint16_t offset = 0;
   uint8_t retries = 0;
   bool isNotFirstAccess = false;

   HardwareEepromErrorSource_t errorSource = HardwareEepromErrorSource_None;

   while((offset < EepromEraseSize) && (errorSource == HardwareEepromErrorSource_None))
   {
      /* Wait about 5ms time when it's not first slice of erase operation */
      if(isNotFirstAccess)
      {
         DELAY(EepromWaitingForNextWriteAccess);
         Action_SafeInvoke(instance._private.watchdogKickAction);
      }

      /* Consider page boundaries, erase each page */
      errorSource = Write_Eeprom_Hardware(offset, NULL, HardwareEeprom_I2c_Page_Size_In_Bytes, true);

      /* Shift to next sequence when operation succeeded */
      retries += 1;
      if(errorSource == HardwareEepromErrorSource_None)
      {
         offset += HardwareEeprom_I2c_Page_Size_In_Bytes;
         retries = 0;
         isNotFirstAccess = true;
      }

      Action_SafeInvoke(instance._private.watchdogKickAction);
   }

   HardwareEepromEventArgs_t eventArgs = {
      .mode = HardwareEepromMode_Erase,
      .source = errorSource
   };
   Event_SafeSynchronous_Publish(&instance._private.eraseCompleteEvent, &eventArgs);
}

static uint16_t GetByteAlignment(I_HardwareEeprom_t *_instance)
{
   IGNORE(_instance);
   return EepromAlignmentByte;
}

static uint16_t GetSize(I_HardwareEeprom_t *_instance)
{
   IGNORE(_instance);
   return EepromSize;
}

static I_Event_t *GetOnWrittenEvent(I_HardwareEeprom_t *_instance)
{
   IGNORE(_instance);
   return &instance._private.writeCompleteEvent.interface;
}

static I_Event_t *GetOnReadEvent(I_HardwareEeprom_t *_instance)
{
   IGNORE(_instance);
   return &instance._private.readCompleteEvent.interface;
}

static I_Event_t *GetOnErasedEvent(I_HardwareEeprom_t *_instance)
{
   IGNORE(_instance);
   return &instance._private.eraseCompleteEvent.interface;
}

static const I_HardwareEeprom_Api_t api = {
   Read,
   Write,
   Erase,
   GetByteAlignment,
   GetSize,
   GetOnWrittenEvent,
   GetOnReadEvent,
   GetOnErasedEvent
};

HardwareEeprom_I2c_t *HardwareEeprom_I2c_Init(I_Action_t *watchdogKickAction)
{
   instance.interface.api = &api;
   instance._private.watchdogKickAction = watchdogKickAction;

   Event_SafeSynchronous_Init(&instance._private.readCompleteEvent);
   Event_SafeSynchronous_Init(&instance._private.writeCompleteEvent);
   Event_SafeSynchronous_Init(&instance._private.eraseCompleteEvent);

   ConfigureEepromPins();
   InitializeEepromHardware();

   return &instance;
}
