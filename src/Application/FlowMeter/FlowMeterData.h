/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FLOWMETERDATA_H
#define FLOWMETERDATA_H

#include <stdint.h>

typedef struct
{
   uint16_t flowMeterOzPerPulseX100000;
   int8_t flowMeterOffsetOzX100;
} FlowMeterData_t;

#endif
