"use strict";

const delay = require("javascript-common").util.delay;
const { msPerSec, msPerMin } = require("../support/constants");
const constants = require("../support/constants");

describe("OffsetAdder", () => {
   const providedTheFreshFoodCabinetOffsetInDegFx100Is = async (requestedTemp) => {
      await rockhopper.write("Erd_FreshFood_CabinetOffsetInDegFx100", requestedTemp);
   };

   const providedTheFreshFoodCrossAmbientOffsetInDegFx100Is = async (requestedTemp) => {
      await rockhopper.write("Erd_FreshFood_CrossAmbientOffsetInDegFx100", requestedTemp);
   };

   const providedTheModuleIsInitializedWithAllConfigurationErdsSetToZero = async () => {
      await providedTheFreshFoodCabinetOffsetInDegFx100Is(0);
      await providedTheFreshFoodCrossAmbientOffsetInDegFx100Is(0);
   };

   const theFreshFoodCabinetCrossAmbientOffsetsSumInDegFx100ShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_FreshFood_CabinetPlusCrossAmbientOffsetInDegFx100");
      expect(actual).toEqual(expected);
   };

   beforeEach(async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });

   it("should calculate the added erds on init", async () => {
      await providedTheFreshFoodCabinetOffsetInDegFx100Is(1);
      await providedTheFreshFoodCrossAmbientOffsetInDegFx100Is(1);

      await theFreshFoodCabinetCrossAmbientOffsetsSumInDegFx100ShouldBe(2);
   });

   it("should calculate the added erds when the configuration erd is updated", async () => {
      await providedTheModuleIsInitializedWithAllConfigurationErdsSetToZero();
      await theFreshFoodCabinetCrossAmbientOffsetsSumInDegFx100ShouldBe(0);

      await providedTheFreshFoodCabinetOffsetInDegFx100Is(1);

      await theFreshFoodCabinetCrossAmbientOffsetsSumInDegFx100ShouldBe(1);
   });

   it("should recalculate the added erds when the other configuration erd is updated", async () => {
      await providedTheModuleIsInitializedWithAllConfigurationErdsSetToZero();
      await theFreshFoodCabinetCrossAmbientOffsetsSumInDegFx100ShouldBe(0);

      await providedTheFreshFoodCabinetOffsetInDegFx100Is(1);
      await theFreshFoodCabinetCrossAmbientOffsetsSumInDegFx100ShouldBe(1);

      await providedTheFreshFoodCrossAmbientOffsetInDegFx100Is(2);
      await theFreshFoodCabinetCrossAmbientOffsetsSumInDegFx100ShouldBe(3);
   });

   it("should limit to add the erds when result erd overflows", async () => {
      await providedTheModuleIsInitializedWithAllConfigurationErdsSetToZero();

      await providedTheFreshFoodCabinetOffsetInDegFx100Is(INT16_MAX);
      await theFreshFoodCabinetCrossAmbientOffsetsSumInDegFx100ShouldBe(INT16_MAX);

      await providedTheFreshFoodCrossAmbientOffsetInDegFx100Is(1);
      await theFreshFoodCabinetCrossAmbientOffsetsSumInDegFx100ShouldBe(INT16_MAX);
   });

   it("should limit to add the erds when result erd underflows", async () => {
      await providedTheModuleIsInitializedWithAllConfigurationErdsSetToZero();

      await providedTheFreshFoodCabinetOffsetInDegFx100Is(INT16_MIN);
      await theFreshFoodCabinetCrossAmbientOffsetsSumInDegFx100ShouldBe(INT16_MIN);

      await providedTheFreshFoodCrossAmbientOffsetInDegFx100Is(-1);
      await theFreshFoodCabinetCrossAmbientOffsetsSumInDegFx100ShouldBe(INT16_MIN);
   });
})
