/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef STAGGEREDHEATER_H
#define STAGGEREDHEATER_H

#include "SoftPwm.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      SoftPwm_t fillTubeHeaterSoftPwm;
      SoftPwm_t freshFoodDamperHeaterSoftPwm;
   } _private;
} SideBySideStaggeredHeater_t;

/*!
 * @param instance
 * @param dataModel
 */
void SideBySideStaggeredHeater_Init(
   SideBySideStaggeredHeater_t *instance,
   I_DataModel_t *dataModel);

#endif
