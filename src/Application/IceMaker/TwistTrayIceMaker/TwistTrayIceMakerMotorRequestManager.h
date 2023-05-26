/*!
 * @file
 * @brief This module takes in the winning twist ice maker motor control ERD and handles requesting motor control.
 * Once control is granted it will command the Twist Tray Ice Maker Motor Controller to start action. Once action is complete,
 * it will clear motor control request.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERMOTORREQUESTMANAGER_H
#define TWISTTRAYICEMAKERMOTORREQUESTMANAGER_H

#include "Erd.h"
#include "I_DataModel.h"
#include "EventSubscription.h"
#include "Timer.h"

typedef struct
{
   Erd_t resolvedVoteErd; // TwistTrayIceMakerMotorVotedAction_t
   Erd_t motorRequestErd; // bool
   Erd_t motorEnableErd; // bool
   Erd_t motorActionResultErd; // TwistTrayIceMakerMotorActionResult_t
   Erd_t motorDoActionErd; // TwistTrayIceMakerMotorDoAction_t
} TwistTrayIceMakerMotorRequestManagerConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelEventSubscription;
      I_DataModel_t *dataModel;
      const TwistTrayIceMakerMotorRequestManagerConfig_t *config;
   } _private;
} TwistTrayIceMakerMotorRequestManager_t;

void TwistTrayIceMakerMotorRequestManager_Init(
   TwistTrayIceMakerMotorRequestManager_t *instance,
   I_DataModel_t *dataModel,
   const TwistTrayIceMakerMotorRequestManagerConfig_t *config);

#endif
