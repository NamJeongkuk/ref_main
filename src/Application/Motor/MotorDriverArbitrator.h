/*!
 * @file
 * @brief This arbitrator exists to block the concurrent run of two loads.
 * Thus, this arbitrator only allows either of the motors to be on at the same time.
 * In the case when both motors are requested to be run, only the motor with the faster request is enabled.
 * When the initially enabled motor's request is disabled,
 * the other motor is enabled.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MOTORDRIVERARBITRATOR_H
#define MOTORDRIVERARBITRATOR_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t motor0ControlRequestErd; // bool
   Erd_t motor1ControlRequestErd; // bool
   Erd_t motor0DriveEnableErd; // bool
   Erd_t motor1DriveEnableErd; // bool
} MotorDriverArbitratorConfiguration_t;

typedef struct
{
   struct
   {
      const MotorDriverArbitratorConfiguration_t *config;
      I_DataModel_t *dataModel;
      EventSubscription_t motor0ControlRequestChangeSubscription;
      EventSubscription_t motor1ControlRequestChangeSubscription;
   } _private;
} MotorDriverArbitrator_t;

/*!
 * Initialize a Motor Driver Arbitrator Module
 * @param instance The object instance.
 * @param dataModel instance of dataModel
 * @param config contains erds and other unique configs for instance.
 */
void MotorDriverArbitrator_Init(
   MotorDriverArbitrator_t *instance,
   I_DataModel_t *dataModel,
   const MotorDriverArbitratorConfiguration_t *config);

#endif
