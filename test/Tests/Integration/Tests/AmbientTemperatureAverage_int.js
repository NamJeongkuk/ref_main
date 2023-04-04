"use strict";

const delay = require("javascript-common").util.delay;
const After = require('../support/After.js');
const { msPerSec, msPerMin } = require("../support/constants");
const constants = require("../support/constants");

/*
Change the following ERDs to NvUnitSetting in SystemErds.h:
Erd_Ambient_FilteredTemperatureOverrideRequest
Erd_Ambient_FilteredInternalTemperatureOverrideValueInDegFx100
Flash board with DEBUG=Y for time acceleration to work
*/

const after = (time) => ({
   inMsec: async () => {
      await After(time);
   },
   inSec: async () => {
      await After(time * msPerSec);
   },
   inMin: async () => {
      await After(time * msPerMin);
   }
});

const value = {
   AmbientTemperatureAverageSampleFrequencyInMinutes: 5,
   AmbientTemperatureAverageSampleCount: 24,
   AmbientTemperatureAverageSampleCountDividedByTwo: 2
};

describe("AmbientTemperatureAverage", () => {
   const providedTheAmbientFilteredTemperatureIs = async (temp) => {
      await rockhopper.write("Erd_Ambient_FilteredTemperatureOverrideRequest", true);
      await rockhopper.write("Erd_Ambient_FilteredInternalTemperatureOverrideValueInDegFx100", temp);
   };

   const providedTheBoardHasResetAndFiveSecondsHavePassed = async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   };

   beforeEach(async () => {
      await providedTheBoardHasResetAndFiveSecondsHavePassed();
   });

   const theAmbientFilteredTemperatureShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_Ambient_CrossAmbientWindowAveragedTemperatureInDegFx100");
      expect(actual).toEqual(expected);
   };

   const crossWindowAverageShouldBe = async (actual) => {
      await rockhopper.read("Erd_Ambient_CrossAmbientWindowAveragedTemperatureInDegFx100", actual);
   };

   it("should set cross window average on init", async () => {
      await providedTheAmbientFilteredTemperatureIs(0);
      await providedTheBoardHasResetAndFiveSecondsHavePassed();
      await theAmbientFilteredTemperatureShouldBe(0);
   });

   it("should wait 5 mins before the filter is fed and read into the cross window average", async () => {
      await providedTheAmbientFilteredTemperatureIs(5);
      await providedTheBoardHasResetAndFiveSecondsHavePassed();
      await crossWindowAverageShouldBe(5);
      await after(value.AmbientTemperatureAverageSampleFrequencyInMinutes).inMin();
      await crossWindowAverageShouldBe(5);
   });

   it("should calculate window average of ambient temperature over time", async () => {
      await providedTheAmbientFilteredTemperatureIs(4);
      await providedTheBoardHasResetAndFiveSecondsHavePassed();
      await after(value.AmbientTemperatureAverageSampleFrequencyInMinutes).inMin() * (value.AmbientTemperatureAverageSampleCount / value.AmbientTemperatureAverageSampleCountDividedByTwo);
      await crossWindowAverageShouldBe(4);
      await providedTheAmbientFilteredTemperatureIs(8);
      await after(value.AmbientTemperatureAverageSampleFrequencyInMinutes).inMin() * (value.AmbientTemperatureAverageSampleCount / value.AmbientTemperatureAverageSampleCountDividedByTwo);
      await crossWindowAverageShouldBe(6);
   });
});
