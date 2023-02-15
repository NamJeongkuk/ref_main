/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FanControllerPlugin.h"
#include "SystemErds.h"
#include "FanController.h"
#include "DataModelErdPointerAccess.h"
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"

// clang-format off
// ENTRY(parametricDataName,        _calculatedFanSpeedErd,                       _inputCaptureErd,                               _fanPwmErd,                        _actualRpmErd)
#define FAN_CONTROLLER_TABLE(ENTRY)                                                                                                                                                                           \
   ENTRY(condenserFan,              Erd_CalculatedCondenserFanControl,              Erd_CondenserFan_InputCaptureTime,              Erd_CondenserFan_Pwm,              Erd_CondenserFan_ActualRpm)              \
   ENTRY(convertibleCompartmentFan, Erd_CalculatedConvertibleCompartmentFanControl, Erd_ConvertibleCompartmentFan_InputCaptureTime, Erd_ConvertibleCompartmentFan_Pwm, Erd_ConvertibleCompartmentFan_ActualRpm) \
   ENTRY(iceCabinetFan,             Erd_CalculatedIceCabinetFanControl,             Erd_IceCabinetFan_InputCaptureTime,             Erd_IceCabinetFan_Pwm,             Erd_IceCabinetFan_ActualRpm)             \
   ENTRY(deliPanFan,                Erd_CalculatedDeliFanControl,                   Erd_DeliFan_InputCaptureTime,                   Erd_DeliFan_Pwm,                   Erd_DeliFan_ActualRpm)                   \
   ENTRY(freezerEvapFan,            Erd_CalculatedFreezerEvapFanControl,            Erd_FreezerEvapFan_InputCaptureTime,            Erd_FreezerEvapFan_Pwm,            Erd_FreezerEvapFan_ActualRpm)            \
   ENTRY(freshFoodEvapFan,          Erd_CalculatedFreshFoodEvapFanControl,          Erd_FreshFoodEvapFan_InputCaptureTime,          Erd_FreshFoodEvapFan_Pwm,          Erd_FreshFoodEvapFan_ActualRpm)
// clang-format on

#define EXPAND_AS_FAN_CONTROLLERS(_name, _calculatedFanSpeedErd, _inputCaptureErd, _fanPwmErd, _actualRpmErd) \
   static const FanControllerConfig_t _name##ControllerConfig = {                                             \
      .calculatedFanControlErd = _calculatedFanSpeedErd,                                                      \
      .inputCaptureErd = _inputCaptureErd,                                                                    \
      .fanPwmErd = _fanPwmErd,                                                                                \
      .fanActualRpmErd = _actualRpmErd,                                                                       \
   };                                                                                                         \
                                                                                                              \
   static FanController_t _name##Controller;                                                                  \
   FanController_Init(                                                                                        \
      &_name##Controller,                                                                                     \
      dataModel,                                                                                              \
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),                                   \
      &PersonalityParametricData_Get(dataModel)->fanData->_name,                                              \
      &_name##ControllerConfig);

void FanControllerPlugin_Init(I_DataModel_t *dataModel)
{
   FAN_CONTROLLER_TABLE(EXPAND_AS_FAN_CONTROLLERS);
}
