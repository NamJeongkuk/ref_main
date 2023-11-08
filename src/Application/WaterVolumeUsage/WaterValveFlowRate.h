/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERVALVEFLOWRATE_H
#define WATERVALVEFLOWRATE_H

#include <stdint.h>

typedef struct
{
   uint8_t aluminumMoldIcemakerValveFlowRateOuncesPerSecX100;
   uint8_t twistTrayIcemakerValveFlowRateOuncesPerSecX100;
   uint8_t dispenserValveFlowRateOuncesPerSecX100;
} WaterValveFlowRate_t;

#endif
