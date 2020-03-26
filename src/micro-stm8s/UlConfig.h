/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ULCONFIG_H
#define ULCONFIG_H

#include "MemoryMap.h"
#include "Crc16.h"

// The address of the expected CRC
// This should be the second-to-last byte of the application
// image if using no boot loader or the STM8-specific boot
// loader
// This should be the address of the application image header
// if using a full boot loader
#define RomCheckCrcAddress ApplicationStart

// Where the ROM check should begin
// This should be the first byte of the application image
// if using no boot loader or the STM8-specific boot loader
// This should be the address immediately following the
// image CRC in the application header if using a full boot
// loader
#define RomCheckStart (ApplicationStart + sizeof(Crc16_t))

// The last byte of ROM that will be checked
#define RomCheckStop ApplicationEnd

// How many bytes should be checked each time the periodic
// ROM test is run
#define RomCheckStep 10

#endif
