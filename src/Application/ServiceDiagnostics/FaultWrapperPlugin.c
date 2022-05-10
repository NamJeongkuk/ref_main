/*!
 * @file
 * @brief Plugs in all the fault wrappers that the mainboard will have
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FaultWrapperPlugin.h"
#include "utils.h"
#include "I_FaultWrapper.h"
#include "FaultWrapper_Consecutive.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "EventSubscription.h"

// clang-format off
// (name, faultOutputErd, consecutiveSets, consecutiveClears)
#define FAULT_WRAPPER_CONSECUTIVE_TABLE(ENTRY) \
   ENTRY(SomeFault, Erd_SomeFault, 3, 3)      \
   ENTRY(SomeOtherFault, Erd_SomeOtherFault, 5, 1)

#define EXPAND_AS_FAULT_WRAPPER_CONSECUTIVE_ENUM(name, _faultOutputErd, consecutiveSets, consecutiveClears)  \
   FaultWrapperIndex_##name,                                                                                \

#define EXPAND_AS_FAULT_WRAPPER_CONSECUTIVE_DATA(name, _faultOutputErd, consecutiveSets, consecutiveClears)  \
   static FaultWrapper_Consecutive_Configuration_t consecutiveFaultWrapperConfiguration_##name = {          \
      .consecutiveRequestsToSetFault = consecutiveSets,                                                     \
      .consecutiveRequestsToClearFault = consecutiveClears,                                                 \
      .faultOutputErd = _faultOutputErd                                                                     \
   };                                                                                                       \
   \
   static FaultWrapper_Consecutive_t consecutiveFaultWrapper_##name;                                        \
   \
   FaultWrapper_Consecutive_Init(                                                                           \
      &consecutiveFaultWrapper_##name,                                                                      \
      &consecutiveFaultWrapperConfiguration_##name,                                                         \
      dataModel);                                                                                           \
   faultWrapperArray[FaultWrapperIndex_##name] = &consecutiveFaultWrapper_##name.interface;                 \

enum 
{
   FAULT_WRAPPER_CONSECUTIVE_TABLE(EXPAND_AS_FAULT_WRAPPER_CONSECUTIVE_ENUM)
   FaultWrapperIndex_NumberConsecutiveFaultWrappers,
   FaultWrapperIndex_Max = FaultWrapperIndex_NumberConsecutiveFaultWrappers
};
// clang-format on

static I_FaultWrapper_t *faultWrapperArray[FaultWrapperIndex_Max];
static EventSubscription_t dataSourceChangeSubscription;

static void DataSourceChanged(void *context, const void *_args)
{
   IGNORE(context);
   REINTERPRET(args, _args, const DataSourceOnDataChangeArgs_t *);

   if(args->erd == Erd_TestFaultSetRequest)
   {
      FaultWrapper_SetRequest(faultWrapperArray[FaultWrapperIndex_SomeFault]);
   }
   else if(args->erd == Erd_TestFaultClearRequest)
   {
      FaultWrapper_ClearRequest(faultWrapperArray[FaultWrapperIndex_SomeFault]);
   }
   else if(args->erd == Erd_TestFaultResetRequest)
   {
      FaultWrapper_Reset(faultWrapperArray[FaultWrapperIndex_SomeFault]);
   }
}

void FaultWrapperPlugin_Init(I_DataModel_t *dataModel)
{
   FAULT_WRAPPER_CONSECUTIVE_TABLE(EXPAND_AS_FAULT_WRAPPER_CONSECUTIVE_DATA)
   DataModelErdPointerAccess_Write(dataModel, Erd_FaultWrapperInterfaceArray, &faultWrapperArray);

   EventSubscription_Init(&dataSourceChangeSubscription, NULL, DataSourceChanged);
   Event_Subscribe(dataModel->OnDataChange, &dataSourceChangeSubscription);
}
