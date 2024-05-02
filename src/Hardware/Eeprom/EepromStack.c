/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "EepromStack.h"
#include "InputOutput_Simple.h"
#include "SimpleStorage.h"
#include "Eeprom_HardwareEeprom.h"
#include "HardwareEeprom_I2cMt24xxmmm.h"
#include "I2c_Rx130SerialChannel0.h"
#include "ContextProtector_Rx2xx.h"
#include "utils.h"

typedef struct
{
   Eeprom_HardwareEeprom_t eepromHardwareEepromAdapter;
   HardwareEeprom_I2cMt24xxmmm_t eepromI2cMt24;
   bool readFaultData;
   bool writeFaultData;
   bool eraseFaultData;
   InputOutput_Simple_t readFaultIo;
   InputOutput_Simple_t writeFaultIo;
   InputOutput_Simple_t eraseFaultIo;
} EepromStack_t;

static const HardwareEeprom_I2cMt24xxmmmConfiguration_t eepromConfig = {
   .peripheralAddress_Write = 0xA2,
   .peripheralAddress_Read = 0xA3,
   .numberOfStorageBytes = (32 * 1024),
   .bytesPerPage = 64,
   .byteAlignment = 1,
   .pollingPeriodMilliseconds = 1,
   .valueToWriteWhenErasing = 0xFF,
   .maxTicksAllowedSinceLastResponse = 90,
   .maxObjectErrorsAllowedWhenPolling = 10,
   .i2cBusAccessTimeOut = 20,
   .coolDownTimeOutAfterTransactionCompletes = 5
};

static EepromStack_t instance;

void EepromStack_Init(TimerModule_t *timerModule)
{
   HardwareEeprom_I2cMt24xxmmm_Init(
      &instance.eepromI2cMt24,
      I2c_Rx130SerialChannel0_Init(ContextProtector_Rx2xx_GetInstance()),
      &eepromConfig,
      timerModule,
      ContextProtector_Rx2xx_GetInstance());

   InputOutput_Simple_Init(&instance.readFaultIo, &instance.readFaultData, sizeof(instance.readFaultData));
   InputOutput_Simple_Init(&instance.writeFaultIo, &instance.writeFaultData, sizeof(instance.writeFaultData));
   InputOutput_Simple_Init(&instance.eraseFaultIo, &instance.eraseFaultData, sizeof(instance.eraseFaultData));

   Eeprom_HardwareEeprom_Init(
      &instance.eepromHardwareEepromAdapter,
      &instance.eepromI2cMt24.interface,
      InputOutput_AsOutput(&instance.readFaultIo.interface),
      InputOutput_AsOutput(&instance.writeFaultIo.interface),
      InputOutput_AsOutput(&instance.eraseFaultIo.interface));
}

I_Eeprom_t *EepromStack_GetEeprom(void)
{
   return &instance.eepromHardwareEepromAdapter.interface;
}

I_Input_t *EepromStack_GetReadFaultInput(void)
{
   return InputOutput_AsInput(&instance.readFaultIo.interface);
}

I_Input_t *EepromStack_GetWriteFaultInput(void)
{
   return InputOutput_AsInput(&instance.writeFaultIo.interface);
}

I_Input_t *EepromStack_GetEraseFaultInput(void)
{
   return InputOutput_AsInput(&instance.eraseFaultIo.interface);
}
