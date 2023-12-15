/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WINNINGVOTEERD_H
#define WINNINGVOTEERD_H

#include "SystemErds.h"

#define EXPAND_AS_WINNING_VOTE_ERD_ENUM(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   WinningVoteErd_##Name = Name,

enum
{
   ERD_TABLE(EXPAND_AS_WINNING_VOTE_ERD_ENUM)
};

typedef uint16_t WinningVoteErd_t;

#define SWAP_DEFINITION_WinningVoteErd_t(_name, _type) \
   SWAPPED_ERD(_name, sizeof(_type)),

#endif
