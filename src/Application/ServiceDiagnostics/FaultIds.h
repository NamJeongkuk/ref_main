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
   FaultId_RfidBoardCommunicationFault = 801,
   FaultId_AndroidSbcCommunicationFault = 3,
   FaultId_DoorBoardCommunicationFault = 523,
   FaultId_EmbeddedWifiCommunicationFault = 5,
   FaultId_InternalTemperatureUiCommunicationFault = 6,
};
typedef uint16_t FaultId_t;

#endif
