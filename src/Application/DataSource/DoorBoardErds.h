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
   DoorBoardErd_PrimaryDoorBoardSignOfLife = 0xF220,
   DoorBoardErd_SecondaryDoorBoardSignOfLife = 0xF221,
};

#endif
