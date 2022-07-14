/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataSource_Personality.h"
#include "PersonalityId.h"
#include "iodefine.h"
#include "I_DataSource.h"
#include "I_GpioGroup.h"
#include "SystemErds.h"
#include "uassert.h"

#define EXPAND_AS_SET_DIRECTION(name, personalityBit, port, portBit) \
   port.PDR.BIT.B##portBit = 0;

#define EXPAND_AS_SET_MODE(name, personalityBit, port, portBit) \
   port.PMR.BIT.B##portBit = 0;

#define EXPAND_AS_SET_PULLUP(name, personalityBit, port, portBit) \
   port.PCR.BIT.B##portBit = 0;

#define EXPAND_AS_READ_INPUTS(name, personalityBit, port, portBit) \
   personalityId |= port.PIDR.BIT.B##portBit << personalityBit;

#define PERSONALITY_TABLE(ENTRY)      \
   ENTRY(Personality_D0, 0, PORTB, 6) \
   ENTRY(Personality_D1, 1, PORTB, 0) \
   ENTRY(Personality_D2, 2, PORTB, 2) \
   ENTRY(Personality_D3, 3, PORTC, 2) \
   ENTRY(Personality_D4, 4, PORTC, 3) \
   ENTRY(Personality_D5, 5, PORTC, 4) \
   ENTRY(Personality_D6, 6, PORTC, 5) \
   ENTRY(Personality_D7, 7, PORTC, 6)

enum
{
   PersonalityDebounceCount = 10
};

static struct
{
   I_DataSource_t interface;
   AppliancePersonality_t personality;
} instance;

static void InitializeInputs(void)
{
   PERSONALITY_TABLE(EXPAND_AS_SET_DIRECTION)
   PERSONALITY_TABLE(EXPAND_AS_SET_MODE)
   PERSONALITY_TABLE(EXPAND_AS_SET_PULLUP)
}

static AppliancePersonality_t ReadInputs(void)
{
   AppliancePersonality_t personalityId = 0;

   PERSONALITY_TABLE(EXPAND_AS_READ_INPUTS)

   return personalityId;
}

static AppliancePersonality_t GetPersonalityId(void)
{
   AppliancePersonality_t previous = ReadInputs();
   AppliancePersonality_t current = previous + 1;
   uint8_t debounceCount = 0;

   while(debounceCount < PersonalityDebounceCount)
   {
      current = ReadInputs();

      if(previous == current)
      {
         debounceCount++;
      }
      else
      {
         debounceCount = 0;
      }

      previous = current;
   }

   // what if counter is not reached? let watchdog catch it
   return current;
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);

   if(erd == Erd_Bsp_Personality)
   {
      *((AppliancePersonality_t *)data) = instance.personality;
   }
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);
   IGNORE(erd);
   IGNORE(data);
}

static bool Has(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   return erd == Erd_Bsp_Personality;
}

static uint8_t SizeOf(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   IGNORE(erd);
   return sizeof(AppliancePersonality_t);
}

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

I_DataSource_t *DataSource_Personality_Init()
{
   InitializeInputs();
   instance.personality = GetPersonalityId();

   instance.interface.api = &api;

   return &instance.interface;
}
