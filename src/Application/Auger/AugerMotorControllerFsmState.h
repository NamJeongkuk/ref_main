/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef AUGERMOTORCONTROLLERFSMSTATE_H
#define AUGERMOTORCONTROLLERFSMSTATE_H

#include <stdint.h>

enum
{
   AugerMotorControllerFsmState_PreIdleDelay,
   AugerMotorControllerFsmState_Idle,
   AugerMotorControllerFsmState_Cubed,
   AugerMotorControllerFsmState_Crushed,
   AugerMotorControllerFsmState_DispensingComplete,
   AugerMotorControllerFsmState_MoveInReverseDirection,
   AugerMotorControllerFsmState_EnsureRelaysAreOff
};
typedef uint8_t AugerMotorControllerFsmState_t;

#endif
