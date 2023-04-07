/*!
 * @file
 * @brief Dispensing Request Selection.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSINGREQUESTSELECTION_H
#define DISPENSINGREQUESTSELECTION_H

#include <stdint.h>

enum
{
   DispensingRequestSelection_Water,
   DispensingRequestSelection_CrushedIce,
   DispensingRequestSelection_CubedIce,
   DispensingRequestSelection_Autofill,
   DispensingRequestSelection_None = UINT8_MAX
};
typedef uint8_t DispensingRequestSelection_t;

#endif
