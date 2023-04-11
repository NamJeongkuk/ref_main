/*!
 * @file
 * @brief ice type for auger motor
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef AUGERMOTERVOTEDICETYPE_H
#define AUGERMOTERVOTEDICETYPE_H

#include <stdint.h>
#include "Vote.h"

enum
{
   AugerMotorIceType_Off,
   AugerMotorIceType_Crushed,
   AugerMotorIceType_Cubed
};
typedef uint8_t AugerMotorIceType_t;

typedef struct
{
   AugerMotorIceType_t iceType;
   Vote_t care;
} AugerMotorVotedIceType_t;

#endif
