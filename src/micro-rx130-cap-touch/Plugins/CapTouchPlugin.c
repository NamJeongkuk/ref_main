/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CapTouchPlugin.h"
#include "CapTouchPluginConfiguration.h"
#include "r_smc_entry.h"
#include "qe_common.h"
#include "r_touch_qe_if.h"
#include "r_ctsu_qe_pinset.h"
#include "KeyState.h"
#include "utils.h"

#define COMMA ,

#define EXPAND_AS_KEY_MASKS(keyId) \
   CONCAT(CONCAT(CapTouchConfigName, _MASK_), keyId),

static const uint64_t keyMasks[] = {
   CAP_TOUCH_KEYS(EXPAND_AS_KEY_MASKS)
};

static TinyTimer_t pollTimer;

static void Poll(void *context, struct TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   REINTERPRET(dataSource, context, I_TinyDataSource_t *);

   R_TOUCH_UpdateDataAndStartScan();

   uint64_t rawState;
   R_TOUCH_GetAllBtnStates(CONCAT(QE_METHOD_, CapTouchConfigName), &rawState);

   CapTouchState_t state = 0;

   for(uint8_t i = NUM_ELEMENTS(keyMasks); i > 0; i--)
   {
      state <<= 1;
      state |= (rawState & keyMasks[i - 1]) == keyMasks[i - 1];
   }

   TinyDataSource_Write(dataSource, CapTouchResultErd, &state);

   TinyTimerModule_Start(
      timerModule,
      &pollTimer,
      CapTouchPollPeriodMsec,
      Poll,
      dataSource);
}

void CapTouchPlugin_Init(I_TinyDataSource_t *dataSource)
{
   R_CTSU_PinSetInit();

   if(R_TOUCH_Open(gp_ctsu_cfgs, gp_touch_cfgs, QE_NUM_METHODS, QE_TRIG_SOFTWARE) != QE_SUCCESS)
   {
      CapTouchSystemReset();
   }

   Poll(dataSource, TinyDataSource_ReadPointer(dataSource, CapTouchTimerModuleErd));
}
