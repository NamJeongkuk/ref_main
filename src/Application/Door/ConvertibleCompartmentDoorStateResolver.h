/*!
 * @file
 * @brief The convertible compartment can act as a fresh food or a freezer
 * depending on its set point. This splits the convertible compartment door status
 * into two separate statuses depending on if it's acting as a fresh food or freezer door.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTDOORSTATERESOLVER_H
#define CONVERTIBLECOMPARTMENTDOORSTATERESOLVER_H

#include "I_DataModel.h"
#include "ConvertibleCompartmentData.h"

typedef struct
{
   Erd_t convertibleCompartmentDoorIsOpenErd; // bool
   Erd_t convertibleCompartmentStateErd; // ConvertibleCompartmentStateType_t
   Erd_t convertibleCompartmentAsFreshFoodDoorIsOpenErd; // bool
   Erd_t convertibleCompartmentAsFreezerDoorIsOpenErd; // bool
} ConvertibleCompartmentDoorResolverConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelSubscription;
      I_DataModel_t *dataModel;
      const ConvertibleCompartmentDoorResolverConfiguration_t *config;
   } _private;
} ConvertibleCompartmentDoorStateResolver_t;

void ConvertibleCompartmentDoorStateResolver_Init(
   ConvertibleCompartmentDoorStateResolver_t *instance,
   I_DataModel_t *dataModel,
   const ConvertibleCompartmentDoorResolverConfiguration_t *config);

#endif
