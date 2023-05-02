/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DoorPlugin.h"
#include "SystemErds.h"
#include "AllFreshFoodDoorStatus.h"
#include "ConvertibleCompartmentDoorStateResolver.h"

static struct
{
   AllFreshFoodDoorStatus_t allFreshFoodDoorStatus;
   ConvertibleCompartmentDoorStateResolver_t convertibleCompartmentDoorStateResolver;
} instance;

static const AllFreshFoodDoorStatusConfiguration_t allFreshFoodDoorStatusConfiguration = {
   .rightFreshDoorIsOpenErd = Erd_RightSideFreshFoodDoorStatus,
   .leftFreshDoorIsOpenErd = Erd_LeftSideFreshFoodDoorIsOpen,
   .allFreshFoodDoorsAreClosedErd = Erd_AllFreshFoodDoorsAreClosed
};

static const ConvertibleCompartmentDoorResolverConfiguration_t convertibleCompartmentDoorStateResolverConfig = {
   .convertibleCompartmentDoorIsOpenErd = Erd_ConvertibleCompartmentDoorIsOpen,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .convertibleCompartmentAsFreshFoodDoorIsOpenErd = Erd_ConvertibleCompartmentAsFreshFoodDoorIsOpen,
   .convertibleCompartmentAsFreezerDoorIsOpenErd = Erd_ConvertibleCompartmentAsFreezerDoorIsOpen
};

static bool HasAConvertibleCompartment(I_DataModel_t *dataModel)
{
   bool hasAConvertibleCompartment;
   DataModel_Read(dataModel, Erd_HasConvertibleCompartment, &hasAConvertibleCompartment);

   return hasAConvertibleCompartment;
}

void DoorPlugin_Init(I_DataModel_t *dataModel)
{
   AllFreshFoodDoorStatus_Init(
      &instance.allFreshFoodDoorStatus,
      dataModel,
      &allFreshFoodDoorStatusConfiguration);

   if(HasAConvertibleCompartment(dataModel))
   {
      ConvertibleCompartmentDoorStateResolver_Init(
         &instance.convertibleCompartmentDoorStateResolver,
         dataModel,
         &convertibleCompartmentDoorStateResolverConfig);
   }
}
