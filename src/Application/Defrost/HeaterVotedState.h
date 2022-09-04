/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef HEATERVOTEDSTATE_H
#define HEATERVOTEDSTATE_H

#include <stdint.h>

enum
{
   HeaterState_Off,
   HeaterState_On
};
typedef uint8_t HeaterState_t;

typedef struct
{
   HeaterState_t state;
   bool care;
} HeaterVotedState_t;

#endif
