/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceMakerSlotsPlugin.h"
#include "SystemErds.h"
#include "IceMakerSlotsData.h"
#include "IceMakerTypeInformation.h"

static void WriteInformationErd(I_DataModel_t *dataModel, Erd_t infoErd, IceMakerType_t type, IceMakerLocation_t location)
{
   IceMakerTypeInformation_t info;
   info.location = location;
   info.type = type;

   DataModel_Write(
      dataModel,
      infoErd,
      &info);
}

#define InitializeSlot(number)                                                    \
   if(slotsData->slot##number##Data->enable)                                      \
   {                                                                              \
      switch(slotsData->slot##number##Data->type)                                 \
      {                                                                           \
         case IceMakerType_TwistTray:                                             \
            TwistTrayIceMaker##number##Configurator_Init(                         \
               &instance->_private.slot##number.twistTrayIceMakerConfigurator,    \
               dataModel);                                                        \
            break;                                                                \
         case IceMakerType_AluminumMold:                                          \
            AluminumMoldIceMaker##number##Configurator_Init(                      \
               &instance->_private.slot##number.aluminumMoldIceMakerConfigurator, \
               dataModel);                                                        \
            break;                                                                \
      }                                                                           \
                                                                                  \
      WriteInformationErd(                                                        \
         dataModel,                                                               \
         Erd_IceMaker##number##_TypeInformation,                                  \
         slotsData->slot##number##Data->type,                                     \
         slotsData->slot##number##Data->location);                                \
   }                                                                              \
   else                                                                           \
   {                                                                              \
      WriteInformationErd(                                                        \
         dataModel,                                                               \
         Erd_IceMaker##number##_TypeInformation,                                  \
         IceMakerType_Disabled,                                                   \
         IceMakerLocation_Disabled);                                              \
   }

void IceMakerSlotsPlugin_Init(IceMakerSlotsPlugin_t *instance, I_DataModel_t *dataModel)
{
   const IceMakerSlotsData_t *slotsData =
      PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots;

   InitializeSlot(0);
   InitializeSlot(1);
   InitializeSlot(2);
}
