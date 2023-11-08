/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Filter_Fallback.h"
#include "Constants_Binary.h"
#include "utils.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "DataModel_TestDouble.h"
#include "Filter_TestDouble.h"

enum
{
   ValidValueDegFx100 = 2,
   ValidValue1DegFx100 = 300,
   ValidValue2DegFx100 = 320,
   ValidValue3DegFx100 = 340,
   ValidValue4DegFx100 = 500,
   InvalidValueDegFx100 = 30000,
   FallbackValueDegFx100 = 200,
   GoodReadingCounterMax = 10,
   BadReadingCounterMax = 5,
   AnotherValidValueDegFx100 = 345
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
         BadReadingCounterMax);
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

   void TheFilterShouldRead(Filter_Fallback_Data_t expected)
   {
      Filter_Fallback_Data_t actual;
      Filter_Read(&instance.interface, &actual);
      CHECK_EQUAL(expected.filteredValue, actual.filteredValue);
      CHECK_EQUAL(expected.isValid, actual.isValid);
   }

   void WhenTheFilterIsRead()
   {
      Filter_Fallback_Data_t data;
      Filter_Read(&instance.interface, &data);
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

   void TheWrappedFilterShouldBeNotReady()
   {
      I_Input_t *readyInput = Filter_GetReadyInput(&wrappedFilter.interface);
      bool ready;
      Input_Read(readyInput, &ready);

      CHECK_FALSE(ready);
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
      AfterTheFilterIsSeededWith(ValidValueDegFx100);
      TheWrappedFilterShouldHaveBeenSeededWith(ValidValueDegFx100);
   }

   void GivenTheFilterHasBeenSeededWithAnInvalidValue()
   {
      GivenTheFilterHasBeenInitialized();
      GivenTheWrappedFilterIsNotReady();

      AfterTheFilterIsSeededWith(InvalidValueDegFx100);
      TheFilterShouldBeReady();
      TheWrappedFilterShouldBeNotReady();
   }

   void TheFilterShouldReadAValidValue()
   {
      Filter_Fallback_Data_t actual;
      Filter_Read(&instance.interface, &actual);
      CHECK_TRUE(actual.isValid);
   }

   void TheFilterShouldReadAnInvalidValue()
   {
      Filter_Fallback_Data_t actual;
      Filter_Read(&instance.interface, &actual);
      CHECK_FALSE(actual.isValid);
   }

   void GivenTheFilterHasBeenFedWithBadReadingCountingMaxNumberOfInvalidValuesAndIsReadingTheFallbackValue()
   {
      GivenTheFilterHasBeenSeededWithAValidValue();

      for(uint8_t i = 0; i < BadReadingCounterMax; i++)
      {
         AfterTheFilterIsFedWith(InvalidValueDegFx100);
      }

      TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });
   }

   void GivenTheFilterSwitchedFromBadReadingCountingMaxNumberOfInvalidValuesToGoodReadingCountingMaxNumberOfValidValues()
   {
      GivenTheFilterHasBeenFedWithBadReadingCountingMaxNumberOfInvalidValuesAndIsReadingTheFallbackValue();

      AfterFilterIsFedGoodReadingCountingMaxNumberOfValidValues();
   }

   void GivenTheFilterSwitchedFromBadReadingCountingMaxNumberOfInvalidValuesToGoodReadingCountingMaxNumberOfValidValuesToBadReadingCountingMaxNumberOfInvalidValues()
   {
      GivenTheFilterSwitchedFromBadReadingCountingMaxNumberOfInvalidValuesToGoodReadingCountingMaxNumberOfValidValues();
      TheWrappedFilterCurrentFilterValueIs(AnotherValidValueDegFx100);

      AfterFilterIsFedBadReadingCountingMaxNumberOfInvalidValues();
   }

   void AfterFilterIsFedGoodReadingCountingMaxNumberOfValidValues()
   {
      for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
      {
         AfterTheFilterIsFedWith(AnotherValidValueDegFx100);
         TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });
      }

      AfterTheFilterIsFedWith(AnotherValidValueDegFx100);
      TheWrappedFilterCurrentFilterValueIs(AnotherValidValueDegFx100);
   }

   void AfterFilterIsFedBadReadingCountingMaxNumberOfInvalidValues()
   {
      for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
      {
         AfterTheFilterIsFedWith(InvalidValueDegFx100);
         TheFilterShouldRead({ .filteredValue = AnotherValidValueDegFx100, .isValid = true });
      }

      AfterTheFilterIsFedWith(InvalidValueDegFx100);
   }

   void GivenTheFilterHasBeenReseededAndLastValueFedToTheWrappedFilterWasValidValue3DegFx100()
   {
      GivenTheFilterHasBeenSeededWithAValidValue();

      AfterTheFilterIsFedWith(ValidValue1DegFx100);
      TheWrappedFilterShouldHaveBeenFedWith(ValidValue1DegFx100);

      AfterTheFilterIsFedWith(ValidValue2DegFx100);
      TheWrappedFilterShouldHaveBeenFedWith(ValidValue2DegFx100);

      AfterTheFilterIsFedWith(ValidValue3DegFx100);
      TheWrappedFilterShouldHaveBeenFedWith(ValidValue3DegFx100);

      TheWrappedFilterCurrentFilterValueIs(ValidValue3DegFx100);

      for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
      {
         AfterTheFilterIsFedWith(InvalidValueDegFx100);
         TheFilterShouldRead({ .filteredValue = ValidValue3DegFx100, .isValid = true });
      }

      AfterTheFilterIsFedWith(InvalidValueDegFx100);
      TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });

      for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
      {
         AfterTheFilterIsFedWith(ValidValue4DegFx100);
         TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });
      }

      AfterTheFilterIsFedWith(ValidValue4DegFx100);

      TheWrappedFilterShouldHaveBeenSeededWith(ValidValue4DegFx100);
      TheLastValueTheWrappedFilterWasFedShouldHaveBeen(ValidValue3DegFx100);
   }
};

TEST(Filter_Fallback, ShouldSeedTheWrappedFilterWithSameValueWhenSeededIfTheValueIsValid)
{
   GivenTheFilterHasBeenInitialized();
   AfterTheFilterIsSeededWith(ValidValueDegFx100);
   TheWrappedFilterShouldHaveBeenSeededWith(ValidValueDegFx100);
}

TEST(Filter_Fallback, ShouldHaveWrappedFilterAsNotReadyBecauseItDidNotSeedTheWrappedFilterWhenFallbackFilterIsSeededWithAnInvalidValue)
{
   GivenTheFilterHasBeenInitialized();

   AfterTheFilterIsSeededWith(InvalidValueDegFx100);
   TheWrappedFilterShouldBeNotReady();
}

TEST(Filter_Fallback, ShouldUassertWhenReadingTheFallbackValueAndFallbackFilterIsNotReady)
{
   GivenTheFilterHasBeenInitialized();
   ShouldFailAssertion(WhenTheFilterIsRead());
}

TEST(Filter_Fallback, ShouldFeedTheWrappedFilterWithSameValueIfItIsValid)
{
   GivenTheFilterHasBeenSeededWithAValidValue();

   AfterTheFilterIsFedWith(ValidValueDegFx100);
   TheWrappedFilterShouldHaveBeenFedWith(ValidValueDegFx100);
}

TEST(Filter_Fallback, ShouldReadTheWrappedFilterValueWhenSeededWithAValidValue)
{
   GivenTheFilterHasBeenSeededWithAValidValue();
   TheWrappedFilterCurrentFilterValueIs(ValidValueDegFx100);

   TheFilterShouldRead({ .filteredValue = ValidValueDegFx100, .isValid = true });
}

TEST(Filter_Fallback, ShouldReadTheFallbackValueWhenSeededWithAnInvalidValue)
{
   GivenTheFilterHasBeenSeededWithAnInvalidValue();

   TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });
}

TEST(Filter_Fallback, ShouldReadTheValidValueAndSwitchThermistorToValidAfterGoodReadingCounterMaxNumberOfValidValuesWhenThermistorIsInvalidOnInit)
{
   GivenTheFilterHasBeenSeededWithAnInvalidValue();
   TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(AnotherValidValueDegFx100);
   }

   AfterTheFilterIsFedWith(AnotherValidValueDegFx100);
   TheWrappedFilterShouldHaveBeenSeededWith(AnotherValidValueDegFx100);
   TheWrappedFilterCurrentFilterValueIs(AnotherValidValueDegFx100);
   TheFilterShouldRead({ .filteredValue = AnotherValidValueDegFx100, .isValid = true });
}

TEST(Filter_Fallback, ShouldReadTheInvalidValueAndSwitchThermistorToInvalidAfterBadReadingCounterIsMaxNumberOfValidValuesWhenThermistorIsValidOnInit)
{
   GivenTheFilterHasBeenSeededWithAValidValue();
   TheWrappedFilterCurrentFilterValueIs(AnotherValidValueDegFx100);

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
      TheFilterShouldRead({ .filteredValue = AnotherValidValueDegFx100, .isValid = true });
   }

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });
}

TEST(Filter_Fallback, ShouldSeedTheWrappedFilterWithValidValueAfterBeingFedGoodReadingCounterMaxNumberOfValidValuesWhenThermistorIsInvalid)
{
   GivenTheFilterHasBeenFedWithBadReadingCountingMaxNumberOfInvalidValuesAndIsReadingTheFallbackValue();

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(AnotherValidValueDegFx100);
      TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });
   }

   AfterTheFilterIsFedWith(AnotherValidValueDegFx100);
   TheWrappedFilterShouldHaveBeenSeededWith(AnotherValidValueDegFx100);
}

TEST(Filter_Fallback, ShouldSwitchFromReadingFallbackValueToWrappedFilterValueBackToFallbackValue)
{
   GivenTheFilterSwitchedFromBadReadingCountingMaxNumberOfInvalidValuesToGoodReadingCountingMaxNumberOfValidValues();
   TheWrappedFilterCurrentFilterValueIs(AnotherValidValueDegFx100);

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
      TheFilterShouldRead({ .filteredValue = AnotherValidValueDegFx100, .isValid = true });
   }

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });
}

TEST(Filter_Fallback, ShouldSwitchFromReadingFallbackValueToWrappedFilterValueBackToFallbackValueAndBackAgain)
{
   GivenTheFilterSwitchedFromBadReadingCountingMaxNumberOfInvalidValuesToGoodReadingCountingMaxNumberOfValidValuesToBadReadingCountingMaxNumberOfInvalidValues();

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(AnotherValidValueDegFx100);
      TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });
   }

   AfterTheFilterIsFedWith(AnotherValidValueDegFx100);
   TheWrappedFilterCurrentFilterValueIs(AnotherValidValueDegFx100);

   TheFilterShouldRead({ .filteredValue = AnotherValidValueDegFx100, .isValid = true });
   TheWrappedFilterShouldHaveBeenSeededWith(AnotherValidValueDegFx100);
}

TEST(Filter_Fallback, ShouldReseedWrappedFilterWithValidValueAfterReachingGoodReadingCounterMaxNumberOfValidValuesWhenThermistorIsInvalidAndNotFeedIt)
{
   GivenTheFilterHasBeenSeededWithAValidValue();

   AfterTheFilterIsFedWith(ValidValue1DegFx100);
   TheWrappedFilterShouldHaveBeenFedWith(ValidValue1DegFx100);

   AfterTheFilterIsFedWith(ValidValue2DegFx100);
   TheWrappedFilterShouldHaveBeenFedWith(ValidValue2DegFx100);

   AfterTheFilterIsFedWith(ValidValue3DegFx100);
   TheWrappedFilterShouldHaveBeenFedWith(ValidValue3DegFx100);

   TheWrappedFilterCurrentFilterValueIs(ValidValue3DegFx100);

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
      TheFilterShouldRead({ .filteredValue = ValidValue3DegFx100, .isValid = true });
   }

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(ValidValue4DegFx100);
      TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });
   }

   AfterTheFilterIsFedWith(ValidValue4DegFx100);

   TheWrappedFilterShouldHaveBeenSeededWith(ValidValue4DegFx100);
   TheLastValueTheWrappedFilterWasFedShouldHaveBeen(ValidValue3DegFx100);
}

TEST(Filter_Fallback, ShouldFeedValidValueAfterReseedingWrappedFilter)
{
   GivenTheFilterHasBeenReseededAndLastValueFedToTheWrappedFilterWasValidValue3DegFx100();

   AfterTheFilterIsFedWith(ValidValue4DegFx100);
   TheWrappedFilterShouldHaveBeenFedWith(ValidValue4DegFx100);
}

TEST(Filter_Fallback, ShouldNotFeedInvalidValueAfterReseedingWrappedFilter)
{
   GivenTheFilterHasBeenReseededAndLastValueFedToTheWrappedFilterWasValidValue3DegFx100();

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheWrappedFilterShouldHaveBeenFedWith(ValidValue3DegFx100);
}

TEST(Filter_Fallback, ShouldReadAnInvalidValueWhenOnlyAfterBeingFedBadReadingCounterMaxNumberOfInvalidValuesAfterBeingReseededWithAValidValue)
{
   GivenTheFilterHasBeenSeededWithAValidValue();
   TheFilterShouldReadAValidValue();

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
   }
   TheFilterShouldReadAValidValue();

   AfterTheFilterIsSeededWith(AnotherValidValueDegFx100);
   TheWrappedFilterShouldHaveBeenSeededWith(AnotherValidValueDegFx100);

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
      TheFilterShouldReadAValidValue();
   }

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheFilterShouldReadAnInvalidValue();
}

TEST(Filter_Fallback, ShouldReadAValidValueWhenOnlyAfterBeingFedGoodReadingCounterMaxNumberOfValidValuesAfterBeingReseededWithAnInvalidValue)
{
   GivenTheFilterHasBeenSeededWithAnInvalidValue();
   TheFilterShouldReadAnInvalidValue();

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(AnotherValidValueDegFx100);
   }
   TheFilterShouldReadAnInvalidValue();

   AfterTheFilterIsSeededWith(InvalidValueDegFx100);
   TheFilterShouldReadAnInvalidValue();

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(AnotherValidValueDegFx100);
      TheFilterShouldReadAnInvalidValue();
   }
   AfterTheFilterIsFedWith(AnotherValidValueDegFx100);
   TheFilterShouldReadAValidValue();
}

TEST(Filter_Fallback, ShouldReseedWithValidValueAfterBeingSeededWithAnInvalidValueAndReadAValidValue)
{
   GivenTheFilterHasBeenSeededWithAnInvalidValue();
   TheFilterShouldReadAnInvalidValue();

   AfterTheFilterIsSeededWith(AnotherValidValueDegFx100);
   TheWrappedFilterShouldHaveBeenSeededWith(AnotherValidValueDegFx100);
   TheFilterShouldReadAValidValue();
}

TEST(Filter_Fallback, ShouldReadAnInvalidValueWhenFilterReadsFallbackValue)
{
   GivenTheFilterHasBeenFedWithBadReadingCountingMaxNumberOfInvalidValuesAndIsReadingTheFallbackValue();

   TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });
   TheFilterShouldReadAnInvalidValue();
}

TEST(Filter_Fallback, ShouldReadAValidValueWhenFilterReadsWrappedFilterValueAgain)
{
   GivenTheFilterSwitchedFromBadReadingCountingMaxNumberOfInvalidValuesToGoodReadingCountingMaxNumberOfValidValues();
   TheWrappedFilterCurrentFilterValueIs(AnotherValidValueDegFx100);

   TheFilterShouldRead({ .filteredValue = AnotherValidValueDegFx100, .isValid = true });
}

TEST(Filter_Fallback, ShouldSwitchBetweenReadingAValidValueAndReadingAnInvalidValue)
{
   GivenTheFilterHasBeenFedWithBadReadingCountingMaxNumberOfInvalidValuesAndIsReadingTheFallbackValue();
   TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });

   AfterFilterIsFedGoodReadingCountingMaxNumberOfValidValues();
   TheFilterShouldRead({ .filteredValue = AnotherValidValueDegFx100, .isValid = true });

   AfterFilterIsFedBadReadingCountingMaxNumberOfInvalidValues();
   TheFilterShouldRead({ .filteredValue = FallbackValueDegFx100, .isValid = false });

   AfterFilterIsFedGoodReadingCountingMaxNumberOfValidValues();
   TheFilterShouldRead({ .filteredValue = AnotherValidValueDegFx100, .isValid = true });
}

TEST(Filter_Fallback, ShouldWaitUntilBadReadingCountingMaxNumberOfInvalidValuesHappenConsecutivelyBeforeValueReadIsInvalid)
{
   GivenTheFilterHasBeenSeededWithAValidValue();

   for(uint8_t i = 0; i < BadReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(InvalidValueDegFx100);
   }
   TheFilterShouldReadAValidValue();

   AfterTheFilterIsFedWith(2);
   TheFilterShouldReadAValidValue();

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheFilterShouldReadAValidValue();
}

TEST(Filter_Fallback, ShouldWaitUntilGoodReadingCountingMaxNumberOfValidValuesHappenConsecutivelyBeforeValueReadIsValid)
{
   GivenTheFilterHasBeenFedWithBadReadingCountingMaxNumberOfInvalidValuesAndIsReadingTheFallbackValue();
   TheFilterShouldReadAnInvalidValue();

   for(uint8_t i = 0; i < GoodReadingCounterMax - 1; i++)
   {
      AfterTheFilterIsFedWith(2);
   }
   TheFilterShouldReadAnInvalidValue();

   AfterTheFilterIsFedWith(InvalidValueDegFx100);
   TheFilterShouldReadAnInvalidValue();

   AfterTheFilterIsFedWith(2);
   TheFilterShouldReadAnInvalidValue();

   for(uint8_t i = 0; i < GoodReadingCounterMax; i++)
   {
      AfterTheFilterIsFedWith(2);
   }
   TheFilterShouldReadAValidValue();
}

TEST(Filter_Fallback, ShouldBeReadyWhenFallbackFilterIsSeededWithAValidValue)
{
   GivenTheFilterHasBeenInitialized();
   AfterTheFilterIsSeededWith(ValidValueDegFx100);
   TheFilterShouldBeReady();
}

TEST(Filter_Fallback, ShouldBeReadyWhenFallbackFilterIsSeededWithAnInvalidValue)
{
   GivenTheFilterHasBeenInitialized();
   AfterTheFilterIsSeededWith(InvalidValueDegFx100);
   TheFilterShouldBeReady();
}

TEST(Filter_Fallback, ShouldBeNotReadyWhenFallbackFilterIsReset)
{
   GivenTheFilterHasBeenInitialized();
   AfterTheFilterIsSeededWith(ValidValueDegFx100);
   TheFilterShouldBeReady();

   WhenTheFilterIsReset();
   TheFilterShouldBeNotReady();
}

TEST(Filter_Fallback, ShouldHaveWrappedFilterAsNotReadyWhenFallbackFilterIsReset)
{
   GivenTheWrappedFilterIsReady();
   GivenTheFilterHasBeenInitialized();

   WhenTheFilterIsReset();
   TheWrappedFilterShouldBeNotReady();
}

TEST(Filter_Fallback, ShouldKeepFilterReadyWhenSeededWithAnInvalidValueWhileAlreadyReady)
{
   GivenTheWrappedFilterIsReady();
   GivenTheFilterHasBeenInitialized();

   AfterTheFilterIsSeededWith(ValidValueDegFx100);
   TheFilterShouldBeReady();

   AfterTheFilterIsSeededWith(InvalidValueDegFx100);
   TheFilterShouldBeReady();
}

TEST(Filter_Fallback, ShouldHaveWrappedFilterAsNotReadyWhenSeededWithAnInvalidValueWhileAlreadyBeingReady)
{
   GivenTheWrappedFilterIsReady();
   GivenTheFilterHasBeenInitialized();

   AfterTheFilterIsSeededWith(ValidValueDegFx100);
   TheFilterShouldBeReady();

   AfterTheFilterIsSeededWith(InvalidValueDegFx100);
   TheWrappedFilterShouldBeNotReady();
}

TEST(Filter_Fallback, ShouldReadAnInvalidValueWhenSeededWithAnInvalidValueWhileAlreadyBeingReady)
{
   GivenTheWrappedFilterIsReady();
   GivenTheFilterHasBeenInitialized();

   AfterTheFilterIsSeededWith(ValidValueDegFx100);
   TheFilterShouldBeReady();

   AfterTheFilterIsSeededWith(InvalidValueDegFx100);
   TheFilterShouldReadAnInvalidValue();
}
