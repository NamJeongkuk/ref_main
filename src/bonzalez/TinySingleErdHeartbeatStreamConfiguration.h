/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TINYSINGLEERDHEARTBEATSTREAMCONFIGURATION_H
#define TINYSINGLEERDHEARTBEATSTREAMCONFIGURATION_H

#define DATA_TYPE uint8_t

enum
{
   StreamEntryCount = 5,
   DestinationAddress = 0xB0,
   HeartbeatPeriodInMsec = 500,

   DataErd = 0xF00D,
   HeartbeatTickErd = 0xF00C,
   ErdStreamErd = 0xF007
};

#endif
