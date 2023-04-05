/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSECONTROLLER_H
#define DISPENSECONTROLLER_H

#include "Fsm.h"
#include "Hsm.h"
#include "I_DataModel.h"
#include "Timer.h"
#include "DispenserData.h"

typedef struct
{
   Erd_t privateDispensingRequestErd; // DispensingRequest_t
   Erd_t privateDispensingResultStatusErd; // DispenseStatus_t
   Erd_t dispensingInhibitedByRfidErd; // bool
   Erd_t waterDispensingInhibitedByDoorErd; // bool
   Erd_t iceDispensingInhibitedByDoorErd; // bool
   Erd_t autofillSensorErrorErd; // bool
   Erd_t dispensingDisabledErd; // bool
   Erd_t augerMotorDispensingVoteErd; // AugerMotorVotedIceType_t
   Erd_t isolationValveDispensingVoteErd; // WaterValveVotedState_t
   Erd_t dispensingValveDispensingVoteErd; // WaterValveVotedState_t
   Erd_t flowMeterWaterDispensedOzX100Erd; // uint32_t
   Erd_t flowMeterMonitoringRequestErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
} DispenseControllerConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Timer_t maxDispenseTimer;
      EventSubscription_t onDataModelChangeSubscription;
      Hsm_t hsm;
      const DispenseControllerConfig_t *config;
      const DispenserData_t *dispenserData;
   } _private;
} DispenseController_t;

/*!
 *
 * @param instance instance of DispenseController
 * @param dataModel pointer to DataModel
 * @param data dispenser parametric data
 * @param config dispenser config
 */
void DispenseController_Init(
   DispenseController_t *instance,
   I_DataModel_t *dataModel,
   const DispenserData_t *data,
   const DispenseControllerConfig_t *config);

#endif
