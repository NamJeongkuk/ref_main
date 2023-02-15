/*!
 * @file
 * @brief Define vote struct supporting the voting ERD with different size
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef VOTETYPE_H
#define VOTETYPE_H

#include "Vote.h"

typedef struct
{
   uint8_t value;
   Vote_t vote;
} U8Vote_t;

typedef struct
{
   uint16_t value;
   Vote_t vote;
} U16Vote_t;

typedef struct
{
   uint32_t value;
   Vote_t vote;
} U32Vote_t;

#endif
