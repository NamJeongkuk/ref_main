/*!
 * @file
 * @brief Auger motor parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef AUGERMOTORDATA_H
#define AUGERMOTORDATA_H

#include <stdint.h>

typedef struct
{
   uint16_t augerMotorReverseDirectionDelayInMsec;
   uint16_t augerMotorDispenseCompleteInMsec;
   bool augerMotorControlledByMainboard;
} AugerMotorData_t;

#endif
