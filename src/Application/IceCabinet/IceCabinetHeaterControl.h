/*!
 * @file
 * @brief Calculate the ice cabinet heater duty cycle
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETHEATERCONTROL_H
#define ICECABINETHEATERCONTROL_H

#include "I_DataModel.h"
#include "IceCabinetHeaterData.h"
#include "I_Event.h"

typedef struct
{
   Erd_t iceFormationIsActiveErd; // bool
   Erd_t iceCabinetFanSpeedVoteErd; // FanVotedSpeed_t
   Erd_t iceCabinetHeaterVoteErd; // PercentageDutyCycleVote_t
   Erd_t freezerResolvedSetpointErd; // TemperatureDegFx100_t
} IceCabinetHeaterControlConfig_t;

typedef struct
{
   struct
   {
      const IceCabinetHeaterData_t *iceCabinetHeaterData;
      const IceCabinetHeaterControlConfig_t *config;
      EventSubscription_t onDataModelChangeSubscription;
      I_DataModel_t *dataModel;
   } _private;
} IceCabinetHeaterControl_t;

void IceCabinetHeaterControl_Init(
   IceCabinetHeaterControl_t *instance,
   I_DataModel_t *dataModel,
   const IceCabinetHeaterControlConfig_t *config,
   const IceCabinetHeaterData_t *iceCabinetHeaterData);

#endif
