/*!
 * @file
 * @brief Module to resolve max on times for defrost heaters for freezer, fresh food, and convertible compartment
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTHEATERMAXONTIME_H
#define DEFROSTHEATERMAXONTIME_H

#include "I_DataModel.h"
#include "DefrostData.h"

typedef struct
{
   Erd_t freezerEvaporatorThermistorIsValidResolvedErd; // bool
   Erd_t freshFoodEvaporatorThermistorIsValidResolvedErd; // bool
   Erd_t hasConvertibleCompartmentErd; // bool
   Erd_t convertibleCompartmentEvaporatorThermistorIsValidResolvedErd; // bool
   Erd_t convertibleCompartmentStateErd; // ConvertibleCompartmentStateType_t
   Erd_t freshFoodDefrostHeaterMaxOnTimeInMinutesErd; // uint8_t
   Erd_t freezerDefrostHeaterMaxOnTimeInMinutesErd; // uint8_t
   Erd_t convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd; // uint8_t
} DefrostHeaterMaxOnTimeConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t onDataModelChange;
      I_DataModel_t *dataModel;
      const DefrostHeaterOnData_t *heaterOnData;
      const DefrostHeaterMaxOnTimeConfiguration_t *config;
   } _private;
} DefrostHeaterMaxOnTime_t;

/*!
 *
 * @param instance
 * @param dataModel
 * @param config
 */
void DefrostHeaterMaxOnTime_Init(
   DefrostHeaterMaxOnTime_t *instance,
   I_DataModel_t *dataModel,
   const DefrostHeaterMaxOnTimeConfiguration_t *config,
   const DefrostHeaterOnData_t *heaterOnData);

#endif
