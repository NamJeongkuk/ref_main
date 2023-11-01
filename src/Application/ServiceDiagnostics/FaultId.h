/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FAULTID_H
#define FAULTID_H

#include <stdint.h>

enum
{
   FaultId_Unknown = 0,
   FaultId_DoorBoardCommunicationFault = 523,
   FaultId_DispenserUiCommunicationFault = 733,
   FaultId_AndroidSbcCommunicationFault = 756,
   FaultId_EmbeddedWifiCommunicationFault = 757,
   FaultId_InternalTemperatureUiCommunicationFault = 758,
   FaultId_RfidBoardHardwareFailureFault = 800,
   FaultId_RfidBoardCommunicationFault = 801,
   FaultId_RfidBoardTagAuthenticationFailedFault = 803,
   FaultId_RfidBoardLeakDetectedFault = 805
};
typedef uint16_t FaultId_t;

#endif
