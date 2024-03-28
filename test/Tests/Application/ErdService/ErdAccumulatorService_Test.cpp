/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ErdAccumulatorService.h"
#include <stdint.h>
}

#include "CppUTest/TestHarness.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"

constexpr uint64_t SomeU64Value1 = 9223372036854775808ULL;
constexpr uint64_t SomeU64Value2 = 64116860184273880;
constexpr uint64_t SomeU64Value3 = 46116860227223553;

enum
{
   SomeI64Value1 = 2251799813685248,
   SomeI64Value2 = 2252899325313024,
   SomeI64Value3 = -1133871366144,

   SomeU32Value1 = 2147483648,
   SomeU32Value2 = 83609280,
   SomeU32Value3 = 203810210,

   SomeI32Value1 = 1048576,
   SomeI32Value2 = 8360928,
   SomeI32Value3 = -20381021,

   SomeU16Value1 = 32768,
   SomeU16Value2 = 8192,
   SomeU16Value3 = 4096,

   SomeI16Value1 = 16384,
   SomeI16Value2 = 8192,
   SomeI16Value3 = 4096,

   SomeU8Value1 = 128,
   SomeU8Value2 = 32,
   SomeU8Value3 = 8,

   SomeI8Value1 = 64,
   SomeI8Value2 = 32,
   SomeI8Value3 = -8,
};

enum
{
   Erd_1,
   Erd_2,
   Erd_3,
   Erd_Cumulative
};

typedef struct
{
   uint8_t data1;
   uint8_t data2;
   uint8_t data3;
} MadeUpType_t;

static const DataModel_TestDoubleConfigurationEntry_t erdTableU64[] = {
   { Erd_1, sizeof(uint64_t) },
   { Erd_2, sizeof(uint64_t) },
   { Erd_3, sizeof(uint64_t) },
   { Erd_Cumulative, sizeof(uint64_t) }
};

static const DataModel_TestDoubleConfigurationEntry_t erdTableI64[] = {
   { Erd_1, sizeof(int64_t) },
   { Erd_2, sizeof(int64_t) },
   { Erd_3, sizeof(int64_t) },
   { Erd_Cumulative, sizeof(int64_t) }
};

static const DataModel_TestDoubleConfigurationEntry_t erdTableU32[] = {
   { Erd_1, sizeof(uint32_t) },
   { Erd_2, sizeof(uint32_t) },
   { Erd_3, sizeof(uint32_t) },
   { Erd_Cumulative, sizeof(uint32_t) }
};

static const DataModel_TestDoubleConfigurationEntry_t erdTableI32[] = {
   { Erd_1, sizeof(int32_t) },
   { Erd_2, sizeof(int32_t) },
   { Erd_3, sizeof(int32_t) },
   { Erd_Cumulative, sizeof(int32_t) }
};

static const DataModel_TestDoubleConfigurationEntry_t erdTableU16[] = {
   { Erd_1, sizeof(uint16_t) },
   { Erd_2, sizeof(uint16_t) },
   { Erd_3, sizeof(uint16_t) },
   { Erd_Cumulative, sizeof(uint16_t) }
};

static const DataModel_TestDoubleConfigurationEntry_t erdTableI16[] = {
   { Erd_1, sizeof(int16_t) },
   { Erd_2, sizeof(int16_t) },
   { Erd_3, sizeof(int16_t) },
   { Erd_Cumulative, sizeof(int16_t) }
};

static const DataModel_TestDoubleConfigurationEntry_t erdTableU8[] = {
   { Erd_1, sizeof(uint8_t) },
   { Erd_2, sizeof(uint8_t) },
   { Erd_3, sizeof(uint8_t) },
   { Erd_Cumulative, sizeof(uint8_t) }
};

static const DataModel_TestDoubleConfigurationEntry_t erdTableI8[] = {
   { Erd_1, sizeof(int8_t) },
   { Erd_2, sizeof(int8_t) },
   { Erd_3, sizeof(int8_t) },
   { Erd_Cumulative, sizeof(int8_t) }
};

static const DataModel_TestDoubleConfigurationEntry_t erdTableWithDifferentSizeInputErd[] = {
   { Erd_1, sizeof(uint64_t) },
   { Erd_2, sizeof(uint32_t) },
   { Erd_3, sizeof(uint64_t) },
   { Erd_Cumulative, sizeof(uint64_t) }
};

static const DataModel_TestDoubleConfigurationEntry_t erdTableWithDifferentCumulativeSize[] = {
   { Erd_1, sizeof(uint64_t) },
   { Erd_2, sizeof(uint64_t) },
   { Erd_3, sizeof(uint64_t) },
   { Erd_Cumulative, sizeof(uint32_t) }
};

static const DataModel_TestDoubleConfigurationEntry_t erdTableWithUnsupportedTypes[] = {
   { Erd_1, sizeof(MadeUpType_t) },
   { Erd_2, sizeof(MadeUpType_t) },
   { Erd_3, sizeof(MadeUpType_t) },
   { Erd_Cumulative, sizeof(MadeUpType_t) }
};

static const Erd_t inputErds[] = {
   Erd_1,
   Erd_2,
   Erd_3
};

static const ErdList_t inputErdList = {
   .erds = inputErds,
   .numberOfErds = NUM_ELEMENTS(inputErds)
};

static const ErdAccumulatorServiceConfig_t erdAccumulatorServiceConfig = {
   .inputErdList = inputErdList,
   .cumulativeValueErd = Erd_Cumulative,
};

TEST_GROUP(ErdAccumulatorService)
{
   ErdAccumulatorService_t instance;
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void InitializeDataModelWith(const DataModel_TestDoubleConfigurationEntry_t *erdTable, uint8_t sizeOfErdTable)
   {
      DataModel_TestDouble_Init(
         &dataModelTestDouble,
         erdTable,
         sizeOfErdTable);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenInitialization(uint8_t integerWidth, bool integerIsSigned)
   {
      ErdAccumulatorService_Init(
         &instance,
         dataModel,
         &erdAccumulatorServiceConfig,
         integerWidth,
         integerIsSigned);
   }

   void WhenU64InputChangesTo(Erd_t erd, uint64_t input)
   {
      DataModel_Write(dataModel, erd, &input);
   }

   void CumulativeU64ErdShouldBe(uint64_t expected)
   {
      uint64_t actual;
      DataModel_Read(dataModel, Erd_Cumulative, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenI64InputChangesTo(Erd_t erd, int64_t input)
   {
      DataModel_Write(dataModel, erd, &input);
   }

   void CumulativeI64ErdShouldBe(int64_t expected)
   {
      int64_t actual;
      DataModel_Read(dataModel, Erd_Cumulative, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenU32InputChangesTo(Erd_t erd, uint32_t input)
   {
      DataModel_Write(dataModel, erd, &input);
   }

   void CumulativeU32ErdShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(dataModel, Erd_Cumulative, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenI32InputChangesTo(Erd_t erd, int32_t input)
   {
      DataModel_Write(dataModel, erd, &input);
   }

   void CumulativeI32ErdShouldBe(int32_t expected)
   {
      int32_t actual;
      DataModel_Read(dataModel, Erd_Cumulative, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenU16InputChangesTo(Erd_t erd, uint16_t input)
   {
      DataModel_Write(dataModel, erd, &input);
   }

   void CumulativeU16ErdShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_Cumulative, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenI16InputChangesTo(Erd_t erd, int16_t input)
   {
      DataModel_Write(dataModel, erd, &input);
   }

   void CumulativeI16ErdShouldBe(int16_t expected)
   {
      int16_t actual;
      DataModel_Read(dataModel, Erd_Cumulative, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenU8InputChangesTo(Erd_t erd, uint8_t input)
   {
      DataModel_Write(dataModel, erd, &input);
   }

   void CumulativeU8ErdShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_Cumulative, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenI8InputChangesTo(Erd_t erd, int8_t input)
   {
      DataModel_Write(dataModel, erd, &input);
   }

   void CumulativeI8ErdShouldBe(int8_t expected)
   {
      int8_t actual;
      DataModel_Read(dataModel, Erd_Cumulative, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(ErdAccumulatorService, ShouldWriteTheSumToCumulativeErdWhenAnyOfTheInputErdsChangeForU64Config)
{
   InitializeDataModelWith(erdTableU64, NUM_ELEMENTS(erdTableU64));
   GivenInitialization(sizeof(uint64_t), IS_SIGNED(uint64_t));

   WhenU64InputChangesTo(Erd_1, SomeU64Value1);
   CumulativeU64ErdShouldBe(SomeU64Value1);

   WhenU64InputChangesTo(Erd_2, SomeU64Value2);
   CumulativeU64ErdShouldBe(SomeU64Value1 + SomeU64Value2);

   WhenU64InputChangesTo(Erd_3, SomeU64Value3);
   CumulativeU64ErdShouldBe(SomeU64Value1 + SomeU64Value2 + SomeU64Value3);
}

TEST(ErdAccumulatorService, ShouldWriteU64MaxToTheCumulativeErdWhenAccumulationExceedsU64Max)
{
   InitializeDataModelWith(erdTableU64, NUM_ELEMENTS(erdTableU64));
   GivenInitialization(sizeof(uint64_t), IS_SIGNED(uint64_t));

   WhenU64InputChangesTo(Erd_1, UINT64_MAX - 1);
   CumulativeU64ErdShouldBe(UINT64_MAX - 1);

   WhenU64InputChangesTo(Erd_2, 1);
   CumulativeU64ErdShouldBe(UINT64_MAX);

   WhenU64InputChangesTo(Erd_3, SomeU64Value1);
   CumulativeU64ErdShouldBe(UINT64_MAX);
}

TEST(ErdAccumulatorService, ShouldWriteTheSumToCumulativeErdWhenAnyOfTheInputErdsChangeForI64Config)
{
   InitializeDataModelWith(erdTableI64, NUM_ELEMENTS(erdTableI64));
   GivenInitialization(sizeof(int64_t), IS_SIGNED(int64_t));

   WhenI64InputChangesTo(Erd_1, SomeI64Value1);
   CumulativeI64ErdShouldBe(SomeI64Value1);

   WhenI64InputChangesTo(Erd_2, SomeI64Value2);
   CumulativeI64ErdShouldBe(SomeI64Value1 + SomeI64Value2);

   WhenI64InputChangesTo(Erd_3, SomeI64Value3);
   CumulativeI64ErdShouldBe(SomeI64Value1 + SomeI64Value2 + SomeI64Value3);
}

TEST(ErdAccumulatorService, ShouldWriteI64MaxToTheCumulativeErdWhenAccumulationExceedsI64Max)
{
   InitializeDataModelWith(erdTableI64, NUM_ELEMENTS(erdTableI64));
   GivenInitialization(sizeof(int64_t), IS_SIGNED(int64_t));

   WhenI64InputChangesTo(Erd_1, INT64_MAX - 1);
   CumulativeI64ErdShouldBe(INT64_MAX - 1);

   WhenI64InputChangesTo(Erd_2, 1);
   CumulativeI64ErdShouldBe(INT64_MAX);

   WhenI64InputChangesTo(Erd_3, SomeI64Value1);
   CumulativeI64ErdShouldBe(INT64_MAX);
}

TEST(ErdAccumulatorService, ShouldWriteTheSumToCumulativeErdWhenAnyOfTheInputErdsChangeForU32Config)
{
   InitializeDataModelWith(erdTableU32, NUM_ELEMENTS(erdTableU32));
   GivenInitialization(sizeof(uint32_t), IS_SIGNED(uint32_t));

   WhenU32InputChangesTo(Erd_1, SomeU32Value1);
   CumulativeU32ErdShouldBe(SomeU32Value1);

   WhenU32InputChangesTo(Erd_2, SomeU32Value2);
   CumulativeU32ErdShouldBe(SomeU32Value1 + SomeU32Value2);

   WhenU32InputChangesTo(Erd_3, SomeU32Value3);
   CumulativeU32ErdShouldBe(SomeU32Value1 + SomeU32Value2 + SomeU32Value3);
}

TEST(ErdAccumulatorService, ShouldWriteU32MaxToTheCumulativeErdWhenAccumulationExceedsU32Max)
{
   InitializeDataModelWith(erdTableU32, NUM_ELEMENTS(erdTableU32));
   GivenInitialization(sizeof(uint32_t), IS_SIGNED(uint32_t));

   WhenU32InputChangesTo(Erd_1, UINT32_MAX - 1);
   CumulativeU32ErdShouldBe(UINT32_MAX - 1);

   WhenU32InputChangesTo(Erd_2, 1);
   CumulativeU32ErdShouldBe(UINT32_MAX);

   WhenU32InputChangesTo(Erd_3, SomeU32Value1);
   CumulativeU32ErdShouldBe(UINT32_MAX);
}

TEST(ErdAccumulatorService, ShouldWriteTheSumToCumulativeErdWhenAnyOfTheInputErdsChangeForI32Config)
{
   InitializeDataModelWith(erdTableI32, NUM_ELEMENTS(erdTableI32));
   GivenInitialization(sizeof(int32_t), IS_SIGNED(int32_t));

   WhenI32InputChangesTo(Erd_1, SomeI32Value1);
   CumulativeI32ErdShouldBe(SomeI32Value1);

   WhenI32InputChangesTo(Erd_2, SomeI32Value2);
   CumulativeI32ErdShouldBe(SomeI32Value1 + SomeI32Value2);

   WhenI32InputChangesTo(Erd_3, SomeI32Value3);
   CumulativeI32ErdShouldBe(SomeI32Value1 + SomeI32Value2 + SomeI32Value3);
}

TEST(ErdAccumulatorService, ShouldWriteI32MaxToTheCumulativeErdWhenAccumulationExceedsI32Max)
{
   InitializeDataModelWith(erdTableI32, NUM_ELEMENTS(erdTableI32));
   GivenInitialization(sizeof(int32_t), IS_SIGNED(int32_t));

   WhenI32InputChangesTo(Erd_1, INT32_MAX - 1);
   CumulativeI32ErdShouldBe(INT32_MAX - 1);

   WhenI32InputChangesTo(Erd_2, 1);
   CumulativeI32ErdShouldBe(INT32_MAX);

   WhenI32InputChangesTo(Erd_3, SomeI32Value1);
   CumulativeI32ErdShouldBe(INT32_MAX);
}

TEST(ErdAccumulatorService, ShouldWriteTheSumToCumulativeErdWhenAnyOfTheInputErdsChangeForU16Config)
{
   InitializeDataModelWith(erdTableU16, NUM_ELEMENTS(erdTableU16));
   GivenInitialization(sizeof(uint16_t), IS_SIGNED(uint16_t));

   WhenU16InputChangesTo(Erd_1, SomeU16Value1);
   CumulativeU16ErdShouldBe(SomeU16Value1);

   WhenU16InputChangesTo(Erd_2, SomeU16Value2);
   CumulativeU16ErdShouldBe(SomeU16Value1 + SomeU16Value2);

   WhenU16InputChangesTo(Erd_3, SomeU16Value3);
   CumulativeU16ErdShouldBe(SomeU16Value1 + SomeU16Value2 + SomeU16Value3);
}

TEST(ErdAccumulatorService, ShouldWriteU16MaxToTheCumulativeErdWhenAccumulationExceedsU16Max)
{
   InitializeDataModelWith(erdTableU16, NUM_ELEMENTS(erdTableU16));
   GivenInitialization(sizeof(uint16_t), IS_SIGNED(uint16_t));

   WhenU16InputChangesTo(Erd_1, UINT16_MAX - 1);
   CumulativeU16ErdShouldBe(UINT16_MAX - 1);

   WhenU16InputChangesTo(Erd_2, 1);
   CumulativeU16ErdShouldBe(UINT16_MAX);

   WhenU16InputChangesTo(Erd_3, SomeU16Value1);
   CumulativeU16ErdShouldBe(UINT16_MAX);
}

TEST(ErdAccumulatorService, ShouldWriteTheSumToCumulativeErdWhenAnyOfTheInputErdsChangeForI16Config)
{
   InitializeDataModelWith(erdTableI16, NUM_ELEMENTS(erdTableI16));
   GivenInitialization(sizeof(int16_t), IS_SIGNED(int16_t));

   WhenI16InputChangesTo(Erd_1, SomeI16Value1);
   CumulativeI16ErdShouldBe(SomeI16Value1);

   WhenI16InputChangesTo(Erd_2, SomeI16Value2);
   CumulativeI16ErdShouldBe(SomeI16Value1 + SomeI16Value2);

   WhenI16InputChangesTo(Erd_3, SomeI16Value3);
   CumulativeI16ErdShouldBe(SomeI16Value1 + SomeI16Value2 + SomeI16Value3);
}

TEST(ErdAccumulatorService, ShouldWriteI16MaxToTheCumulativeErdWhenAccumulationExceedsI16Max)
{
   InitializeDataModelWith(erdTableI16, NUM_ELEMENTS(erdTableI16));
   GivenInitialization(sizeof(int16_t), IS_SIGNED(int16_t));

   WhenI16InputChangesTo(Erd_1, INT16_MAX - 1);
   CumulativeI16ErdShouldBe(INT16_MAX - 1);

   WhenI16InputChangesTo(Erd_2, 1);
   CumulativeI16ErdShouldBe(INT16_MAX);

   WhenI16InputChangesTo(Erd_3, SomeI16Value1);
   CumulativeI16ErdShouldBe(INT16_MAX);
}

TEST(ErdAccumulatorService, ShouldWriteTheSumToCumulativeErdWhenAnyOfTheInputErdsChangeForU8Config)
{
   InitializeDataModelWith(erdTableU8, NUM_ELEMENTS(erdTableU8));
   GivenInitialization(sizeof(uint8_t), IS_SIGNED(uint8_t));

   WhenU8InputChangesTo(Erd_1, SomeU8Value1);
   CumulativeU8ErdShouldBe(SomeU8Value1);

   WhenU8InputChangesTo(Erd_2, SomeU8Value2);
   CumulativeU8ErdShouldBe(SomeU8Value1 + SomeU8Value2);

   WhenU8InputChangesTo(Erd_3, SomeU8Value3);
   CumulativeU8ErdShouldBe(SomeU8Value1 + SomeU8Value2 + SomeU8Value3);
}

TEST(ErdAccumulatorService, ShouldWriteU8MaxToTheCumulativeErdWhenAccumulationExceedsU8Max)
{
   InitializeDataModelWith(erdTableU8, NUM_ELEMENTS(erdTableU8));
   GivenInitialization(sizeof(uint8_t), IS_SIGNED(uint8_t));

   WhenU8InputChangesTo(Erd_1, UINT8_MAX - 1);
   CumulativeU8ErdShouldBe(UINT8_MAX - 1);

   WhenU8InputChangesTo(Erd_2, 1);
   CumulativeU8ErdShouldBe(UINT8_MAX);

   WhenU8InputChangesTo(Erd_3, SomeU8Value1);
   CumulativeU8ErdShouldBe(UINT8_MAX);
}

TEST(ErdAccumulatorService, ShouldWriteTheSumToCumulativeErdWhenAnyOfTheInputErdsChangeForI8Config)
{
   InitializeDataModelWith(erdTableI8, NUM_ELEMENTS(erdTableI8));
   GivenInitialization(sizeof(int8_t), IS_SIGNED(int8_t));

   WhenI8InputChangesTo(Erd_1, SomeI8Value1);
   CumulativeI8ErdShouldBe(SomeI8Value1);

   WhenI8InputChangesTo(Erd_2, SomeI8Value2);
   CumulativeI8ErdShouldBe(SomeI8Value1 + SomeI8Value2);

   WhenI8InputChangesTo(Erd_3, SomeI8Value3);
   CumulativeI8ErdShouldBe(SomeI8Value1 + SomeI8Value2 + SomeI8Value3);
}

TEST(ErdAccumulatorService, ShouldWriteI8MaxToTheCumulativeErdWhenAccumulationExceedsI8Max)
{
   InitializeDataModelWith(erdTableI8, NUM_ELEMENTS(erdTableI8));
   GivenInitialization(sizeof(int8_t), IS_SIGNED(int8_t));

   WhenI8InputChangesTo(Erd_1, INT8_MAX - 1);
   CumulativeI8ErdShouldBe(INT8_MAX - 1);

   WhenI8InputChangesTo(Erd_2, 1);
   CumulativeI8ErdShouldBe(INT8_MAX);

   WhenI8InputChangesTo(Erd_3, SomeI8Value1);
   CumulativeI8ErdShouldBe(INT8_MAX);
}

TEST(ErdAccumulatorService, ShouldUassertWhenTheCumulativeErdDoesNotMatchTheSizeInTheConfiguration)
{
   InitializeDataModelWith(erdTableWithDifferentCumulativeSize, NUM_ELEMENTS(erdTableWithDifferentCumulativeSize));

   ShouldFailAssertion(GivenInitialization(sizeof(uint64_t), IS_SIGNED(uint64_t)));
}

TEST(ErdAccumulatorService, ShouldUassertWhenAnInputErdDoesNotMatchSizeOfConfiguration)
{
   InitializeDataModelWith(erdTableWithDifferentSizeInputErd, NUM_ELEMENTS(erdTableWithDifferentSizeInputErd));

   ShouldFailAssertion(GivenInitialization(sizeof(uint64_t), IS_SIGNED(uint64_t)));
}

TEST(ErdAccumulatorService, ShouldUassertWhenConfiguredWithUnsupportedType)
{
   InitializeDataModelWith(erdTableWithUnsupportedTypes, NUM_ELEMENTS(erdTableWithUnsupportedTypes));

   ShouldFailAssertion(GivenInitialization(sizeof(MadeUpType_t), true));
}
