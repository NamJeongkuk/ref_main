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
   FaultId_FreshFoodThermistorIsInvalid = 1,
   FaultId_FreezerThermistorIsInvalid = 3,
   FaultId_FreezerEvaporatorThermistorIsInvalid = 4,
   FaultId_AmbientThermistorIsInvalid = 8,
   FaultId_AmbientHumiditySensorIsInvalid = 9,
   FaultId_IceMaker0ThermistorIsInvalid = 10,
   FaultId_IceMaker1ThermistorIsInvalid = 12,
   FaultId_ExcessiveCompressorRunTime = 300,
   FaultId_CompressorOffWhileCoolingSystemOnForMaxTime = 301,
   FaultId_DoorBoardCommunicationFault = 523,
   FaultId_AutofillCouplingFault = 721,
   FaultId_DispenserUiCommunicationFault = 733,
   FaultId_AutofillNoWaterMovementFault = 737,
   FaultId_AutofillTargetRemovedFault = 739,
   FaultId_AutofillEmiDetectedFault = 740,
   FaultId_AndroidSbcCommunicationFault = 756,
   FaultId_EmbeddedWifiCommunicationFault = 757,
   FaultId_InternalTemperatureUiCommunicationFault = 758,
   FaultId_RfidBoardHardwareFailureFault = 800,
   FaultId_RfidBoardCommunicationFault = 801,
   FaultId_RfidBoardTagAuthenticationFailedFault = 803,
   FaultId_RfidBoardLeakDetectedFault = 805,
   FaultId_RfidBoardBlockedTagFault = 806
};
typedef uint16_t FaultId_t;

#endif
