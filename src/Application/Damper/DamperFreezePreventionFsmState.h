/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DAMPERFREEZEPREVENTIONFSMSTATE_H
#define DAMPERFREEZEPREVENTIONFSMSTATE_H

enum
{
   DamperFreezePreventionFsmState_Idle,
   DamperFreezePreventionFsmState_MonitoringTemperatureChange,
   DamperFreezePreventionFsmState_DamperHeaterOn,
};
typedef uint8_t DamperFreezePreventionFsmState_t;

#endif
