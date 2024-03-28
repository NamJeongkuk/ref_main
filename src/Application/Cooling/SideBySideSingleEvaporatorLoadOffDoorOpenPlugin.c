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

static const LoadOffDoorOpenConfiguration_t freezerConfig = {
   .compartmentVoteErdList = {
      .pairs = freezerCompartmentLoadVotePairs,
      .numberOfPairs = NUM_ELEMENTS(freezerCompartmentLoadVotePairs),
   },
   .timerModuleErd = Erd_TimerModule,
   .doorStatus = {
      .leftSideFreshFoodDoorErd = Erd_Invalid,
      .rightSideFreshFoodDoorErd = Erd_RightSideFreshFoodDoorStatusResolved,
      .leftSideFreezerDoorErd = Erd_LeftSideFreezerDoorStatusResolved,
      .rightSideFreezerDoorErd = Erd_Invalid,
      .convertibleCompartmentDoorErd = Erd_Invalid,
      .doorInDoorErd = Erd_Invalid,
   }
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
