/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostStateOnCompareMatch.h"
#include "ErdWriterOnCompareMatch.h"
#include "DefrostState.h"
#include "DefrostHsmState.h"
#include "SystemErds.h"

static struct
{
   ErdWriterOnCompareMatch_t erdWriterOnCompareMatch;
} instance;

static const DefrostState_t stateIdle = DefrostState_Idle;
static const DefrostState_t statePrechill = DefrostState_Prechill;
static const DefrostState_t stateHeaterOn = DefrostState_HeaterOn;
static const DefrostState_t stateDwell = DefrostState_Dwell;
static const DefrostState_t stateDisabled = DefrostState_Disabled;

static const DefrostHsmState_t hsmStatePowerUp = DefrostHsmState_PowerUp;
static const DefrostHsmState_t hsmStateIdle = DefrostHsmState_Idle;
static const DefrostHsmState_t hsmStatePrechillPrep = DefrostHsmState_PrechillPrep;
static const DefrostHsmState_t hsmStatePrechill = DefrostHsmState_Prechill;
static const DefrostHsmState_t hsmStatePostPrechill = DefrostHsmState_PostPrechill;
static const DefrostHsmState_t hsmStateHeaterOnEntry = DefrostHsmState_HeaterOnEntry;
static const DefrostHsmState_t hsmStateHeaterOnHeat = DefrostHsmState_HeaterOnHeat;
static const DefrostHsmState_t hsmStateDwell = DefrostHsmState_Dwell;
static const DefrostHsmState_t hsmStatePostDwell = DefrostHsmState_PostDwell;
static const DefrostHsmState_t hsmStateDisabled = DefrostHsmState_Disabled;

static const ErdWriterOnCompareMatchConfigurationEntry_t configurationEntries[] = {
   {
      Erd_DefrostHsmState,
      Erd_DefrostState,
      &hsmStateIdle,
      &stateIdle,
   },
   {
      Erd_DefrostHsmState,
      Erd_DefrostState,
      &hsmStatePowerUp,
      &stateIdle,
   },
   {
      Erd_DefrostHsmState,
      Erd_DefrostState,
      &hsmStatePrechillPrep,
      &statePrechill,
   },
   {
      Erd_DefrostHsmState,
      Erd_DefrostState,
      &hsmStatePrechill,
      &statePrechill,
   },
   {
      Erd_DefrostHsmState,
      Erd_DefrostState,
      &hsmStatePostPrechill,
      &statePrechill,
   },
   {
      Erd_DefrostHsmState,
      Erd_DefrostState,
      &hsmStateHeaterOnEntry,
      &stateHeaterOn,
   },
   {
      Erd_DefrostHsmState,
      Erd_DefrostState,
      &hsmStateHeaterOnHeat,
      &stateHeaterOn,
   },
   {
      Erd_DefrostHsmState,
      Erd_DefrostState,
      &hsmStateDwell,
      &stateDwell,
   },
   {
      Erd_DefrostHsmState,
      Erd_DefrostState,
      &hsmStatePostDwell,
      &stateDwell,
   },
   {
      Erd_DefrostHsmState,
      Erd_DefrostState,
      &hsmStateDisabled,
      &stateDisabled,
   },
};

static const ErdWriterOnCompareMatchConfiguration_t config = {
   configurationEntries,
   NUM_ELEMENTS(configurationEntries)
};

void DefrostStateOnCompareMatch(I_DataModel_t *dataModel)
{
   ErdWriterOnCompareMatch_Init(
      &instance.erdWriterOnCompareMatch,
      DataModel_AsDataSource(dataModel),
      &config);
}
