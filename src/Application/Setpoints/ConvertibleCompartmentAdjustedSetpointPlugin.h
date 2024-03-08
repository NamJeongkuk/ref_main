/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTADJUSTEDSETPOINTPLUGIN_H
#define CONVERTIBLECOMPARTMENTADJUSTEDSETPOINTPLUGIN_H

#include "ConvertibleCompartmentShiftOffsetCalculatorPlugin.h"
#include "I16ErdAdder.h"
#include "ResolvedSetpointWriter.h"

typedef struct
{
   struct
   {
      I16ErdAdder_t convertibleCompartmentErdAdderConfig;
      ResolvedSetpointWriter_t convertibleCompartmentResolvedSetpointWriter;
      ConvertibleCompartmentShiftOffsetCalculatorPlugin_t convertibleCompartmentShiftOffsetCalculatorPlugin;
   } _private;
} ConvertibleCompartmentAdjustedSetpointPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void ConvertibleCompartmentAdjustedSetpointPlugin_Init(
   ConvertibleCompartmentAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
