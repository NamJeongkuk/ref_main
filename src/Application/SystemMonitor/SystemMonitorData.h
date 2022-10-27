/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SYSTEMMONITORDATA_H
#define SYSTEMMONITORDATA_H

#include <stdint.h>

typedef struct
{
   uint8_t periodicNvUpdateInMinutes;
   uint8_t periodicNvUpdateDefrostHeaterOnTimeInMinutes;
} SystemMonitorData_t;

#endif
