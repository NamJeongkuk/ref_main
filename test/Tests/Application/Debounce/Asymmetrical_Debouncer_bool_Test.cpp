/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Asymmetrical_Debouncer_bool.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"

enum
{
   SomeOffToOnValue = 4,
   SomeOnToOffValue = 5,
};

static const Asymmetrical_Debouncer_bool_Config_t lowInitialValueConfig = {
   .debounceCountOffToOn = SomeOffToOnValue,
   .debounceCountOnToOff = SomeOnToOffValue,
};

static const Asymmetrical_Debouncer_bool_Config_t highInitialValueConfig = {
   .debounceCountOffToOn = SomeOffToOnValue,
   .debounceCountOnToOff = SomeOnToOffValue,
};

static const Asymmetrical_Debouncer_bool_Config_t configWithDebounceCountOffToOnOfOneAndLowInitialValue = {
   .debounceCountOffToOn = 1,
   .debounceCountOnToOff = SomeOnToOffValue,
};

static const Asymmetrical_Debouncer_bool_Config_t configWithDebounceCountOnToOffOfOneAndHighInitialValue = {
   .debounceCountOffToOn = SomeOffToOnValue,
   .debounceCountOnToOff = 1,
};

TEST_GROUP(Asymmetrical_Debouncer_bool)
{
   Asymmetrical_Debouncer_bool_t instance;

   void GivenModuleIsInitializedWithLowInitialValue()
   {
      Asymmetrical_Debouncer_bool_Init(&instance, &lowInitialValueConfig, LOW);
   }

   void GivenModuleIsInitializedWithHighInitialValue()
   {
      Asymmetrical_Debouncer_bool_Init(&instance, &highInitialValueConfig, HIGH);
   }

   void GivenModuleIsInitializedWithDebounceCountOffToOnOfOneAndLowInitialValue()
   {
      Asymmetrical_Debouncer_bool_Init(&instance, &configWithDebounceCountOffToOnOfOneAndLowInitialValue, LOW);
   }

   void GivenModuleIsInitializedWithDebounceCountOnToOffOfOneAndHighInitialValue()
   {
      Asymmetrical_Debouncer_bool_Init(&instance, &configWithDebounceCountOnToOffOfOneAndHighInitialValue, HIGH);
   }

   void DebouncedValueShouldNotChangeWhenProcessingConsecutiveValues(uint8_t numberOfCalls, bool valueToProcess, bool expectedDebouncedValue)
   {
      bool signalChanged = false;
      for(uint8_t i = 0; i < numberOfCalls; i++)
      {
         signalChanged |= Asymmetrical_Debouncer_bool_Process(&instance, valueToProcess);
         CHECK_FALSE(signalChanged);
         CHECK_EQUAL(Asymmetrical_Debouncer_bool_GetDebounced(&instance), expectedDebouncedValue);
      }
   }

   void DebouncedValueShouldChangeWhenProcessedWith(bool valueToProcess)
   {
      CHECK_TRUE(Asymmetrical_Debouncer_bool_Process(&instance, valueToProcess));
   }

   void DebouncedValueShouldBe(bool expectedDebounceValue)
   {
      CHECK_EQUAL(Asymmetrical_Debouncer_bool_GetDebounced(&instance), expectedDebounceValue);
   }
};

TEST(Asymmetrical_Debouncer_bool, ShouldKeepOutputLowIfInitialValueIsLowForOffToOnCountsThenChangeOutputToHigh)
{
   GivenModuleIsInitializedWithLowInitialValue();

   DebouncedValueShouldNotChangeWhenProcessingConsecutiveValues((SomeOffToOnValue - 1), HIGH, LOW);

   DebouncedValueShouldChangeWhenProcessedWith(HIGH);
   DebouncedValueShouldBe(HIGH);
}

TEST(Asymmetrical_Debouncer_bool, ShouldKeepOutputHighIfInitialValueIsHighForOnToOffCountsThenChangeOutputToLow)
{
   GivenModuleIsInitializedWithHighInitialValue();

   DebouncedValueShouldNotChangeWhenProcessingConsecutiveValues((SomeOnToOffValue - 1), LOW, HIGH);

   DebouncedValueShouldChangeWhenProcessedWith(LOW);
   DebouncedValueShouldBe(LOW);
}

TEST(Asymmetrical_Debouncer_bool, ShouldKeepDebouncedSignalHighIfTrueIsProcessedMoreThanOnToOffCounts)
{
   GivenModuleIsInitializedWithHighInitialValue();

   DebouncedValueShouldNotChangeWhenProcessingConsecutiveValues(SomeOnToOffValue + 1, HIGH, HIGH);
}

TEST(Asymmetrical_Debouncer_bool, ShouldKeepDebouncedSignalLowIfFalseIsProcessedMoreThanOnToOffCounts)
{
   GivenModuleIsInitializedWithLowInitialValue();

   DebouncedValueShouldNotChangeWhenProcessingConsecutiveValues(SomeOffToOnValue + 1, LOW, LOW);
}

TEST(Asymmetrical_Debouncer_bool, ShouldChangeDebouncedSignalToHighAfterOneProcessCallWithLowInitialValueAndDebounceCountOf1)
{
   GivenModuleIsInitializedWithDebounceCountOffToOnOfOneAndLowInitialValue();

   DebouncedValueShouldChangeWhenProcessedWith(HIGH);
   DebouncedValueShouldBe(HIGH);
}

TEST(Asymmetrical_Debouncer_bool, ShouldChangeDebouncedSignalToLowAfterOneProcessCallWithHighInitialValueAndDebounceCountOf1)
{
   GivenModuleIsInitializedWithDebounceCountOnToOffOfOneAndHighInitialValue();

   DebouncedValueShouldChangeWhenProcessedWith(LOW);
   DebouncedValueShouldBe(LOW);
}

TEST(Asymmetrical_Debouncer_bool, ShouldToggleDebouncedSignalAfterEnoughProcessCalls)
{
   GivenModuleIsInitializedWithLowInitialValue();

   for(uint8_t i = 0; i < SomeOffToOnValue; i++)
   {
      Asymmetrical_Debouncer_bool_Process(&instance, HIGH);
   }

   DebouncedValueShouldNotChangeWhenProcessingConsecutiveValues(SomeOnToOffValue - 1, LOW, HIGH);

   DebouncedValueShouldChangeWhenProcessedWith(LOW);
   DebouncedValueShouldBe(LOW);
}

TEST(Asymmetrical_Debouncer_bool, ShouldResetLowToHighDebouncedSignalAfterLowValue)
{
   GivenModuleIsInitializedWithLowInitialValue();

   for(uint8_t i = 0; i < (SomeOffToOnValue - 1); i++)
   {
      Asymmetrical_Debouncer_bool_Process(&instance, HIGH);
   }

   DebouncedValueShouldNotChangeWhenProcessingConsecutiveValues(1, LOW, LOW);

   DebouncedValueShouldNotChangeWhenProcessingConsecutiveValues(SomeOffToOnValue - 1, HIGH, LOW);

   DebouncedValueShouldChangeWhenProcessedWith(HIGH);
   DebouncedValueShouldBe(HIGH);
}

TEST(Asymmetrical_Debouncer_bool, ShouldResetHighToLowDebouncedSignalAfterHighValue)
{
   GivenModuleIsInitializedWithHighInitialValue();

   for(uint8_t i = 0; i < (SomeOnToOffValue - 1); i++)
   {
      Asymmetrical_Debouncer_bool_Process(&instance, LOW);
   }

   DebouncedValueShouldNotChangeWhenProcessingConsecutiveValues(1, HIGH, HIGH);

   DebouncedValueShouldNotChangeWhenProcessingConsecutiveValues(SomeOnToOffValue - 1, LOW, HIGH);

   DebouncedValueShouldChangeWhenProcessedWith(LOW);
   DebouncedValueShouldBe(LOW);
}
