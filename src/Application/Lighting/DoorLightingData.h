/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DOORLIGHTINGDATA_H
#define DOORLIGHTINGDATA_H

#include "RampingPwmDutyCyclePercentageBundleData.h"

typedef struct
{
   const RampingPwmDutyCyclePercentageBundleData_t *normalOperationRampingPwmDutyCycle;
   const RampingPwmDutyCyclePercentageBundleData_t *normalSabbathRampingPwmDutyCycle;
   const RampingPwmDutyCyclePercentageBundleData_t *enhancedSabbathRampingPwmDutyCycle;
   const RampingPwmDutyCyclePercentageBundleData_t *snackModeRampingPwmDutyCycle;
} DoorLightingData_t;

#endif
