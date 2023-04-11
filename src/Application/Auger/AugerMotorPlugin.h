/*!
 * @file
 * @brief Initialize modules related to the auger motor
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef AUGERMOTORPLUGIN_H
#define AUGERMOTORPLUGIN_H

#include "I_DataModel.h"
#include "ErdResolver.h"
#include "AugerMotorController.h"

typedef struct
{
   struct
   {
      ErdResolver_t augerMotorErdResolver;
      AugerMotorController_t augerMotorController;
   } _private;
} AugerMotorPlugin_t;

/*!
 *
 * @param instance instance of auger motor plugin
 * @param dataModel pointer to DataModel
 */
void AugerMotorPlugin_Init(
   AugerMotorPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
