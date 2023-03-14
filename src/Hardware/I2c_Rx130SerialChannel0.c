/*!
 * @file
 * @brief Implementation of the the I2C driver on the SCI0 peripheral
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdbool.h>
#include <string.h>
#include "iodefine.h"
#include "utils.h"
#include "uassert.h"
#include "I2c_Rx130SerialChannel0.h"

enum
{
   I2c_AckReceived = 0x00
};

enum
{
   CurrentOperation_Idle = 0,
   CurrentOperation_ReadFirstByte,
   CurrentOperation_ReadAdditionalBytes,
   CurrentOperation_ReadByte,
   CurrentOperation_Write,
   CurrentOperation_WriteByte,
   CurrentOperation_Stop,
   CurrentOperation_Start,
   CurrentOperation_Restart
};
typedef uint8_t CurrentOperation_t;

typedef struct
{
   I_I2c_t interface;

   struct
   {
      void *context;
      uint8_t *readDataBuffer;
      const uint8_t *writeDataBuffer;
      union
      {
         I2cWriteCompleteCallback_t writeCompleteCallback;
         I2cReadByteCompleteCallback_t readByteCompleteCallback;
         I2cReadCompleteCallback_t readCompleteCallback;
         I2cConditionCompleteCallback_t conditionCompleteCallback;
      };
      I2cErrorOccurredCallback_t errorOccurredCallback;
      uint8_t sizeOfReadDataBuffer;
      uint8_t sizeOfWriteDataBuffer;
      bool dataHasBeenRead;
      uint8_t dataBufferIndex;
      CurrentOperation_t currentOperation;
      uint8_t dataRead;
      bool busBusy;
   } _private;
} I2c_Rx130SerialChannel0_t;

static I2c_Rx130SerialChannel0_t instance;

static inline void WriteByteToTransmitRegister(uint8_t byte)
{
   SCI0.TDR = byte;
}

static inline uint8_t ReadReceivedData(void)
{
   return SCI0.RDR;
}

static inline bool AnOverrunHasOccurred(void)
{
   return (SCI0.SSR.BIT.ORER); // Overrun error flag indicating a bad message
}

static inline bool AnAckWasReceived(void)
{
   return (SCI0.SISR.BIT.IICACKR == I2c_AckReceived);
}

static inline void EnableReceiveInterrupt(void)
{
   SCI0.SCR.BIT.RIE = 0x01;
}

static inline void AckTheNextBytesReceived(void)
{
   SCI0.SIMR2.BIT.IICACKT = 0x00;
}

static inline void NackTheNextBytesReceived(void)
{
   SCI0.SIMR2.BIT.IICACKT = 0x01;
}

static inline void SendDummyData(void)
{
   WriteByteToTransmitRegister(0xFF); // Renesas needs dummy data in here for reasons that were not stated in the manual
}

static inline bool AllBytesHaveBeenWritten(void)
{
   return (instance._private.dataBufferIndex >= instance._private.sizeOfWriteDataBuffer);
}

static inline bool AllBytesHaveBeenRead(void)
{
   return (instance._private.dataBufferIndex >= instance._private.sizeOfReadDataBuffer);
}

static inline bool TheNextByteIsTheLastByteToBeRead(void)
{
   return ((instance._private.dataBufferIndex + 1) == instance._private.sizeOfReadDataBuffer);
}

static inline void NotifyCallerWriteComplete(void)
{
   instance._private.writeCompleteCallback(instance._private.context);
}

static inline void NotifyCallerErrorOccurred(void)
{
   instance._private.errorOccurredCallback(instance._private.context);
}

static inline void NotifyCallerReadByteComplete(void)
{
   instance._private.readByteCompleteCallback(instance._private.context, instance._private.dataRead);
}

static inline void NotifyCallerReadComplete(void)
{
   instance._private.readCompleteCallback(instance._private.context);
}

static inline void NotifyCallerConditionComplete(void)
{
   instance._private.conditionCompleteCallback(instance._private.context);
}

static void SetCurrentOperation(CurrentOperation_t operation)
{
   instance._private.currentOperation = operation;
}

/*!
 * The RXI interrupt means that data has been received and is available
 * for reading from the RDR (Receive Data Register)
 */
// SCI0 RXI0 Reception, ACK detection.
void SCI0_RXI0(void) __attribute__((interrupt));
void SCI0_RXI0(void)
{
   if(AnOverrunHasOccurred())
   {
      SetCurrentOperation(CurrentOperation_Idle);
      NotifyCallerErrorOccurred();
   }
   else
   {
      switch(instance._private.currentOperation)
      {
         case CurrentOperation_ReadByte:
            instance._private.dataRead = ReadReceivedData();
            instance._private.dataHasBeenRead = true;
            break;

         case CurrentOperation_ReadFirstByte:
         case CurrentOperation_ReadAdditionalBytes:
            instance._private.readDataBuffer[instance._private.dataBufferIndex++] = ReadReceivedData();
            instance._private.dataHasBeenRead = true;
            break;
      }
   }
}

/*!
 * The TXI interrupt means that the transmission of data that was written
 * to the TDR (Transmit Data Register) is complete. TDR is empty.
 *
 * It is also called during the read process after sending the Ack or Nack
 * in response to the byte that was just received.
 */
// SCI0 TXI0 Transmission, NACK detection.
void SCI0_TXI0(void) __attribute__((interrupt));
void SCI0_TXI0(void)
{
   switch(instance._private.currentOperation)
   {
      case CurrentOperation_Write:
      case CurrentOperation_WriteByte:
         if(AnAckWasReceived())
         {
            if(AllBytesHaveBeenWritten())
            {
               SetCurrentOperation(CurrentOperation_Idle);
               NotifyCallerWriteComplete();
            }
            else
            {
               WriteByteToTransmitRegister(instance._private.writeDataBuffer[instance._private.dataBufferIndex++]);
            }
         }
         else
         {
            SetCurrentOperation(CurrentOperation_Idle);
            NotifyCallerErrorOccurred();
         }
         break;

      case CurrentOperation_ReadFirstByte:
         if(AnAckWasReceived())
         {
            AckTheNextBytesReceived();
            EnableReceiveInterrupt();
            SendDummyData();
            SetCurrentOperation(CurrentOperation_ReadAdditionalBytes);
         }
         else
         {
            SetCurrentOperation(CurrentOperation_Idle);
            NotifyCallerErrorOccurred();
         }
         break;

      case CurrentOperation_ReadAdditionalBytes:
         if(AllBytesHaveBeenRead())
         {
            SetCurrentOperation(CurrentOperation_Idle);
            NotifyCallerReadComplete();
         }
         else if(TheNextByteIsTheLastByteToBeRead())
         {
            NackTheNextBytesReceived();
            SendDummyData();
         }
         else
         {
            SendDummyData();
         }
         break;

      case CurrentOperation_ReadByte:
         if(instance._private.dataHasBeenRead)
         {
            SetCurrentOperation(CurrentOperation_Idle);
            NotifyCallerReadByteComplete();
         }
         else if(AnAckWasReceived())
         {
            NackTheNextBytesReceived();
            EnableReceiveInterrupt();
            SendDummyData();
         }
         else
         {
            SetCurrentOperation(CurrentOperation_Idle);
            NotifyCallerErrorOccurred();
         }
         break;
   }
}

/*!
 * The TEI interrupt means that one of start, restart or stop operations has completed.
 */
// SCI0 TEI0 Transmit end interrupt handler, handles start/stop conditions
//! @note That in simple I2C mode, the STI interrupt is allocated to the TEI interrupt
void SCI0_TEI0(void) __attribute__((interrupt));
void SCI0_TEI0(void)
{
   SCI0.SIMR3.BIT.IICSTIF = 0x00; // Clear all start/stop/restart conditions

   switch(instance._private.currentOperation)
   {
      case CurrentOperation_Start:
      case CurrentOperation_Restart:
         SCI0.SIMR3.BIT.IICSCLS = 0x00; // Set clock to serial clock output
         SCI0.SIMR3.BIT.IICSDAS = 0x00; // Set data to serial data output
         SCI0.SCR.BIT.RIE = 0x00;
         SCI0.SCR.BIT.TIE = 0x01;
         break;

      case CurrentOperation_Idle:
      case CurrentOperation_Stop:
         SCI0.SIMR3.BIT.IICSCLS = 0x03; // Set clock line to high impedance
         SCI0.SIMR3.BIT.IICSDAS = 0x03; // Set data line to high impedance
         SCI0.SCR.BIT.RIE = 0x00;
         SCI0.SCR.BIT.TIE = 0x00;
         instance._private.busBusy = false;
         break;
   }

   SetCurrentOperation(CurrentOperation_Idle);
   NotifyCallerConditionComplete();
}

static void InitializePorts(void)
{
   PORT2.ODR0.BYTE = 0x05; // P20 and P21 are N-channel open drain
   PORT2.PMR.BIT.B0 = 0; // Initially set as general I/O
   PORT2.PMR.BIT.B1 = 0; // Initially set as general I/O
   MPC.PWPR.BIT.B0WI = 0; // Writing to the PFSWE bit is enabled
   MPC.PWPR.BIT.PFSWE = 1; // Writing to the PFS register is enabled
   MPC.P20PFS.BYTE = 0x0A; // Set to use SSDA0 peripheral
   MPC.P21PFS.BYTE = 0x0A; // set to use SSCL0 peripheral
   MPC.PWPR.BIT.PFSWE = 0; // Writing to the PFS register is disabled
   MPC.PWPR.BIT.B0WI = 1; // Writing to the PFSWE bit is disabled
   PORT2.PMR.BIT.B0 = 1; // Set to use the peripheral
   PORT2.PMR.BIT.B1 = 1; // set to use the peripheral
   SYSTEM.PRCR.WORD = 0xA502;
   MSTP(SCI0) = 0; // Start the port
   SYSTEM.PRCR.WORD = 0xA500;
}

static void InitializeI2C(void)
{
   SCI0.SIMR3.BIT.IICSDAS = 0x03; // Place data line in high impedance until start condition is generated
   SCI0.SIMR3.BIT.IICSCLS = 0x03; // Place clock line in high impedance until start condition is generated
   SCI0.SMR.BYTE = 0x00; // Serial mode is simple I2C: use PCLK, 8-bits of data, 1 stop bit, async mode, parity is disabled
   SCI0.SCMR.BIT.SDIR = 0x01; // Serial mode is simple I2C: Transfer MSB first, SMIF bit = 0, will enable use of simple I2C
   SCI0.SCR.BYTE = 0x00; // Disable all interrupts
   SCI0.BRR = 0x14; // Setting bit rate to ~40kHz
   SCI0.SEMR.BIT.NFEN = 0x01; // Enable noise cancellation for input signals
   SCI0.SNFR.BIT.NFCS = 0x01; // Clock divide by 1 for noise filter
   SCI0.SIMR1.BIT.IICM = 0x00; // Resets I2C mode
   SCI0.SIMR1.BIT.IICDL = 0x1F; // Data line delay output select - 30 to 31 cycles
   SCI0.SIMR2.BIT.IICACKT = 0x01; // NACK transmission and reception of ACK/NACK
   SCI0.SIMR2.BIT.IICCSC = 0x01; // Synchronize with clock signal
   SCI0.SIMR2.BIT.IICINTM = 0x01; // Use reception and transmission interrupts
   SCI0.SISR.BIT.IICACKR = 0x00; // Use ACK reception flag
   SCI0.SPMR.BYTE = 0x00; // Not using SPI
   SCI0.SCMR.BIT.SMIF = 0x00; // Place in simple I2C mode
   SCI0.SIMR1.BIT.IICM = 0x01; // Place in simple I2C mode
   SCI0.SCR.BYTE = 0xB4; // Enable:TEIE,TIE,TE,RE bit
}

static void InitializeInterrupts(void)
{
   // Set interrupt priority
   IPR(SCI0, RXI0) = 5;
   IPR(SCI0, TXI0) = 5;
   IPR(SCI0, TEI0) = 5;

   // Enable interrupts
   IEN(SCI0, RXI0) = 1;
   IEN(SCI0, TXI0) = 1;
   IEN(SCI0, TEI0) = 1;
}

static void GenerateCondition(void)
{
   SCI0.SIMR3.BIT.IICSTIF = 0x00; // Prepare for generation of a start/stop/restart condition
   SCI0.SIMR3.BYTE = 0x50; // Set CLK and DATA lines to generate a start/stop/restart condition

   switch(instance._private.currentOperation)
   {
      case CurrentOperation_Start:
         SCI0.SCR.BIT.RIE = 0x00; // Disable receive interrupt
         SCI0.SCR.BIT.TE = 0x01; // Enable transmit
         SCI0.SCR.BIT.RE = 0x01; // Enable receive
         SCI0.SIMR3.BIT.IICSTAREQ = 0x01; // Generate a start condition
         break;

      case CurrentOperation_Restart:
         SCI0.SIMR3.BIT.IICRSTAREQ = 0x01; // Generate a re-start condition
         break;

      case CurrentOperation_Idle:
      case CurrentOperation_Stop:
         SCI0.SIMR2.BIT.IICACKT = 0x01; // Send NACK to notify that we are done receiving data
         SCI0.SIMR3.BIT.IICSTPREQ = 0x01; // Generate a stop condition
         break;
   }
}

static void Write(
   I_I2c_t *_instance,
   uint8_t peripheralAddress,
   const void *writeDataBuffer,
   uint8_t sizeOfWriteDataBuffer,
   I2cWriteCompleteCallback_t writeCompleteCallback,
   I2cErrorOccurredCallback_t errorOccurredCallback,
   void *context)
{
   IGNORE(_instance);
   uassert(instance._private.currentOperation == CurrentOperation_Idle);

   instance._private.writeDataBuffer = writeDataBuffer;
   instance._private.sizeOfWriteDataBuffer = sizeOfWriteDataBuffer;
   instance._private.writeCompleteCallback = writeCompleteCallback;
   instance._private.errorOccurredCallback = errorOccurredCallback;
   instance._private.dataBufferIndex = 0;
   instance._private.context = context;
   SetCurrentOperation(CurrentOperation_Write);
   instance._private.dataHasBeenRead = false;

   WriteByteToTransmitRegister(peripheralAddress);
}

static void WriteByte(
   I_I2c_t *_instance,
   uint8_t byte,
   I2cWriteByteCompleteCallback_t writeByteCompleteCallback,
   I2cErrorOccurredCallback_t errorOccurredCallback,
   void *context)
{
   IGNORE(_instance);
   uassert(instance._private.currentOperation == CurrentOperation_Idle);

   instance._private.writeDataBuffer = NULL;
   instance._private.sizeOfWriteDataBuffer = 0;
   instance._private.dataBufferIndex = 0;
   instance._private.writeCompleteCallback = writeByteCompleteCallback;
   instance._private.errorOccurredCallback = errorOccurredCallback;
   instance._private.context = context;
   SetCurrentOperation(CurrentOperation_WriteByte);

   WriteByteToTransmitRegister(byte);
}

static void Read(
   I_I2c_t *_instance,
   uint8_t peripheralAddress,
   void *readDataBuffer,
   uint8_t sizeOfReadDataBuffer,
   I2cReadCompleteCallback_t readCompleteCallback,
   I2cErrorOccurredCallback_t errorOccurredCallback,
   void *context)
{
   IGNORE(_instance);
   uassert(instance._private.currentOperation == CurrentOperation_Idle);

   instance._private.readCompleteCallback = readCompleteCallback;
   instance._private.errorOccurredCallback = errorOccurredCallback;
   instance._private.context = context;
   SetCurrentOperation(CurrentOperation_ReadFirstByte);
   instance._private.readDataBuffer = readDataBuffer;
   instance._private.sizeOfReadDataBuffer = sizeOfReadDataBuffer;
   instance._private.dataBufferIndex = 0;
   instance._private.dataHasBeenRead = false;

   WriteByteToTransmitRegister(peripheralAddress);
}

static void ReadByte(
   I_I2c_t *_instance,
   uint8_t peripheralAddress,
   I2cReadByteCompleteCallback_t readByteCompleteCallback,
   I2cErrorOccurredCallback_t errorOccurredCallback,
   void *context)
{
   IGNORE(_instance);
   uassert(instance._private.currentOperation == CurrentOperation_Idle);

   instance._private.readByteCompleteCallback = readByteCompleteCallback;
   instance._private.errorOccurredCallback = errorOccurredCallback;
   instance._private.context = context;
   SetCurrentOperation(CurrentOperation_ReadByte);
   instance._private.readDataBuffer = NULL;
   instance._private.sizeOfReadDataBuffer = 0;
   instance._private.dataBufferIndex = 0;
   instance._private.dataHasBeenRead = false;

   WriteByteToTransmitRegister(peripheralAddress);
}

static void Start(
   I_I2c_t *_instance,
   I2cConditionCompleteCallback_t conditionCompleteCallback,
   void *context)
{
   IGNORE(_instance);
   uassert(instance._private.currentOperation == CurrentOperation_Idle);

   instance._private.conditionCompleteCallback = conditionCompleteCallback;
   instance._private.context = context;
   instance._private.busBusy = true;
   SetCurrentOperation(CurrentOperation_Start);

   GenerateCondition();
}

static void Stop(
   I_I2c_t *_instance,
   I2cConditionCompleteCallback_t conditionCompleteCallback,
   void *context)
{
   IGNORE(_instance);
   uassert(instance._private.currentOperation == CurrentOperation_Idle);

   instance._private.conditionCompleteCallback = conditionCompleteCallback;
   instance._private.context = context;
   SetCurrentOperation(CurrentOperation_Stop);

   GenerateCondition();
}

static void Restart(
   I_I2c_t *_instance,
   I2cConditionCompleteCallback_t conditionCompleteCallback,
   void *context)
{
   IGNORE(_instance);
   uassert(instance._private.currentOperation == CurrentOperation_Idle);

   instance._private.conditionCompleteCallback = conditionCompleteCallback;
   instance._private.context = context;
   SetCurrentOperation(CurrentOperation_Restart);

   GenerateCondition();
}

static void Cancel(
   I_I2c_t *_instance,
   I2cConditionCompleteCallback_t conditionCompleteCallback,
   void *context)
{
   IGNORE(_instance);
   instance._private.conditionCompleteCallback = conditionCompleteCallback;
   instance._private.context = context;
   SetCurrentOperation(CurrentOperation_Idle);

   GenerateCondition();
}

static bool Busy(I_I2c_t *_instance)
{
   IGNORE(_instance);
   return instance._private.busBusy;
}

static const I_I2c_Api_t api = {
   .Start = Start,
   .Stop = Stop,
   .Restart = Restart,
   .Write = Write,
   .WriteByte = WriteByte,
   .Read = Read,
   .ReadByte = ReadByte,
   .Cancel = Cancel,
   .Busy = Busy
};

I_I2c_t *I2c_Rx130SerialChannel0_Init(I_ContextProtector_t *contextProtector)
{
   ContextProtector_Protect(contextProtector);
   InitializePorts();
   InitializeI2C();
   InitializeInterrupts();
   ContextProtector_Unprotect(contextProtector);

   instance._private.busBusy = false;

   SetCurrentOperation(CurrentOperation_Idle);
   instance.interface.api = &api;
   return &instance.interface;
}
