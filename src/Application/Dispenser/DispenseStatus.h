/*!
 * @file
 * @brief Dispense Status from the Dispensing Result Status.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSESTATUS_H
#define DISPENSESTATUS_H

#include <stdint.h>

enum
{
   DispenseStatus_CompletedSuccessfully,
   DispenseStatus_Dispensing,
   DispenseStatus_SensorError,
   DispenseStatus_DisabledOrBlocked,
   DispenseStatus_HitMaxVolume,
   DispenseStatus_HitMaxTime,
   DispenseStatus_DidNotReceiveDispenseRequestAfterHeartbeat,
   DispenseStatus_PreciseFillAmountReached,
   DispenseStatus_BadCommand,
   DispenseStatus_DispenseInhibitedDueToDoorOpen,
   DispenseStatus_DispenseInhibitedBecauseRedundantCupSwitchPressIsRequired,
   DispenseStatus_DispenseInhibitedDueToRfidErrorOrLeak,
};
typedef uint8_t DispenseStatus_t;

#endif
