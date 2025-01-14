/*!
 * @file
 * @brief All the ERDs that we care about on the doorboard.
 * These ERD numbers behave as a contract between the
 * doorboard and mainboard
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DOORBOARDERDS_H
#define DOORBOARDERDS_H

enum
{
   DoorBoardErd_MainboardSignOfLife = 0x0900,
   DoorBoardErd_IceCabinetThermistor_FilteredTemperatureInDegFx100 = 0xF232,
   DoorBoardErd_IceCabinetThermistorIsValid = 0xF233
};

#endif
