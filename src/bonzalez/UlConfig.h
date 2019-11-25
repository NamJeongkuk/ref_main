/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ULCONFIG_H
#define ULCONFIG_H

enum
{
   // Where the ROM check should begin
   // This should be the first byte of the application image
   RomCheckStart = 0x8440,

   // How many bytes should be checked each time the periodic
   // ROM test is run
   RomCheckStep = 10
};

#endif
