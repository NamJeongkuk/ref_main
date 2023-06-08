/*!
 * @file
 * @brief One wire AT21CS11 driver
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdbool.h>
#include "OneWire_AT21CS11.h"
#include "HardwareTimer.h"
#include "iodefine.h"
#include "ContextProtector_Rx2xx.h"
#include "utils.h"

enum
{
   Input = 0,
   Output = 1,
   High = 1,
   Low = 0,
   StartAddressOfHighPage = 0x80,
   MaxNumberOfTransferBytes = 8,
   TransferByteCountMask = MaxNumberOfTransferBytes - 1,
   MinimumLowTimeMicroseconds = 1,
   ReadBitSettleTimeMicroseconds = 20,
   WriteBitTimeMicroseconds = 6,
   WriteBitSettleTimeMicroseconds = 20,
   ResetLowTimeMicroseconds = 50,
   ResetHighTimeMicroseconds = 50,
   ResetSettleTimeMicroseconds = 8,
   StartStopHighTimeMicroseconds = 150
};

static struct
{
   I_OneWire_t interface;
   bool deviceIsPresent;
   I_ContextProtector_t *contextProtector;
} instance;

#define PORT_DIRECTION PORTA.PDR.BIT.B4
#define PORT_OUTPUT_DATA PORTA.PODR.BIT.B4
#define PORT_INPUT_DATA PORTA.PIDR.BIT.B4

static bool ReadBit(void)
{
   bool returnValue = false;

   PORT_OUTPUT_DATA = High;
   PORT_DIRECTION = Output;
   PORT_OUTPUT_DATA = Low;
   PORT_DIRECTION = Input;
   HardwareTimer_BlockingDelayInMicroSeconds(MinimumLowTimeMicroseconds);

   returnValue = PORT_INPUT_DATA;
   HardwareTimer_BlockingDelayInMicroSeconds(ReadBitSettleTimeMicroseconds);

   return returnValue;
}

static void SendBit(bool bit)
{
   PORT_OUTPUT_DATA = High;
   PORT_DIRECTION = Output;
   PORT_OUTPUT_DATA = Low;

   if(bit)
   {
      HardwareTimer_BlockingDelayInMicroSeconds(MinimumLowTimeMicroseconds);
      PORT_OUTPUT_DATA = High;
      HardwareTimer_BlockingDelayInMicroSeconds(WriteBitTimeMicroseconds + WriteBitSettleTimeMicroseconds);
   }
   else
   {
      HardwareTimer_BlockingDelayInMicroSeconds(MinimumLowTimeMicroseconds + WriteBitTimeMicroseconds);
      PORT_OUTPUT_DATA = High;
      HardwareTimer_BlockingDelayInMicroSeconds(WriteBitSettleTimeMicroseconds);
   }
}

void SendReset(void)
{
   PORT_OUTPUT_DATA = High;
   PORT_DIRECTION = Output;
   HardwareTimer_BlockingDelayInMicroSeconds(ResetHighTimeMicroseconds);

   PORT_OUTPUT_DATA = Low;
   HardwareTimer_BlockingDelayInMicroSeconds(ResetLowTimeMicroseconds);

   PORT_OUTPUT_DATA = High;
   HardwareTimer_BlockingDelayInMicroSeconds(ResetSettleTimeMicroseconds);
}

void SendStartStop(void)
{
   PORT_OUTPUT_DATA = High;
   PORT_DIRECTION = Output;
   HardwareTimer_BlockingDelayInMicroSeconds(StartStopHighTimeMicroseconds);
}

void WriteByte(uint8_t data)
{
   SendBit(data & 0x80);
   SendBit(data & 0x40);
   SendBit(data & 0x20);
   SendBit(data & 0x10);
   SendBit(data & 0x08);
   SendBit(data & 0x04);
   SendBit(data & 0x02);
   SendBit(data & 0x01);
}

uint8_t ReadByte(void)
{
   uint8_t returnValue = 0;
   uint8_t bitCounter;
   for(bitCounter = 0; bitCounter < 8; bitCounter++)
   {
      returnValue = (returnValue << 1) | ReadBit();
   }
   return returnValue;
}

static uint8_t Read(I_OneWire_t *context, uint8_t address, uint8_t numBytes, uint8_t *readBuffer)
{
   IGNORE(context);
   bool nacked;
   uint8_t bytesRead = 0;
   // CAUTION!
   // We're using block read here - the device can read up to 8 bytes but it is based on fixed pages of 8 bytes, so if
   // you try to read 6 bytes from address 6 you will get data from 6,7,0,1,2,3 - thus we have to read up to the end of
   // the page. In the example above we need to break the 6 byte read into 2 reads - 6,7 and afterwards 8,9,10,11
   uint8_t maxBytesThatCanBeRead = MaxNumberOfTransferBytes - (address & TransferByteCountMask);

   numBytes = MIN(numBytes, maxBytesThatCanBeRead);
   ContextProtector_Protect(instance.contextProtector);
   SendStartStop();
   if(address < StartAddressOfHighPage)
   {
      WriteByte(0xa0);
      nacked = ReadBit();
      WriteByte(address);
      nacked |= ReadBit();
      SendStartStop();
      WriteByte(0xa1);
   }
   else
   {
      address -= StartAddressOfHighPage;
      WriteByte(0xb0);
      nacked = ReadBit();
      WriteByte(address);
      nacked |= ReadBit();
      SendStartStop();
      WriteByte(0xb1);
   }
   nacked |= ReadBit();
   while(numBytes > 0)
   {
      *readBuffer++ = ReadByte();
      numBytes--;
      if(!nacked)
      {
         bytesRead++;
      }
      if(numBytes > 0)
      {
         SendBit(Low);
      }
      else
      {
         SendBit(High);
      }
   }
   SendStartStop();
   ContextProtector_Unprotect(instance.contextProtector);

   return (bytesRead);
}

static uint8_t Write(I_OneWire_t *context, uint8_t address, uint8_t numBytes, const uint8_t *writeBuffer)
{
   IGNORE(context);
   bool nacked;
   uint8_t bytesWritten = 0;
   // CAUTION!
   // We're using block write here - the device can write up to 8 bytes but it is based on fixed pages of 8 bytes, so if
   // you try to write 6 bytes from address 6 you will write data to 6,7,0,1,2,3 - thus we have to write to the end of
   // the page. In the example above we need to break the 6 byte write into 2 writes - 6,7 and afterwards 8,9,10,11
   uint8_t maxBytesThatCanBeWritten = MaxNumberOfTransferBytes - (address & TransferByteCountMask);

   numBytes = MIN(numBytes, maxBytesThatCanBeWritten);
   ContextProtector_Protect(instance.contextProtector);
   SendStartStop();
   if(address < StartAddressOfHighPage)
   {
      WriteByte(0xa0);
   }
   else
   {
      WriteByte(0xb0);
      address -= StartAddressOfHighPage;
   }
   nacked = ReadBit();
   WriteByte(address);
   nacked |= ReadBit();
   while(numBytes > 0)
   {
      WriteByte(*writeBuffer++);
      nacked |= ReadBit();
      numBytes--;
      if(!nacked)
      {
         bytesWritten++;
      }
   }
   SendStartStop();
   ContextProtector_Unprotect(instance.contextProtector);
   // The data takes time to program - need to wait 5mS before writing the next packet
   return (bytesWritten);
}

static bool DeviceIsPresent(I_OneWire_t *context)
{
   IGNORE(context);
   uint8_t manufacturerData[3] = { 0, 0, 0 };
   bool nacked;

   ContextProtector_Protect(instance.contextProtector);
   SendStartStop();
   WriteByte(0xc1);
   nacked = ReadBit();
   if(!nacked)
   {
      manufacturerData[0] = ReadByte();
      SendBit(false);
      manufacturerData[1] = ReadByte();
      SendBit(false);
      manufacturerData[2] = ReadByte();
      SendBit(true);
   }
   SendStartStop();
   ContextProtector_Unprotect(instance.contextProtector);

   return ((0xD3 == manufacturerData[1]) && (0x80 == manufacturerData[2]));
}

static const I_OneWire_Api_t api = {
   Read,
   Write,
   DeviceIsPresent
};

I_OneWire_t *OneWire_AT21CS11_Init(void)
{
   instance.interface.api = &api;
   instance.contextProtector = ContextProtector_Rx2xx_GetInstance();

   ContextProtector_Protect(instance.contextProtector);
   SendReset();
   ReadBit();
   ContextProtector_Unprotect(instance.contextProtector);

   return &instance.interface;
}
