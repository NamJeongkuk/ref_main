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
   Erd_t freezerEvaporatorThermistorIsValidErd; // bool
   Erd_t freshFoodEvaporatorThermistorIsValidErd; // bool
   Erd_t convertibleCompartmentEvaporatorThermistorIsValidErd; // bool
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
      const DefrostData_t *defrostParametricData;
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
   const DefrostHeaterMaxOnTimeConfiguration_t *config);

#endif
