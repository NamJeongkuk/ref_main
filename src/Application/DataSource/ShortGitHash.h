/*!
 * @file
 * @brief Defines the short Git hash struct. A short Git hash is 4 bytes (8 Hex characters, 32 bits).
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef SHORTGITHASH_H
#define SHORTGITHASH_H

#include <stdint.h>

typedef struct
{
   uint8_t shortGitHash[4];
} ShortGitHash_t;

#endif
