/*!
 * @file
 * @brief definition files for compressor time and time tables.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORTIMES_H
#define COMPRESSORTIMES_H

typedef struct
{
   uint8_t minimumOffTimeInMinutes;
   uint8_t minimumOnTimeInMinutes;
   uint8_t startupOnTimeInSeconds;
   uint8_t remainOffAfterValveMoveTimeInMinutes;
   uint8_t remainOffAfterValveMoveLowAmbientTimeInMinutes;
   uint8_t sabbathDelayTimeInSeconds;
   uint8_t compressorOffCoolingOnMaxAllowedTimeInMinutes;
   uint16_t excessiveRunTimeInMinutes;
} CompressorTimes_t;

#endif
