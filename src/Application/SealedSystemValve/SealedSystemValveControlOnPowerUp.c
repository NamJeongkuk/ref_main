/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SealedSystemValveControlOnPowerUp.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"

static void Timeout(void *context)
{
   SealedSystemValveControlOnPowerUp_t *instance = context;

   SealedSystemValveVotedPosition_t votedPosition = {
      .position = SealedSystemValvePosition_Home,
      .care = Vote_DontCare
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sealedSystemValvePositionPowerUpVoteErd,
      &votedPosition);
}

void SealedSystemValveControlOnPowerUp_Init(
   SealedSystemValveControlOnPowerUp_t *instance,
   I_DataModel_t *dataModel,
   const SealedSystemValveControlOnPowerUpConfig_t *config,
   const SealedSystemValveData_t *sealedSystemValveData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   SealedSystemValveVotedPosition_t votedPosition = {
      .position = sealedSystemValveData->refrigerantValvePowerUpServicePosition,
      .care = Vote_Care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sealedSystemValvePositionPowerUpVoteErd,
      &votedPosition);

   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer,
      sealedSystemValveData->refrigerantValvePowerUpTimeInSeconds * MSEC_PER_SEC,
      Timeout,
      instance);
}
