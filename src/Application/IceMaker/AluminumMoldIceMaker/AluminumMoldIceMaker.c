/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AluminumMoldIceMaker.h"
#include "AluminumMoldIceMakerHsmState.h"
#include "utils.h"

static bool State_Global(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Freeze(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Harvest(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Fill(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HarvestFix(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HarvestFault(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_ThermistorFault(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_Global, HSM_NO_PARENT },
   { State_Freeze, State_Global },
   { State_Harvest, State_Global },
   { State_Fill, State_Global },
   { State_HarvestFix, State_Harvest },
   { State_HarvestFault, State_Harvest },
   { State_ThermistorFault, State_Global }
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static AluminumMoldIceMaker_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(AluminumMoldIceMaker_t, _private.hsm, hsm);
}

static void UpdateHsmStateTo(AluminumMoldIceMaker_t *instance, AluminumMoldIceMakerHsmState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->aluminumMoldIceMakerHsmStateErd,
      &state);
}

static bool State_Global(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_Global);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Freeze(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_Freeze);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Harvest(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_Harvest);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Fill(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_Fill);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_HarvestFix(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_HarvestFix);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_HarvestFault(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_HarvestFault);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_ThermistorFault(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_ThermistorFault);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

void AluminumMoldIceMaker_Init(
   AluminumMoldIceMaker_t *instance,
   I_DataModel_t *dataModel,
   const AluminumMoldIceMakerConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_Freeze);
}
