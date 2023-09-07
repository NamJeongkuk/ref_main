/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideDispensePlugin.h"
#include "Constants_Binary.h"
#include "SystemErds.h"
#include "DoorIndex.h"

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
   .dispensingInhibitedErd = Erd_DispensingInhibitedReason,
   .autofillSensorErrorErd = Erd_AutofillSensorError,
   .dispensingDisabledErd = Erd_DispensingDisabled,
   .augerMotorDispensingVoteErd = Erd_AugerMotor_DispensingVote,
   .isolationValveDispensingVoteErd = Erd_IsolationWaterValve_DispensingVote,
   .dispensingValveDispensingVoteErd = Erd_DispenserWaterValve_DispensingVote,
   .timerModuleErd = Erd_TimerModule,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus
};

static const DoorInhibitDispensingResolverDoorConfig_t doorsThatInhibitDispense[] = {
   {
      .doorIsOpenErd = Erd_RightSideFreshFoodDoorStatusResolved,
      .doorIndex = DoorIndex_RightSideFreshFood,
      .offsetInParametricForDoorInhibitsDispense = OFFSET_OF(DoorInhibitDispenseTable_t, rightHandFreshFood),
   },
   {
      .doorIsOpenErd = Erd_LeftSideFreezerDoorStatusResolved,
      .doorIndex = DoorIndex_LeftSideFreezer,
      .offsetInParametricForDoorInhibitsDispense = OFFSET_OF(DoorInhibitDispenseTable_t, leftHandFreezer),
   },
};

static const DoorInhibitDispensingResolverConfig_t doorInhibitWaterDispensingResolverConfig = {
   .dispensingInhibitedErd = Erd_DispensingInhibitedReason,
   .dispensingInhibitedReason = DispensingInhibitedReason_WaterDueToDoorOpen,
   .doorsThatInhibitDispense = doorsThatInhibitDispense,
   .numberOfDoors = NUM_ELEMENTS(doorsThatInhibitDispense)
};

static const DoorInhibitDispensingResolverConfig_t doorInhibitIceDispensingResolverConfig = {
   .dispensingInhibitedErd = Erd_DispensingInhibitedReason,
   .dispensingInhibitedReason = DispensingInhibitedReason_IceDueToDoorOpen,
   .doorsThatInhibitDispense = doorsThatInhibitDispense,
   .numberOfDoors = NUM_ELEMENTS(doorsThatInhibitDispense)
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
