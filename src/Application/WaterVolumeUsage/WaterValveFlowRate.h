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
   uint8_t icemaker0ValveFlowRateOuncesPerSecX100;
   uint8_t icemaker1ValveFlowRateOuncesPerSecX100;
   uint8_t icemaker2ValveFlowRateOuncesPerSecX100;
   uint8_t dispenserValveFlowRateOuncesPerSecX100;
} WaterValveFlowRate_t;

#endif
