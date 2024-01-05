/*!
 * @file
 * @brief Sets cabinet temperature exceeded fault if
 * cabinet temperature > setpoint + temperature upper hysteresis limit above setpoint
 * and cabinet thermistor is valid
 * and defrost post dwell has been completed for at least a minimum time
 * and all doors have been closed for at least a minimum time
 * and at least some time after power on
 *
 * Clears cabinet temperature exceeded fault if
 * cabinet temperature < setpoint + temperature lower hysteresis limit above setpoint
 * and cabinet thermistor is valid
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CABINETTEMPERATUREEXCEEDEDMONITOR_H
#define CABINETTEMPERATUREEXCEEDEDMONITOR_H

#include "CabinetTemperatureExceededMonitor.h"
#include "I_DataModel.h"
#include "CabinetTemperatureExceededData.h"
#include "ErdList.h"
#include "Timer.h"
#include "Fsm.h"

typedef struct
{
   ErdList_t doorErdList; // ErdList_t
   Erd_t cabinetFilteredTemperatureInDegFX100Erd; // TemperatureDegFx100_t
   Erd_t cabinetThermistorIsValidErd; // bool
   Erd_t cabinetSetpointTemperatureInDegFErd; // Setpoint_t
   Erd_t defrostStateErd; // DefrostState_t
   Erd_t faultErd; // bool
} CabinetTemperatureExceededMonitorConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const CabinetTemperatureExceededMonitorConfiguration_t *config;
      const CabinetTemperatureExceededData_t *data;
      EventSubscription_t dataModelSubscription;
      Timer_t powerUpAndDoorTimer;
      Timer_t defrostTimer;
      bool allDoorsClosedForMinimumTime;
      bool defrostIsInIdleForMinimumTime;
      Fsm_t doorFsm;
      Fsm_t defrostFsm;
   } _private;
} CabinetTemperatureExceededMonitor_t;

void CabinetTemperatureExceededMonitor_Init(
   CabinetTemperatureExceededMonitor_t *instance,
   I_DataModel_t *dataModel,
   const CabinetTemperatureExceededMonitorConfiguration_t *config,
   const CabinetTemperatureExceededData_t *data);

#endif
