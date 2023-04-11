/*!
 * @file
 * @brief Auger motor controller which takes in a auger motor voted ice type
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef AUGERMOTORCONTROLLER_H
#define AUGERMOTORCONTROLLER_H

#include "I_DataModel.h"
#include "Fsm.h"
#include "AugerMotorData.h"
#include "AugerMotorVotedIceType.h"
#include "Timer.h"

typedef struct
{
   Erd_t augerMotorControllerFsmStateErd; // AugerMotorControllerFsmState_t
   Erd_t augerMotorIceTypeVoteErd; // AugerMotorVotedIceType_t
   Erd_t augerMotorPowerErd; // bool
   Erd_t augerMotorDirectionErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
} AugerMotorControllerConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t augerMotorIceTypeVoteChangeSubscription;
      const AugerMotorData_t *augerMotorData;
      const AugerMotorControllerConfig_t *config;
      AugerMotorIceType_t storedAugerMotorIceType;
      Fsm_t fsm;
      Timer_t timer;
   } _private;
} AugerMotorController_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param augerMotorData
 */
void AugerMotorController_Init(
   AugerMotorController_t *instance,
   I_DataModel_t *dataModel,
   const AugerMotorControllerConfig_t *config,
   const AugerMotorData_t *augerMotorData);

#endif
