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
   FaultId_FreshFoodEvaporatorThermistorIsInvalid = 6,
   FaultId_AmbientThermistorIsInvalid = 8,
   FaultId_AmbientHumiditySensorIsInvalid = 9,
   FaultId_IceMaker0ThermistorIsInvalid = 10,
   FaultId_IceMaker1ThermistorIsInvalid = 12,
   FaultId_IceMaker2ThermistorIsInvalid = 19,
   FaultId_FreezerEvapFanMissingFanFeedbackWhenFanIsRunning = 100,
   FaultId_FreezerEvapFanFeedbackPresentWhenFanIsOff = 101,
   FaultId_FreezerEvapFanCannotReachTargetRpm = 102,
   FaultId_CondenserFanMissingFanFeedbackWhenFanIsRunning = 103,
   FaultId_CondenserEvapFanFeedbackPresentWhenFanIsOff = 104,
   FaultId_CondenserEvapFanCannotReachTargetRpm = 105,
   FaultId_FreshFoodEvapFanMissingFanFeedbackWhenFanIsRunning = 106,
   FaultId_FreshFoodEvapFanFeedbackPresentWhenFanIsOff = 107,
   FaultId_FreshFoodEvapFanCannotReachTargetRpm = 108,
   FaultId_FreezerDefrostHeaterOnForMaxTime = 200,
   FaultId_FreshFoodDefrostHeaterOnForMaxTimeFault = 201,
   FaultId_ConvertibleCompartmentDefrostHeaterOnForMaxTimeFault = 204,
   FaultId_ExcessiveCompressorRunTime = 300,
   FaultId_CompressorOffWhileCoolingSystemOnForMaxTime = 301,
   FaultId_FreshFoodNoFreezeLimitTripped = 302,
   FaultId_FreshFoodTemperatureExceeded = 303,
   FaultId_FreezerTemperatureExceeded = 304,
   FaultId_IceWaterStopsDispensingBasedOnTime = 401,
   FaultId_DoorBoardCommunicationFault = 523,
   FaultId_AutofillCouplingFault = 721,
   FaultId_DispenserUiCommunicationFault = 733,
   FaultId_AutofillNoWaterMovementFault = 737,
   FaultId_AutofillTargetRemovedFault = 739,
   FaultId_AutofillEmiDetectedFault = 740,
   FaultId_AndroidSbcCommunicationFault = 756,
   FaultId_AndroidSbcSignOfLifeFault = 757,
   FaultId_EmbeddedWifiCommunicationFault = 758,
   FaultId_InternalTemperatureUiCommunicationFault = 759,
   FaultId_RfidBoardHardwareFailureFault = 800,
   FaultId_RfidBoardCommunicationFault = 801,
   FaultId_RfidBoardTagAuthenticationFailedFault = 803,
   FaultId_RfidBoardLeakDetectedFault = 805,
   FaultId_RfidBoardBlockedTagFault = 806
};
typedef uint16_t FaultId_t;

#endif
