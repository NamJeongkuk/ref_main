/*!
 * @file
 * @brief The module controls sealed system valve on power up.
 *        The sealed system valve will home, then will move immediately to a position
 *        (power_up_service_position) for power_up_time_in_seconds.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SEALEDSYSTEMVALVECONTROLONPOWERUP_H
#define SEALEDSYSTEMVALVECONTROLONPOWERUP_H

#include "I_DataModel.h"
#include "SealedSystemValveData.h"
#include "Timer.h"

typedef struct
{
   Erd_t sealedSystemValvePositionPowerUpVoteErd;
} SealedSystemValveControlOnPowerUpConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const SealedSystemValveControlOnPowerUpConfig_t *config;
      Timer_t timer;
   } _private;
} SealedSystemValveControlOnPowerUp_t;

void SealedSystemValveControlOnPowerUp_Init(
   SealedSystemValveControlOnPowerUp_t *instance,
   I_DataModel_t *dataModel,
   const SealedSystemValveControlOnPowerUpConfig_t *config,
   const SealedSystemValveData_t *sealedSystemValveData);

#endif
