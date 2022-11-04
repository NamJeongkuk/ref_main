/*!
 * @file
 * @brief Aluminum mold ice maker hsm state type
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALUMINUMMOLDICEMAKERHSMSTATE_H
#define ALUMINUMMOLDICEMAKERHSMSTATE_H

#include <stdint.h>

enum
{
   AluminumMoldIceMakerHsmState_Global,
   AluminumMoldIceMakerHsmState_Freeze,
   AluminumMoldIceMakerHsmState_Harvest,
   AluminumMoldIceMakerHsmState_Fill,
   AluminumMoldIceMakerHsmState_HarvestFix,
   AluminumMoldIceMakerHsmState_HarvestFault,
   AluminumMoldIceMakerHsmState_ThermistorFault
};
typedef uint8_t AluminumMoldIceMakerHsmState_t;

#endif
