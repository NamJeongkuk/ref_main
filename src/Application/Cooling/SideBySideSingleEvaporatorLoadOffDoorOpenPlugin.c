/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideSingleEvaporatorLoadOffDoorOpenPlugin.h"
#include "SystemErds.h"

static const LoadVotePair_t freezerCompartmentLoadVotePairs[] = {
   { Erd_FreezerEvapFanSpeed_LoadOffDoorOpenVote, FanSpeed_Off }
};

static const CompartmentVoteErdList_t freezerCompartmentVoteErdList = {
   .pairs = freezerCompartmentLoadVotePairs,
   .numberOfPairs = NUM_ELEMENTS(freezerCompartmentLoadVotePairs)
};

static const Erd_t freezerCompartmentAssociatedDoors[] = {
   Erd_RightSideFreshFoodDoorStatusResolved,
   Erd_LeftSideFreezerDoorStatusResolved
};

static const ErdList_t freezerAssociatedDoorsList = {
   .erds = freezerCompartmentAssociatedDoors,
   .numberOfErds = NUM_ELEMENTS(freezerCompartmentAssociatedDoors),
};

static const LoadOffDoorOpenConfiguration_t freezerConfig = {
   .compartmentVoteErdList = freezerCompartmentVoteErdList,
   .doorStatusErdList = freezerAssociatedDoorsList,
   .timerModuleErd = Erd_TimerModule
};

void SideBySideSingleEvaporatorLoadOffDoorOpenPlugin_Init(
   SideBySideSingleEvaporatorLoadOffDoorOpenPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const LoadOffDoorOpenData_t *loadOffDoorOpenData = PersonalityParametricData_Get(dataModel)->loadOffDoorOpenData;

   if(loadOffDoorOpenData->freezerCompartmentData.enable)
   {
      LoadOffDoorOpen_Init(
         &instance->_private.freezerLoadOffDoorOpen,
         dataModel,
         &loadOffDoorOpenData->freezerCompartmentData,
         &freezerConfig);
   }
}
