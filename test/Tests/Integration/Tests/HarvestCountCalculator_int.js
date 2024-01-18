"use strict";

const delay = require("javascript-common").util.delay;
const After = require('../support/After.js');
const constants = require("../support/constants");

const after = (time) => ({
   inMsec: async () => {
      await After(time);
   },
   inSec: async () => {
      await After(time * constants.msPerSec);
   },
   inMin: async () => {
      await After(time * constants.msPerMin);
   }
});

describe("HarvestCountCalculator", () => {

   const temperatureBelowFreezeIntegrationTemperature = 1;
   const someIntegrationCount = 9000;

   beforeEach(async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });

   const theFreezeIntegrationCountShouldBeGreaterThan = async (expected) => {
      const actual = await rockhopper.read("Erd_AluminumMoldFreezeIntegrationCount");
      expect(actual).toBeGreaterThanOrEqual(expected);
   };

   const providedThatTheIceMakerMoldTemperatureIs = async (temperature) => {
      await rockhopper.write("Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideRequest", true);
      await rockhopper.write("Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideValueInDegFx100", temperature);
   };

   const providedThatTheCountCalculationRequestIs = async (request) => {
      await rockhopper.write("Erd_IceMaker1_HarvestCountCalculationRequest", request);
   };

   const providedTheFreezeIntegrationCountShouldBeGreaterThan = async (expected) => {
      const actual = rockhopper.read("Erd_AluminumMoldFreezeIntegrationCount");
      expect(actual).toBeGreaterThan(expected);
   };

   it('should have zero freeze integration count on init', async () => {
      await theFreezeIntegrationCountShouldBe(0);
   });

   it('should have positive freeze integration count when calculation is requested and ice maker temperature is below limit', async () => {
      await providedThatTheIceMakerMoldTemperatureIs(temperatureBelowFreezeIntegrationTemperature);
      await providedThatTheCountCalculationRequestIs(constants.set);
      await after(1).inMin();
      await theFreezeIntegrationCountShouldBeGreaterThan(someIntegrationCount);
   });

   it('should increment integration freeze count if temperature is below threshold', async () => {
      await providedThatTheIceMakerMoldTemperatureIs(temperatureBelowFreezeIntegrationTemperature);
      await providedThatTheCountCalculationRequestIs(constants.set);
      await after(1).inSec();
      theFreezeIntegrationCountShouldBeGreaterThan(2000);
      await after(1).inSec();
      theFreezeIntegrationCountShouldBeGreaterThan(3000);
   });
});
