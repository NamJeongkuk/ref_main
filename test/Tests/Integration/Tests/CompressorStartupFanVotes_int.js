"use strict";

const delay = require("javascript-common").util.delay;
const After = require('../support/After.js');
const { msPerSec } = require("../support/constants");
const constants = require("../support/constants");

describe("CompressorStartupFanVotes,", () => {
   const startUpTimeInSeconds = 100;

   const compressorState = {
      compressorStateOff: "CompressorState_Off",
      compressorStateStartup: "CompressorState_Startup"
   };

   const coolingMode = {
      coolingModeFreshFood: "CoolingMode_FreshFood",
      coolingModeFreezer: "CoolingMode_Freezer",
      coolingModeConvertibleCompartment: "CoolingMode_ConvertibleCompartment",
      coolingModeUnknown: "CoolingMode_Unknown"
   };

   const fanSpeed = {
      fanSpeedOff: "FanSpeed_Off",
      fanSpeedSuperLow: "FanSpeed_SuperLow",
      fanSpeedLow: "FanSpeed_Low",
      fanSpeedMedium: "FanSpeed_Medium",
      fanSpeedHigh: "FanSpeed_High",
      fanSpeedSuperHigh: "FanSpeed_SuperHigh"
   };

   const after = (time) => ({
      inMsec: async () => {
         await After(time);
      },
      inSec: async () => {
         await After(time * msPerSec);
      }
   });

   const theCompressorStateIs = async (state) => {
      await rockhopper.write("Erd_CompressorState", state);
   };

   const theCoolingModeIs = async (state) => {
      await rockhopper.write("Erd_CoolingMode", state);
   };

   const theVoteFor = () => ({
      FreezerEvapFanSpeedShouldBe: async (expectedCare, expectedSpeed = null) => {
         const actual = await rockhopper.read("Erd_FreezerEvapFanSpeed_CompressorStartUpVote");
         if (expectedSpeed != null) {
            expect(actual.speed).toEqual(expectedSpeed);
         }
         expect(actual.care).toEqual(expectedCare);
      },
      FreshFoodEvapFanSpeedShouldBe: async (expectedCare, expectedSpeed = null) => {
         const actual = await rockhopper.read("Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote");
         if (expectedSpeed != null) {
            expect(actual.speed).toEqual(expectedSpeed);
         }
         expect(actual.care).toEqual(expectedCare);
      },
      CondenserFanSpeedShouldBe: async (expectedCare, expectedSpeed = null) => {
         const actual = await rockhopper.read("Erd_CondenserFanSpeed_CompressorStartUpVote");
         if (expectedSpeed != null) {
            expect(actual.speed).toEqual(expectedSpeed);
         }
         expect(actual.care).toEqual(expectedCare);
      }
   });

   beforeEach(async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });

   // Single evaporator number_evaporators = 1
   it("should vote for condenser fan speed low when compressor state changes to startup", async () => {
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().CondenserFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
   });

   it("should vote don't care for condenser fan speed after start up time expires", async () => {
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().CondenserFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);

      await after(startUpTimeInSeconds).inSec();
      await theVoteFor().CondenserFanSpeedShouldBe(false, fanSpeed.fanSpeedOff);
   });

   it("should not vote for fresh food fan speed when compressor state changes to startup", async () => {
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreshFoodEvapFanSpeedShouldBe(false, fanSpeed.fanSpeedOff);
   });

   it("should vote for freezer fan speed low when compressor state changes to startup", async () => {
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreezerEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
   });

   it("should vote don't care for freezer fan speed after start up time expires", async () => {
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreezerEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);

      await after(startUpTimeInSeconds).inSec();
      await theVoteFor().FreezerEvapFanSpeedShouldBe(false, fanSpeed.fanSpeedOff);
   });

   // Dual evaporator number_evaporators = 2
   xit("should vote for condenser fan speed low when compressor state changes to startup", async () => {
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().CondenserFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
   });

   xit("should vote for fresh food fan speed low when compressor state changes to startup and cooling mode is fresh food", async () => {
      await theCoolingModeIs(coolingMode.coolingModeFreshFood);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreshFoodEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
   });

   xit("should vote for freezer fan speed off when compressor state changes to startup and cooling mode is fresh food", async () => {
      await theCoolingModeIs(coolingMode.coolingModeFreshFood);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreezerEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);
   });

   xit("should vote for fresh food fan speed off when compressor state changes to startup and cooling mode is freezer", async () => {
      await theCoolingModeIs(coolingMode.coolingModeFreezer);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreshFoodEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);
   });

   xit("should vote for freezer fan speed low when compressor state changes to startup and cooling mode is freezer", async () => {
      await theCoolingModeIs(coolingMode.coolingModeFreezer);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreezerEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
   });

   xit("should vote for fresh food fan speed off when compressor state changes to startup and cooling mode is convertible compartment", async () => {
      await theCoolingModeIs(coolingMode.coolingModeConvertibleCompartment);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreshFoodEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);
   });

   xit("should vote for freezer fan speed low when compressor state changes to startup and cooling mode is convertible compartment", async () => {
      await theCoolingModeIs(coolingMode.coolingModeConvertibleCompartment);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreezerEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
   });

   xit("should vote for fresh food fan speed off when compressor state changes to startup and cooling mode is unknown", async () => {
      await theCoolingModeIs(coolingMode.coolingModeUnknown);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreshFoodEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);
   });

   xit("should vote for freezer fan speed low when compressor state changes to startup and cooling mode is unknown", async () => {
      await theCoolingModeIs(coolingMode.coolingModeUnknown);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreezerEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
   });

   xit("should vote don't care for fresh food fan speed after start up time expires", async () => {
      await theCoolingModeIs(coolingMode.coolingModeFreshFood);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreshFoodEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);

      await after(startUpTimeInSeconds).inSec();
      await theVoteFor().FreshFoodEvapFanSpeedShouldBe(false, fanSpeed.fanSpeedOff);
   });

   // Triple evaporator number_evaporators = 3
   xit("should vote for condenser fan speed low when compressor state changes to startup", async () => {
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().CondenserFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
   });

   xit("should vote for fresh food fan speed low when compressor state changes to startup and cooling mode is fresh food", async () => {
      await theCoolingModeIs(coolingMode.coolingModeFreshFood);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreshFoodEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
   });

   xit("should vote for freezer fan speed off when compressor state changes to startup and cooling mode is fresh food", async () => {
      await theCoolingModeIs(coolingMode.coolingModeFreshFood);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreezerEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);
   });

   xit("should vote for fresh food fan speed off when compressor state changes to startup and cooling mode is freezer", async () => {
      await theCoolingModeIs(coolingMode.coolingModeFreezer);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreshFoodEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);
   });

   xit("should vote for freezer fan speed low when compressor state changes to startup and cooling mode is freezer", async () => {
      await theCoolingModeIs(coolingMode.coolingModeFreezer);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreezerEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
   });

   xit("should vote for fresh food fan speed off when compressor state changes to startup and cooling mode is convertible compartment", async () => {
      await theCoolingModeIs(coolingMode.coolingModeConvertibleCompartment);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreshFoodEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);
   });

   xit("should vote for freezer fan speed low when compressor state changes to startup and cooling mode is convertible compartment", async () => {
      await theCoolingModeIs(coolingMode.coolingModeConvertibleCompartment);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreezerEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
   });

   xit("should vote for fresh food fan speed off when compressor state changes to startup and cooling mode is unknown", async () => {
      await theCoolingModeIs(coolingMode.coolingModeUnknown);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreshFoodEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);
   });

   xit("should vote for freezer fan speed low when compressor state changes to startup and cooling mode is unknown", async () => {
      await theCoolingModeIs(coolingMode.coolingModeUnknown);
      await theCompressorStateIs(compressorState.compressorStateStartup);
      await theVoteFor().FreezerEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
   });
})
