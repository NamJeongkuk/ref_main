/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GEA2ADDRESSES_H
#define GEA2ADDRESSES_H

enum
{
   Gea2Address_InternalTemperatureUi = 0x01,
   Gea2Address_Dispenser = 0x02,
   Gea2Address_DoorBoard = 0x03,
   Gea2Address_DeliPanBoard = 0x04,
   Gea2Address_RfidBoard = 0x0A,
   Gea2Address_ConvertibleCompartmentBoard = 0x0D,
   Gea2Address_PcToolAddress = 0xE4,
   Gea2Address_EmbeddedWiFi = 0xBF,
   Gea2Address_ConnectPlusWiFi = 0xBE,
   Gea2Address_Mainboard = 0xC0,
   Gea2Address_AndroidSbc = 0xC1,
   Gea2Address_AllBoards = 0xFF,
};

#endif
