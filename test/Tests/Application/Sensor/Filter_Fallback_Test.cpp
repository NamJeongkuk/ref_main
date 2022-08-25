/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Filter_Fallback.h"
#include "utils.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "DataModel_TestDouble.h"
#include "Filter_TestDouble.h"

enum
{
   InvalidValueDegFx100 = 30000,
   FallbackValueDegFx100 = 200,
   GoodReadingCounterMax = 10,
   BadReadingCounterMax = 5,
   ValidDegFx100 = 345
};

enum
{
   Erd_ThermistorIsValid
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_ThermistorIsValid, sizeof(bool) }
};

TEST_GROUP(Filter_Fallback)
{
   Filter_Fallback_t instance;
   Filter_TestDouble_t wrappedFilter;
   int16_t seedBuffer;
   int16_t feedBuffer;
   int16_t readBuffer;

   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      Filter_TestDouble_Init(&wrappedFilter, &seedBuffer, &feedBuffer, &readBuffer, sizeof(seedBuffer));
   }

   void GivenTheFilterHasBeenInitialized()
   {
      Filter_Fallback_Init(
         &instance,
         &wrappedFilter.interface,
         dataModel,
         InvalidValueDegFx100,
         FallbackValueDegFx100,
         GoodReadingCounterMax,
         BadReadingCounterMax,
         Erd_ThermistorIsValid);
   }

   void AfterTheFilterIsSeededWith(int16_t value)
   {
      Filter_Seed(&instance.interface, &value);
   }

   void TheWrappedFilterShouldHaveBeenSeededWith(int16_t expected)
   {
      int16_t actual;
      Filter_TestDouble_GetLastSeedValue(&wrappedFilter, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void AfterTheFilterIsFedWith(int16_t value)
   {
      Filter_Feed(&instance.interface, &value);
   }

   void TheWrappedFilterShouldHaveBeenFedWith(int16_t expected)
   {
      int16_t actual;
      Filter_TestDouble_GetLastFeedValue(&wrappedFilter, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheLastValueTheWrappedFilterWasFedShouldHaveBeen(int16_t expected)
   {
      TheWrappedFilterShouldHaveBeenFedWith(expected);
   }

   void TheFilterShouldRead(int16_t expected)
   {
      int16_t actual;
      Filter_Read(&instance.interface, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheWrappedFilterCurrentFilterValueIs(int16_t value)
   {
      Filter_TestDouble_SetCurrentFilterValue(&wrappedFilter, &value);
   }

   void GivenTheWrappedFilterIsReady()
   {
      Filter_TestDouble_ChangeReadyStateTo(&wrappedFilter, true);
   }

   void GivenTheWrappedFilterIsNotReady()
   {
      Filter_TestDouble_ChangeReadyStateTo(&wrappedFilter, false);
   }

   void TheFilterShouldBeReady()
   {
      I_Input_t *readyInput = Filter_GetReadyInput(&instance.interface);
      bool ready;
      Input_Read(readyInput, &ready);

      CHECK_TRUE(ready);
   }

   void TheFilterShouldBeNotReady()
   {
      I_Input_t *readyInput = Filter_GetReadyInput(&instance.interface);
      bool ready;
      Input_Read(readyInput, &ready);

      CHECK_FALSE(ready);
   }

   void WhenTheFilterIsReset()
   {
      Filter_Reset(&instance.interface);
   }

   void GivenTheValidThermistorErdIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ThermistorIsValid, &state);
   }

   void TheValidThermistorErdShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_ThermistorIsValid, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheFilterHasBeenSeededWithAValidValue()
   {
      GivenTheFilterHasBeenInitialized();
      AfterTheFilterIsSeededWith(2);
      TheWrappedFilterShouldHaveBeenSeededWith(2);
   }

   void GivenTheFilterHasBeenSeededWithAnInvalidValue()
   {
      GivenTheFilterHasBeenInitialized();
      GivenTheWrappedFilterIsNotReady();

      AfterTheFilterIsSeededWith(InvalidValueDegFx100);
      TheFilterShouldBeNotReady();
   }

   void GivenTheFilterHasBeenFedWithBadReadingCountingMaxNumberOfInvalidValuesAndIsReadingTheFallbackValue()
   {
      GivenTheFilterHasBeenSeededWithAValidValue();

      for(uint8_t i = 0; i < BadReadingCounterMax; i++)
      {
         AfterTheFilterIsFedWith(InvalidValueDegFx100);
      }

      TheFilterShouldRead(FallbackValueDegFx100);
   }

   void GivenTheFilterSwitchedFromBadReadingCountingMaxNumberOfInvalidValuesToGoodReadingCountingMaxNumberOfValidValues()
   {
      GivenTheFilterHasBeenFedWithBadReadingCountingMaxNumberOfInvalidValuesAndIsReadingTheFallbackValue();

      AfterFilterIsFedGoodReadingCountingMaxNumberOfValidValues();
   }

   void GivenTheFilterSwitchedFromBadReadingCountingMaxNumberOfInvalidValuesToGoodReadingCountingMaxNumberOfValidValuesToBadReadingCountingMaxNumberOfInvalidValues()
   {
      GivenTheFilterSwitchedFromBadReadingCountingMaxNumberOfInvalidValuesToGoodReadingCountingMaxNumberOfValidValues();
      TheWrappedFilterCurrentFilterValueIs(ValidDegFx100);

      AfterFilterIsFedBadReadingCountingMaxNumberOfInvalidValues();
   }

   void AfterFilterIsFedGoodReadingCountingMaxNumberOfValidValues()
   {
      for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
      {
         AfterTheFilterIsFedWith(ValidDegFx100);
         TheFilterShouldRead(FallbackValueDegFx100);
      }

      AfterTheFilterIsFedWith(ValidDegFx100);
      TheWrappedFilterCurrentFilterValueIs(ValidDegFx100);
   }

   void AfterFilterIsFedBadReadingCountingMaxNumberOfInvalidValues()
   {
      for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
      {
         AfterTheFilterIsFedWith(InvalidValueDegFx100);
         TheFilterShouldRead(ValidDegFx100);
      }

      AfterTheFilterIsFedWith(InvalidValueDegFx100);
   }
};

TEST(Filter_Fallback, ShouldSeedTheWrappedFilterWithSameValueWhenSeeded)
{
   GivenTheFilterHasBeenInitialized();
   AfterTheFilterIsSeededWith(2);
   TheWrappedFilterShouldHaveBeenSeededWith(2);
}

TEST(Filter_Fallback, ShouldFeedTheWrappedFilterWithSameValueIfItIsValid)
{
   GivenTheFilterHasBeenSeededWithAValidValue();

   AfterTheFilterIsFedWith(20);
   TheWrappedFilterShouldHaveBeenFedWith(20);
}

TEST(Filter_Fallback, ShouldNotSeedTheWrappedFilterIfItIsInvalid)
{
   GivenTheFilterHasBeenInitialized();
   GivenTheWrappedFilterIsNotReady();

   AfterTheFilterIsSeededWith(InvalidValueDegFx100);
   TheFilterShouldBeNotReady();
}

TEST(Filter_Fallback, ShouldReadTheWrappedFilterValueWhenSeededWithAValidValue)
{
   GivenTheFilterHasBeenSeededWithAValidValue();
   TheWrappedFilterCurrentFilterValueIs(2);

   TheFilterShouldRead(2);
}

TEST(Filter_Fallback, ShouldReadTheFallbackValueWhenSeededWithAnInvalidValue)
{
   GivenTheFilterHasBeenSeededWithAnInvalidValue();

   TheFilterShouldRead(FallbackValueDegFx100);
}

TEST(Filter_Fallback, ShouldWriteTrueToValidThermistorErdWhenSeededWithValidValue)
{
   GivenTheValidThermistorErdIs(false);
   GivenTheFilterHasBeenSeededWithAValidValue();

   TheValidThermistorErdShouldBe(true);
}

TEST(Filter_Fallback, ShouldWriteFalseToValidThermistorErdWhenSeededWithInvalidValue)
{
   GivenTheValidThermistorErdIs(true);
   GivenTheFilterHasBeenSeededWithAnInvalidValue();

   TheValidThermistorErdShouldBe(false);
}

TEST(Filter_Fallback, ShouldSwitchThermistorToValidAfterGoodReadingCounterMaxNumberOfValidValuesWhenThermistorIsInvalidOnInit)
{
   GivenTheValidThermistorErdIs(true);
   GivenTheFilterHasBeenSeededWithAnInvalidValue();

   TheValidThermistorErdShouldBe(false);

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(ValidDegFx100);
      TheValidThermistorErdShouldBe(false);
   }

   AfterTheFilterIsFedWith(ValidDegFx100);
   TheWrappedFilterShouldHaveBeenSeededWith(ValidDegFx100);
   TheValidThermistorErdShouldBe(true);
}

TEST(Filter_Fallback, ShouldSwitchThermistorToInvalidAfterBadReadingCounterMaxNumberOfValidValuesWhenThermistorIsValidOnInit)
{
   GivenTheValidThermistorErdIs(false);
   GivenTheFilterHasBeenSeededWithAValidValue();

   TheValidThermistorErdShouldBe(true);

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
      TheValidThermistorErdShouldBe(true);
   }

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheValidThermistorErdShouldBe(false);
}

TEST(Filter_Fallback, ShouldReadTheFallbackValueAfterBeingFedBadReadingCounterMaxNumberOfInvalidValues)
{
   GivenTheFilterHasBeenSeededWithAValidValue();
   TheWrappedFilterCurrentFilterValueIs(ValidDegFx100);

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
      TheFilterShouldRead(ValidDegFx100);
   }

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheFilterShouldRead(FallbackValueDegFx100);
}

TEST(Filter_Fallback, ShouldSeedTheWrappedFilterWithValidValueAfterBeingFedGoodReadingCounterMaxNumberOfValidValuesWhenThermistorIsInvalid)
{
   GivenTheFilterHasBeenFedWithBadReadingCountingMaxNumberOfInvalidValuesAndIsReadingTheFallbackValue();

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(ValidDegFx100);
      TheFilterShouldRead(FallbackValueDegFx100);
   }

   AfterTheFilterIsFedWith(ValidDegFx100);
   TheWrappedFilterShouldHaveBeenSeededWith(ValidDegFx100);
}

TEST(Filter_Fallback, ShouldSwitchFromReadingFallbackValueToWrappedFilterValueBackToFallbackValue)
{
   GivenTheFilterSwitchedFromBadReadingCountingMaxNumberOfInvalidValuesToGoodReadingCountingMaxNumberOfValidValues();
   TheWrappedFilterCurrentFilterValueIs(ValidDegFx100);

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
      TheFilterShouldRead(ValidDegFx100);
   }

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheFilterShouldRead(FallbackValueDegFx100);
}

TEST(Filter_Fallback, ShouldSwitchFromReadingFallbackValueToWrappedFilterValueBackToFallbackValueAndBackAgain)
{
   GivenTheFilterSwitchedFromBadReadingCountingMaxNumberOfInvalidValuesToGoodReadingCountingMaxNumberOfValidValuesToBadReadingCountingMaxNumberOfInvalidValues();

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(ValidDegFx100);
      TheFilterShouldRead(FallbackValueDegFx100);
   }

   AfterTheFilterIsFedWith(ValidDegFx100);
   TheWrappedFilterCurrentFilterValueIs(ValidDegFx100);

   TheFilterShouldRead(ValidDegFx100);
   TheWrappedFilterShouldHaveBeenSeededWith(ValidDegFx100);
}

TEST(Filter_Fallback, ShouldReseedWrappedFilterWithValidValueAfterReachingGoodReadingCounterMaxNumberOfValidValuesWhenThermistorIsInvalidAndNotFeedIt)
{
   GivenTheFilterHasBeenSeededWithAValidValue();

   AfterTheFilterIsFedWith(300);
   TheWrappedFilterShouldHaveBeenFedWith(300);

   AfterTheFilterIsFedWith(320);
   TheWrappedFilterShouldHaveBeenFedWith(320);

   AfterTheFilterIsFedWith(340);
   TheWrappedFilterShouldHaveBeenFedWith(340);

   TheWrappedFilterCurrentFilterValueIs(340);

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
      TheFilterShouldRead(340);
   }

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheFilterShouldRead(FallbackValueDegFx100);

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(500);
      TheFilterShouldRead(FallbackValueDegFx100);
   }

   AfterTheFilterIsFedWith(500);

   TheWrappedFilterShouldHaveBeenSeededWith(500);
   TheLastValueTheWrappedFilterWasFedShouldHaveBeen(340);
}

TEST(Filter_Fallback, ShouldSetThermistorAsInvalidWhenOnlyAfterBeingFedBadReadingCounterMaxNumberOfInvalidValuesAfterBeingReseededWithAValidValue)
{
   GivenTheFilterHasBeenSeededWithAValidValue();
   TheValidThermistorErdShouldBe(true);

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
   }
   TheValidThermistorErdShouldBe(true);

   AfterTheFilterIsSeededWith(ValidDegFx100);
   TheWrappedFilterShouldHaveBeenSeededWith(ValidDegFx100);

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
      TheValidThermistorErdShouldBe(true);
   }
   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheValidThermistorErdShouldBe(false);
}

TEST(Filter_Fallback, ShouldSetThermistorAsValidWhenOnlyAfterBeingFedGoodReadingCounterMaxNumberOfValidValueAfterBeingReseededWithAnInvalidValue)
{
   GivenTheFilterHasBeenSeededWithAnInvalidValue();
   TheValidThermistorErdShouldBe(false);

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(ValidDegFx100);
   }
   TheValidThermistorErdShouldBe(false);

   AfterTheFilterIsSeededWith(InvalidValueDegFx100);

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(ValidDegFx100);
      TheValidThermistorErdShouldBe(false);
   }
   AfterTheFilterIsFedWith(ValidDegFx100);
   TheValidThermistorErdShouldBe(true);
}

TEST(Filter_Fallback, ShouldReseedWithValidValueAfterBeingSeededWithAnInvalidValueAndSetThermistorAsValid)
{
   GivenTheFilterHasBeenSeededWithAnInvalidValue();
   TheValidThermistorErdShouldBe(false);

   AfterTheFilterIsSeededWith(ValidDegFx100);
   TheWrappedFilterShouldHaveBeenSeededWith(ValidDegFx100);
   TheValidThermistorErdShouldBe(true);
}

TEST(Filter_Fallback, ShouldWriteFalseToThermistorIsValidErdWhenFilterReadsFallbackValue)
{
   GivenTheValidThermistorErdIs(true);
   GivenTheFilterHasBeenFedWithBadReadingCountingMaxNumberOfInvalidValuesAndIsReadingTheFallbackValue();

   TheFilterShouldRead(FallbackValueDegFx100);
   TheValidThermistorErdShouldBe(false);
}

TEST(Filter_Fallback, ShouldWriteTrueToThermistorIsValidErdWhenFilterReadsWrappedFilterValueAgain)
{
   GivenTheValidThermistorErdIs(true);
   GivenTheFilterSwitchedFromBadReadingCountingMaxNumberOfInvalidValuesToGoodReadingCountingMaxNumberOfValidValues();
   TheWrappedFilterCurrentFilterValueIs(ValidDegFx100);

   TheFilterShouldRead(ValidDegFx100);
   TheValidThermistorErdShouldBe(true);
}

TEST(Filter_Fallback, ShouldSwitchBetweenWritingTrueAndFalseToValidErd)
{
   GivenTheValidThermistorErdIs(true);
   GivenTheFilterHasBeenFedWithBadReadingCountingMaxNumberOfInvalidValuesAndIsReadingTheFallbackValue();

   TheFilterShouldRead(FallbackValueDegFx100);
   TheValidThermistorErdShouldBe(false);

   AfterFilterIsFedGoodReadingCountingMaxNumberOfValidValues();

   TheFilterShouldRead(ValidDegFx100);
   TheValidThermistorErdShouldBe(true);

   AfterFilterIsFedBadReadingCountingMaxNumberOfInvalidValues();

   TheFilterShouldRead(FallbackValueDegFx100);
   TheValidThermistorErdShouldBe(false);

   AfterFilterIsFedGoodReadingCountingMaxNumberOfValidValues();

   TheFilterShouldRead(ValidDegFx100);
   TheValidThermistorErdShouldBe(true);
}

TEST(Filter_Fallback, ShouldWaitUntilBadReadingCountingMaxNumberOfInvalidValuesHappenConsecutivelyBeforeWritingValidErdToFalse)
{
   GivenTheValidThermistorErdIs(true);
   GivenTheFilterHasBeenSeededWithAValidValue();

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
   }
   TheValidThermistorErdShouldBe(true);

   AfterTheFilterIsFedWith(2);
   TheValidThermistorErdShouldBe(true);

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheValidThermistorErdShouldBe(true);
}

TEST(Filter_Fallback, ShouldWaitUntilGoodReadingCountingMaxNumberOfValidValuesHappenConsecutivelyBeforeWritingValidErdToTrue)
{
   GivenTheValidThermistorErdIs(true);
   GivenTheFilterHasBeenFedWithBadReadingCountingMaxNumberOfInvalidValuesAndIsReadingTheFallbackValue();

   TheValidThermistorErdShouldBe(false);

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(2);
   }
   TheValidThermistorErdShouldBe(false);

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheValidThermistorErdShouldBe(false);

   AfterTheFilterIsFedWith(2);
   TheValidThermistorErdShouldBe(false);

   for(uint8_t i = 0; i < GoodReadingCounterMax; i++)
   {
      AfterTheFilterIsFedWith(2);
   }
   TheValidThermistorErdShouldBe(true);
}

TEST(Filter_Fallback, ShouldBeReadyWhenWrappedFilterIsReady)
{
   GivenTheFilterHasBeenInitialized();
   GivenTheWrappedFilterIsReady();

   TheFilterShouldBeReady();
}

TEST(Filter_Fallback, ShouldBeNotReadyWhenWrappedFilterIsNotReady)
{
   GivenTheFilterHasBeenInitialized();
   GivenTheWrappedFilterIsNotReady();

   TheFilterShouldBeNotReady();
}

TEST(Filter_Fallback, ShouldBeNotReadyWhenWrappedFilterIsReset)
{
   GivenTheFilterHasBeenInitialized();
   GivenTheWrappedFilterIsReady();

   WhenTheFilterIsReset();
   TheFilterShouldBeNotReady();
}
