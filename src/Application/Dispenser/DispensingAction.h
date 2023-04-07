/*!
 * @file
 * @brief Dispensing Action for a Dispensing Request
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSINGACTION_H
#define DISPENSINGACTION_H

#include <stdint.h>

enum
{
   DispensingAction_Stop,
   DispensingAction_Start,
   DispensingAction_Continue,
   DispensingAction_None = UINT8_MAX
};
typedef uint8_t DispensingAction_t;

#endif
