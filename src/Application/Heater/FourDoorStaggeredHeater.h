/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORSTAGGEREDHEATER_H
#define FOURDOORSTAGGEREDHEATER_H

#include "I_DataModel.h"
#include "SoftPwm.h"

typedef struct
{
   struct
   {
      SoftPwm_t convertibleCompartmentDamperHeaterSoftPwm;
      SoftPwm_t convertibleCompartmentHeaterSoftPwm;
      SoftPwm_t iceMaker0FillTubeHeaterSoftPwm;
      SoftPwm_t iceMaker1FillTubeHeaterSoftPwm;
      SoftPwm_t iceMaker2FillTubeHeaterSoftPwm;
   } _private;
} FourDoorStaggeredHeater_t;

/*!
 * @brief Initialize the staggered heaters for the four door configuration
 * @param instance
 * @param dataModel
 */
void FourDoorStaggeredHeater_Init(FourDoorStaggeredHeater_t *instance,
   I_DataModel_t *dataModel);

#endif
