/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FILLTUBEHEATERVOTEDDUTYCYCLE_H
#define FILLTUBEHEATERVOTEDDUTYCYCLE_H

#include "PwmDutyCycle.h"

typedef struct
{
   PwmDutyCycle_t dutyCycle;
   bool care;
} FillTubeHeaterVotedDutyCycle_t;

#endif
