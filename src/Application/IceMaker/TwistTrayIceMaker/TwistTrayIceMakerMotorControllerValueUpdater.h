/*!
 * @file
 * @brief This module periodically updates motor controller private values into ERDs
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERMOTORCONTOLLERVALUEUPDATER_H
#define TWISTTRAYICEMAKERMOTORCONTOLLERVALUEUPDATER_H

#include "TwistTrayIceMakerMotorController.h"
#include "Timer.h"

typedef struct
{
   Erd_t motorActionResultErd; // IceMakerMotorActionResult_t
   Erd_t motorOperationStateErd; // IceMakerMotorOperationState_t
   Erd_t motorErrorReasonErd; // IceMakerMotorErrorReason_t
} TwistTrayIceMakerMotorControllerValueUpdaterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      TwistTrayIceMakerMotorController_t *twistTrayMotorController;
      const TwistTrayIceMakerMotorControllerValueUpdaterConfig_t *config;
      Timer_t periodicUpdateTimer;
   } _private;
} TwistTrayIceMakerMotorControllerValueUpdater_t;

void TwistTrayIceMakerMotorControllerValueUpdater_Init(
   TwistTrayIceMakerMotorControllerValueUpdater_t *instance,
   I_DataModel_t *dataModel,
   TwistTrayIceMakerMotorController_t *twistTrayMotorController,
   const TwistTrayIceMakerMotorControllerValueUpdaterConfig_t *config);

#endif
