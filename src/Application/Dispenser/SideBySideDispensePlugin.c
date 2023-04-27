/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideDispensePlugin.h"
#include "Constants_Binary.h"
#include "SystemErds.h"

static const DispenseSelectionRequestHandlerConfig_t dispenseSelectionRequestHandlerConfig = {
   .dispenseSelectionRequestErd = Erd_DispenseSelectionRequest,
   .dispenseSelectionStatusErd = Erd_DispenseSelectionStatus
};

static const DispensingRequestHandlerConfig_t dispensingRequestHandlerConfig = {
   .dispensingRequestErd = Erd_DispensingRequest,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus,
   .privateDispensingRequestErd = Erd_PrivateDispensingRequest,
   .privateDispensingResultStatusErd = Erd_PrivateDispensingResultStatus
};

static const DispenseControllerConfig_t dispenseControllerConfig = {
   .privateDispensingRequestErd = Erd_PrivateDispensingRequest,
   .privateDispensingResultStatusErd = Erd_PrivateDispensingResultStatus,
   .dispensingInhibitedByRfidErd = Erd_DispensingInhibitedByRfid,
   .waterDispensingInhibitedByDoorErd = Erd_WaterDispensingInhibitedByDoor,
   .iceDispensingInhibitedByDoorErd = Erd_IceDispensingInhibitedByDoor,
   .autofillSensorErrorErd = Erd_AutofillSensorError,
   .dispensingDisabledErd = Erd_DispensingDisabled,
   .augerMotorDispensingVoteErd = Erd_AugerMotor_DispensingVote,
   .isolationValveDispensingVoteErd = Erd_IsolationWaterValve_DispensingVote,
   .dispensingValveDispensingVoteErd = Erd_DispenserWaterValve_DispensingVote,
   .timerModuleErd = Erd_TimerModule
};

static const DoorInhibitDispensingResolverConfig_t doorInhibitWaterDispensingResolverConfig = {
   .leftHandFreshFoodDoorIsOpenErd = Erd_LeftHandFreshFoodDoorIsOpen,
   .rightHandFreshFoodDoorIsOpenErd = Erd_RightHandFreshFoodDoorIsOpen,
   .leftHandFreezerDoorIsOpenErd = Erd_FreezerDoorIsOpen,
   .rightHandFreezerDoorIsOpenErd = Erd_Invalid,
   .convertibleCompartmentDoorIsOpenErd = Erd_ConvertibleCompartmentDoorIsOpen,
   .doorInDoorIsOpenErd = Erd_DoorInDoorIsOpen,
   .dispensingInhibitedByDoorErd = Erd_WaterDispensingInhibitedByDoor
};

static const DoorInhibitDispensingResolverConfig_t doorInhibitIceDispensingResolverConfig = {
   .leftHandFreshFoodDoorIsOpenErd = Erd_LeftHandFreshFoodDoorIsOpen,
   .rightHandFreshFoodDoorIsOpenErd = Erd_RightHandFreshFoodDoorIsOpen,
   .leftHandFreezerDoorIsOpenErd = Erd_FreezerDoorIsOpen,
   .rightHandFreezerDoorIsOpenErd = Erd_Invalid,
   .convertibleCompartmentDoorIsOpenErd = Erd_ConvertibleCompartmentDoorIsOpen,
   .doorInDoorIsOpenErd = Erd_DoorInDoorIsOpen,
   .dispensingInhibitedByDoorErd = Erd_IceDispensingInhibitedByDoor
};

void SideBySideDispensePlugin_Init(SideBySideDispensePlugin_t *instance, I_DataModel_t *dataModel)
{
   DispenseSelectionRequestHandler_Init(
      &instance->_private.dispenseSelectionRequestHandler,
      &dispenseSelectionRequestHandlerConfig,
      dataModel);

   DispensingRequestHandler_Init(
      &instance->_private.dispensingRequestHandler,
      dataModel,
      &dispensingRequestHandlerConfig);

   DispenseController_Init(
      &instance->_private.dispenseController,
      dataModel,
      PersonalityParametricData_Get(dataModel)->dispenserData,
      &dispenseControllerConfig);

   DoorInhibitDispensingResolver_Init(
      &instance->_private.doorInhibitWaterDispensingResolver,
      dataModel,
      &doorInhibitWaterDispensingResolverConfig,
      PersonalityParametricData_Get(dataModel)->dispenserData->doorInhibitWaterDispenseTable);

   DoorInhibitDispensingResolver_Init(
      &instance->_private.doorInhibitIceDispensingResolver,
      dataModel,
      &doorInhibitIceDispensingResolverConfig,
      PersonalityParametricData_Get(dataModel)->dispenserData->doorInhibitIceDispenseTable);
}
