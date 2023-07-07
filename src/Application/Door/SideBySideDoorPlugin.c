/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideDoorPlugin.h"
#include "SystemErds.h"
#include "AllFreshFoodDoorStatus.h"
#include "ConvertibleCompartmentDoorStateResolver.h"
#include "SabbathInhibitDoors.h"

static struct
{
   AllFreshFoodDoorStatus_t allFreshFoodDoorStatus;
   ConvertibleCompartmentDoorStateResolver_t convertibleCompartmentDoorStateResolver;
   SabbathInhibitDoors_t sabbathInhibitDoors;
} instance;

static const AllFreshFoodDoorStatusConfiguration_t allFreshFoodDoorStatusConfiguration = {
   .rightFreshDoorIsOpenErd = Erd_RightSideFreshFoodDoorStatusResolved,
   .leftFreshDoorIsOpenErd = Erd_LeftSideFreshFoodDoorIsOpenResolved,
   .allFreshFoodDoorsAreClosedErd = Erd_AllFreshFoodDoorsAreClosed
};

static const ConvertibleCompartmentDoorResolverConfiguration_t convertibleCompartmentDoorStateResolverConfig = {
   .convertibleCompartmentDoorIsOpenErd = Erd_ConvertibleCompartmentDoorIsOpen,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .convertibleCompartmentAsFreshFoodDoorIsOpenErd = Erd_ConvertibleCompartmentAsFreshFoodDoorIsOpen,
   .convertibleCompartmentAsFreezerDoorIsOpenErd = Erd_ConvertibleCompartmentAsFreezerDoorIsOpen
};

static const SabbathDoorResolvedPair_t doorResolvedPairs[] = {
   { .doorStatusErd = Erd_LeftSideFreezerDoorStatus, .doorStatusResolvedErd = Erd_LeftSideFreezerDoorStatusResolved },
   { .doorStatusErd = Erd_RightSideFreshFoodDoorStatus, .doorStatusResolvedErd = Erd_RightSideFreshFoodDoorStatusResolved },
};

static const SabbathInhibitDoorsConfiguration_t sabbathInhibitDoorsConfig = {
   .doorResolvedPairErdList = doorResolvedPairs,
   .numberOfPairs = NUM_ELEMENTS(doorResolvedPairs),
   .sabbathModeErd = Erd_SabbathMode,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeStatus
};

static bool HasAConvertibleCompartment(I_DataModel_t *dataModel)
{
   bool hasAConvertibleCompartment;
   DataModel_Read(dataModel, Erd_HasConvertibleCompartment, &hasAConvertibleCompartment);

   return hasAConvertibleCompartment;
}

void SideBySideDoorPlugin_Init(I_DataModel_t *dataModel)
{
   SabbathInhibitDoors_Init(
      &instance.sabbathInhibitDoors,
      dataModel,
      &sabbathInhibitDoorsConfig);

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
