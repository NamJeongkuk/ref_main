/*!
 * @file
 * @brief Test file for implementation of Unsigned Signed lookup table implementation of I_Mapper.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

extern "C"
{
#include "Mapper_UnsignedSignedLookupTable.h"
#include "utils.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"

enum
{
   LotsOfMappings = 1000
};

static const Mapper_UnsignedSignedLookupTableMapping_t mappingsWithNoInterpolation[] = {
   { 1, 0 },
   { 349, 0 },
   { 350, 1 },
   { 699, 1 },
   { 700, 2 },
   { 1024, 2 }
};

static const Mapper_UnsignedSignedLookupTableConfiguration_t configurationWithNoInterpolation = {
   mappingsWithNoInterpolation,
   NUM_ELEMENTS(mappingsWithNoInterpolation)
};

static const Mapper_UnsignedSignedLookupTableMapping_t mappingsWithInterpolation[] = {
   { 0, -32 },
   { 127, 200 },
   { 255, 500 },
   { 511, 1000 },
   { 1023, 2000 }
};

static const Mapper_UnsignedSignedLookupTableConfiguration_t configurationWithInterpolation = {
   mappingsWithInterpolation,
   NUM_ELEMENTS(mappingsWithInterpolation)
};

static const Mapper_UnsignedSignedLookupTableMapping_t largeValueLookup[] = {
   { 0, -5 },
   { 127, INT16_MAX },
};

static const Mapper_UnsignedSignedLookupTableConfiguration_t configurationWithLargeValues = {
   largeValueLookup,
   NUM_ELEMENTS(largeValueLookup)
};

static Mapper_UnsignedSignedLookupTableMapping_t lookupWithLotsOfMappings[LotsOfMappings];

static const Mapper_UnsignedSignedLookupTableConfiguration_t configurationWithLotsOfMappings = {
   lookupWithLotsOfMappings,
   NUM_ELEMENTS(lookupWithLotsOfMappings)
};

TEST_GROUP(Mapper_UnsignedSignedLookupTable)
{
   Mapper_UnsignedSignedLookupTable_t instance;

   void GivenThatTheMapperHasBeenInitializedWithNoInterpolation()
   {
      Mapper_UnsignedSignedLookupTable_Init(&instance, &configurationWithNoInterpolation);
   }

   void GivenThatTheMapperHasBeenInitializedWithInterpolation()
   {
      Mapper_UnsignedSignedLookupTable_Init(&instance, &configurationWithInterpolation);
   }

   void GivenThatTheMapperHasBeenInitializedWithLargeValuesToInterpolate()
   {
      Mapper_UnsignedSignedLookupTable_Init(&instance, &configurationWithLargeValues);
   }

   void GivenThatTheMapperHasBeenInitializedWithLotsOfMappedValues()
   {
      uint16_t i;
      for(i = 0; i < LotsOfMappings; i++)
      {
         lookupWithLotsOfMappings[i].x = i * 2;
         lookupWithLotsOfMappings[i].y = i * 2;
      }

      Mapper_UnsignedSignedLookupTable_Init(&instance, &configurationWithLotsOfMappings);
   }

   void MappedValueShouldBe(int64_t x, int64_t y)
   {
      CHECK_EQUAL(y, Mapper_GetMappedValue(&instance.interface, x));
   }

   void MapIndexForValue(int64_t value, uint16_t index)
   {
      CHECK_EQUAL(index, Mapper_GetValueIndex(&instance.interface, value));
   }
};

TEST(Mapper_UnsignedSignedLookupTable, ShouldAllowLowEndOfRangeToBeMapped)
{
   GivenThatTheMapperHasBeenInitializedWithNoInterpolation();

   MappedValueShouldBe(50, 0);
   MappedValueShouldBe(300, 0);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldAllowMiddleOfRangeToBeMapped)
{
   GivenThatTheMapperHasBeenInitializedWithNoInterpolation();

   MappedValueShouldBe(350, 1);
   MappedValueShouldBe(699, 1);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldAllowHighEndOfRangeToBeMapped)
{
   GivenThatTheMapperHasBeenInitializedWithNoInterpolation();

   MappedValueShouldBe(700, 2);
   MappedValueShouldBe(1024, 2);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldAllowLowerBoundToBeMapped)
{
   GivenThatTheMapperHasBeenInitializedWithNoInterpolation();

   MappedValueShouldBe(0, 0);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldAllowUpperBoundToBeMapped)
{
   GivenThatTheMapperHasBeenInitializedWithNoInterpolation();

   MappedValueShouldBe(1025, 2);
   MappedValueShouldBe(2000, 2);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldInterpolateInFirstSegment)
{
   GivenThatTheMapperHasBeenInitializedWithInterpolation();
   MappedValueShouldBe(64, (-32 + 200) / 2);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldInterpolateInMiddleSegment)
{
   GivenThatTheMapperHasBeenInitializedWithInterpolation();
   MappedValueShouldBe(127 + 64, (200 + 500) / 2);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldInterpolateInLastSegment)
{
   GivenThatTheMapperHasBeenInitializedWithInterpolation();
   MappedValueShouldBe(255 + 128, (500 + 1000) / 2);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldSafelyInterpolateLargeValues)
{
   GivenThatTheMapperHasBeenInitializedWithLargeValuesToInterpolate();
   MappedValueShouldBe(64, 16510);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldFindIndexesAtHighEndOfRange)
{
   GivenThatTheMapperHasBeenInitializedWithLargeValuesToInterpolate();
   MapIndexForValue(127, 1);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldFindIndexesAtLowEndOfRange)
{
   GivenThatTheMapperHasBeenInitializedWithLargeValuesToInterpolate();
   MapIndexForValue(0, 0);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldFindFirstMatchingIndexes)
{
   GivenThatTheMapperHasBeenInitializedWithNoInterpolation();
   MapIndexForValue(700, 4);
   MapIndexForValue(699, 3);
   MapIndexForValue(698, 2);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldReturnDefaultValueOfZero)
{
   GivenThatTheMapperHasBeenInitializedWithNoInterpolation();
   MapIndexForValue(0, 0);
}

TEST(Mapper_UnsignedSignedLookupTable, ShouldFindMatchingValueInLargeMappingTable)
{
   GivenThatTheMapperHasBeenInitializedWithLotsOfMappedValues();
   MappedValueShouldBe(LotsOfMappings - 1, LotsOfMappings - 1);
}
