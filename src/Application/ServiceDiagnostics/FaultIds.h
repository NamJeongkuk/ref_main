/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FAULTIDS_H
#define FAULTIDS_H

#include <stdint.h>

enum
{
   FaultId_Unknown = 0,
   FaultId_DispenserUiCommunicationFault = 733,
   FaultId_AndroidSbcCommunicationFault = 3,
   FaultId_DoorBoardCommunicationFault = 523,
   FaultId_EmbeddedWifiCommunicationFault = 5,
   FaultId_InternalTemperatureUiCommunicationFault = 6,
   FaultId_RfidBoardHardwareFailureFault = 800,
   FaultId_RfidBoardCommunicationFault = 801,
   FaultId_RfidBoardTagAuthenticationFailedFault = 803,
   FaultId_RfidBoardLeakDetectedFault = 805
};
typedef uint16_t FaultId_t;

#endif
