/*!
 * @file
 * @brief Status object for CapSense keys
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPSENSESTATUS_H
#define CAPSENSESTATUS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
   bool valid;
   uint16_t word;
} CapSenseStatus_t;

#endif
