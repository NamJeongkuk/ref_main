/*!
 * @file
 * @brief Humidity expressed as a percentage times 100 for resolution.
 *
 * CopyrightGE Appliances - Confidential - All right reserved.
 */

#ifndef RELATIVEHUMIDITYPERCENTX100_H
#define RELATIVEHUMIDITYPERCENTX100_H

#include <stdint.h>

typedef uint16_t RelativeHumidityPercentx100_t;

#define SWAP_DEFINITION_RelativeHumidityPercentx100_t(_name, _type) \
   SWAPPED_ERD(_name, sizeof(_type)),

#endif
