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
#include "FaultWrapper_Count.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"

// clang-format off
// (name, faultOutputErd, consecutiveSets, consecutiveClears)
#define FAULT_WRAPPER_CONSECUTIVE_TABLE(ENTRY)     \
   ENTRY(SomeFault, Erd_SomeFault, 3, 3)           \

// (name, faultOutputErd, requestsToSet, numberRequestsToConsider)
#define FAULT_WRAPPER_COUNT_TABLE(ENTRY)           \
   ENTRY(SomeOtherFault, Erd_SomeOtherFault, 3, 10) \

#define EXPAND_AS_FAULT_WRAPPER_CONSECUTIVE_ENUM(name, _faultOutputErd, consecutiveSets, consecutiveClears)  \
   FaultWrapperIndex_##name,                                                                                 \

#define EXPAND_AS_FAULT_WRAPPER_COUNT_ENUM(name, _faultOutputErd, requestsToSet, numberRequestsToConsider)   \
   FaultWrapperIndex_##name,

#define EXPAND_AS_FAULT_WRAPPER_CONSECUTIVE_DATA(name, _faultOutputErd, consecutiveSets, consecutiveClears)  \
   static const FaultWrapper_Consecutive_Configuration_t consecutiveFaultWrapperConfiguration_##name = {     \
      .consecutiveRequestsToSetFault = consecutiveSets,                                                      \
      .consecutiveRequestsToClearFault = consecutiveClears,                                                  \
      .faultOutputErd = _faultOutputErd                                                                      \
   };                                                                                                        \
   \
   static FaultWrapper_Consecutive_t consecutiveFaultWrapper_##name;                                         \
   \
   FaultWrapper_Consecutive_Init(                                                                            \
      &consecutiveFaultWrapper_##name,                                                                       \
      &consecutiveFaultWrapperConfiguration_##name,                                                          \
      dataModel);                                                                                            \
   faultWrapperArray[FaultWrapperIndex_##name] = &consecutiveFaultWrapper_##name.interface;                  \

#define EXPAND_AS_FAULT_WRAPPER_COUNT_DATA(name, _faultOutputErd, _requestsToSetFault, _totalRequestsToConsider)  \
   static const FaultWrapper_Count_Configuration_t countFaultWrapperConfiguration_##name = {                      \
      .requestsToSetFault = _requestsToSetFault,                                                                  \
      .totalRequestsToConsider = _totalRequestsToConsider,                                                        \
      .faultOutputErd = _faultOutputErd,                                                                          \
   };                                                                                                             \
   \
   static FaultWrapper_Count_t countFaultWrapper_##name;                                                          \
   FaultWrapper_Count_Init(                                                                                       \
      &countFaultWrapper_##name,                                                                                  \
      &countFaultWrapperConfiguration_##name,                                                                     \
      dataModel);                                                                                                 \
   faultWrapperArray[FaultWrapperIndex_##name] = &countFaultWrapper_##name.interface;                             \

enum
{
   FAULT_WRAPPER_CONSECUTIVE_TABLE(EXPAND_AS_FAULT_WRAPPER_CONSECUTIVE_ENUM)
   FAULT_WRAPPER_COUNT_TABLE(EXPAND_AS_FAULT_WRAPPER_COUNT_ENUM)
   FaultWrapperIndex_Max
};
// clang-format on

static I_FaultWrapper_t *faultWrapperArray[FaultWrapperIndex_Max];

static EventSubscription_t dataSourceChangeSubscription;

static void DataSourceChanged(void *context, const void *_args)
{
   IGNORE(context);
   REINTERPRET(args, _args, const DataSourceOnDataChangeArgs_t *);

   if(args->erd == Erd_TestFaultWrapperCountSetRequest)
   {
      FaultWrapper_SetRequest(faultWrapperArray[FaultWrapperIndex_SomeOtherFault]);
   }
   else if(args->erd == Erd_TestFaultWrapperCountClearRequest)
   {
      FaultWrapper_ClearRequest(faultWrapperArray[FaultWrapperIndex_SomeOtherFault]);
   }
   else if(args->erd == Erd_TestFaultWrapperCountResetRequest)
   {
      FaultWrapper_Reset(faultWrapperArray[FaultWrapperIndex_SomeOtherFault]);
   }
   else if(args->erd == Erd_TestFaultWrapperConsecutiveSetRequest)
   {
      FaultWrapper_SetRequest(faultWrapperArray[FaultWrapperIndex_SomeFault]);
   }
   else if(args->erd == Erd_TestFaultWrapperConsecutiveClearRequest)
   {
      FaultWrapper_ClearRequest(faultWrapperArray[FaultWrapperIndex_SomeFault]);
   }
   else if(args->erd == Erd_TestFaultWrapperConsecutiveResetRequest)
   {
      FaultWrapper_Reset(faultWrapperArray[FaultWrapperIndex_SomeFault]);
   }
}

/*!
 * @param dataModel
 */
void FaultWrapperPlugin_Init(I_DataModel_t *dataModel)
{
   FAULT_WRAPPER_CONSECUTIVE_TABLE(EXPAND_AS_FAULT_WRAPPER_CONSECUTIVE_DATA)
   FAULT_WRAPPER_COUNT_TABLE(EXPAND_AS_FAULT_WRAPPER_COUNT_DATA)
   DataModelErdPointerAccess_Write(dataModel, Erd_FaultWrapperInterfaceArray, &faultWrapperArray);

   EventSubscription_Init(&dataSourceChangeSubscription, NULL, DataSourceChanged);
   Event_Subscribe(dataModel->OnDataChange, &dataSourceChangeSubscription);
}
