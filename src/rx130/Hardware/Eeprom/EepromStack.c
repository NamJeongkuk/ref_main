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
#include "HardwareEeprom_I2c.h"
#include "utils.h"

typedef struct
{
   Eeprom_HardwareEeprom_t eepromHardwareEepromAdapter;
   bool readFaultData;
   bool writeFaultData;
   bool eraseFaultData;
   InputOutput_Simple_t readFaultIo;
   InputOutput_Simple_t writeFaultIo;
   InputOutput_Simple_t eraseFaultIo;
} EepromStack_t;

static EepromStack_t instance;

void EepromStack_Init(I_Action_t *watchdogKickAction)
{
   HardwareEeprom_I2c_t *hardwareEepromI2c;
   hardwareEepromI2c = HardwareEeprom_I2c_Init(watchdogKickAction);

   InputOutput_Simple_Init(&instance.readFaultIo, &instance.readFaultData, sizeof(instance.readFaultData));
   InputOutput_Simple_Init(&instance.writeFaultIo, &instance.writeFaultData, sizeof(instance.writeFaultData));
   InputOutput_Simple_Init(&instance.eraseFaultIo, &instance.eraseFaultData, sizeof(instance.eraseFaultData));

   Eeprom_HardwareEeprom_Init(
      &instance.eepromHardwareEepromAdapter,
      &hardwareEepromI2c->interface,
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
