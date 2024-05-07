/*!
 * @file
 * @brief Erd mapping between the mainboard and doorboard
 * The mainboard is behaving as the client and the
 * doorboard is the server. The mainboard
 * is subscribed to the doorboard
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DOORBOARDGEA3MAPPINGS_H
#define DOORBOARDGEA3MAPPINGS_H

#include "SystemErds.h"
#include "DoorBoardErds.h"

// clang-format off

/*
   Published => Subscribe to DoorBoardErd
   OnChangeOnly => Write to DoorBoardErd *only* when ERD changes
      - written only ever on change
   PeriodicAndOnChange => Write to DoorBoardErd periodically *and* on change
      - written at enable (init, most of the time), on subscription host startup, on change, and on period (if enabled)

         DoorBoardErd                                                     PublicErd                                           Published/None    OnChangeOnly/PeriodicAndOnChange/None
*/

#define DOORBOARD_ERD_MAPPINGS(ENTRY) \
   ENTRY(DoorBoardErd_SecondaryDoorBoardSignOfLife,                       PublicErd_SignOfLifeFromDoorboardToMainboard,       Published,        None               ) \
   ENTRY(DoorBoardErd_PrimaryDoorBoardSignOfLife,                         PublicErd_SignOfLifeFromMainboardToDoorboard,       None,             OnChangeOnly       )

// clang-format on

#endif