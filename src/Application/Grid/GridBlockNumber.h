/*!
 * @file
 * @brief Grid Block Number definition.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef GRIDBLOCKNUMBER_H
#define GRIDBLOCKNUMBER_H

#include <stdint.h>

enum
{
   GridBlockNumber_Max = UINT8_MAX
};
typedef uint8_t GridBlockNumber_t;

enum
{
   PreviousGridBlockNumbers_MaxBlockNumbers = 5
};

typedef struct
{
   uint8_t currentNumberOfBlocksStored;
   GridBlockNumber_t blockNumbers[PreviousGridBlockNumbers_MaxBlockNumbers];
} PreviousGridBlockNumbers_t;

#endif
