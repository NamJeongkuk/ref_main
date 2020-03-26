/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ULCONFIG_H
#define ULCONFIG_H

// The address of the expected CRC
// This should be the second-to-last byte of the application
// image if using no boot loader or the STM8-specific boot
// loader
// This should be the address of the application image header
// if using a full boot loader
#define RomCheckCrcAddress 0x87FE

// Where the ROM check should begin
// This should be the first byte of the application image
// if using no boot loader or the STM8-specific boot loader
// This should be the address immediately following the
// image CRC in the application header if using a full boot
// loader
#define RomCheckStart 0x8440

// The last byte of ROM that will be checked
#define RomCheckStop 0x87FD

// How many bytes should be checked each time the periodic
// ROM test is run
#define RomCheckStep 1

#endif
