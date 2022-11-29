"use strict";

const delay = require("javascript-common").util.delay;
const { msPerSec, msPerMin } = require("../support/constants");
const constants = require("../support/constants");

describe("SingleEvapGrid", () => {
   const active = true;
   const inActive = false;
   let block = [];

   const generateGridBlocks = async () => {
      const calculatedGridLines = await rockhopper.read("Erd_Grid_CalculatedGridLines");
      block = [
         { freezer: calculatedGridLines.freezerGridLine.extremeHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.nflGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.extremeHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.extremeHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystDeltaGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.extremeHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.highHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.extremeHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.extraHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.extremeHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.extremeHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 + 1 },

         { freezer: calculatedGridLines.freezerGridLine.superHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.nflGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.superHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.superHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystDeltaGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.superHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.highHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.superHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.extraHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.superHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.superHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 + 1 },

         { freezer: calculatedGridLines.freezerGridLine.extraHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.nflGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.extraHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.extraHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystDeltaGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.extraHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.highHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.extraHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.extraHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.extraHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.extraHighGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 + 1 },

         { freezer: calculatedGridLines.freezerGridLine.highHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.nflGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.highHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.highHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystDeltaGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.highHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.highHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.highHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.extraHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.highHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.highHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 + 1 },

         { freezer: calculatedGridLines.freezerGridLine.deltaGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.nflGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.deltaGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.deltaGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystDeltaGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.deltaGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.highHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.deltaGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.extraHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.deltaGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.deltaGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 + 1 },

         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.nflGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystDeltaGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.highHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.extraHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 + 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 + 1 },

         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 - 1, freshFood: calculatedGridLines.freshFoodGridLine.nflGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 - 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 - 1, freshFood: calculatedGridLines.freshFoodGridLine.lowHystDeltaGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 - 1, freshFood: calculatedGridLines.freshFoodGridLine.highHystGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 - 1, freshFood: calculatedGridLines.freshFoodGridLine.extraHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 - 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 - 1 },
         { freezer: calculatedGridLines.freezerGridLine.lowHystGridLinesDegFx100 - 1, freshFood: calculatedGridLines.freshFoodGridLine.superHighGridLinesDegFx100 + 1 },
      ];
   }

   beforeAll(async () => {
      await generateGridBlocks();
      await rockhopper.write("Erd_GridOverrideEnable", true);
      await rockhopper.write("Erd_Freezer_FilteredTemperatureOverrideRequest", true);
      await rockhopper.write("Erd_FreshFood_FilteredTemperatureOverrideRequest", true);
   });

   afterAll(async () => {
      await rockhopper.write("Erd_GridOverrideEnable", false);
      await rockhopper.write("Erd_Freezer_FilteredTemperatureOverrideRequest", false);
      await rockhopper.write("Erd_FreshFood_FilteredTemperatureOverrideRequest", false);
   });

   const providedTheResolvedFilteredTemperatureForThe = () => ({
      FreezerCompartmentIs: async (temperatureDegFx100) => {
         await rockhopper.write("Erd_Freezer_FilteredTemperatureOverrideValueInDegFx100", temperatureDegFx100);
      },
      FreshFoodCompartmentIs: async (temperatureDegFx100) => {
         await rockhopper.write("Erd_FreshFood_FilteredTemperatureOverrideValueInDegFx100", temperatureDegFx100);
      }
   });

   const providedTheGridLinesAreAdjustedTo = async (gridLineBlock) => {
      await providedTheResolvedFilteredTemperatureForThe().FreezerCompartmentIs(gridLineBlock.freezer);
      await providedTheResolvedFilteredTemperatureForThe().FreshFoodCompartmentIs(gridLineBlock.freshFood);
   }

   const theGridBlockShouldBe = async (expectedBlockNumber) => {
      const actual = await rockhopper.read("Erd_Grid_BlockNumber");
      expect(actual).toEqual(expectedBlockNumber);
   }

   const providedTheCoolingModeIs = async (coolingMode) => {
      await rockhopper.write("Erd_CoolingMode", coolingMode);
   }

   const providedTheCoolingSpeedIs = async (coolingSpeed) => {
      await rockhopper.write("Erd_CoolingSpeed", coolingSpeed);
   }

   const providedTheSingleEvapPulldownAndTripMitigationIs = async (state) => {
      await rockhopper.write("Erd_SingleEvaporatorPulldownActive", state);
      await rockhopper.write("Erd_CompressorTripMitigationActive", state);
   }

   const providedThePulldownInMediumCompressorSpeedIs = async (state) => {
      await rockhopper.write("Erd_PulldownInMediumCompressorSpeedEnabled", state);
   }

   const theCoolingModeShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_CoolingMode");
      expect(actual).toEqual(expected);
   }

   const theCoolingSpeedShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_CoolingSpeed");
      expect(actual).toEqual(expected);
   }

   const thePulldownStateShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_SingleEvaporatorPulldownActive");
      expect(actual).toEqual(expected);
   }

   const providedTheGridAreaIs = async (gridArea) => {
      await rockhopper.write("Erd_GridArea", gridArea);
   }

   const afterTheGridRuns = async () => {
      let signal = await rockhopper.read("Erd_GridOverrideSignal");
      signal++;

      await rockhopper.write("Erd_GridOverrideSignal", signal);
   }

   const theGridAreaShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_GridArea");
      expect(actual).toEqual(expected);
   }

   const theGridVotesShouldBe = async (expectedCompressorSpeed, expectedCondenserFanSpeed, expectedFreezerEvapFanSpeed, expectedFreshFoodDamperPosition) => {
      const actualCompressorVotedSpeed = await rockhopper.read("Erd_CompressorSpeed_GridVote");
      const actualCondenserFanVotedSpeed = await rockhopper.read("Erd_CondenserFanSpeed_GridVote");
      const actualFreezerEvapFanVotedSpeed = await rockhopper.read("Erd_FreezerEvapFanSpeed_GridVote");
      const actualFreshFoodDamperVotedPosition = await rockhopper.read("Erd_FreshFoodDamperPosition_GridVote");

      expect(actualCompressorVotedSpeed.speed).toEqual(expectedCompressorSpeed);
      expect(actualCondenserFanVotedSpeed.speed).toEqual(expectedCondenserFanSpeed);
      expect(actualFreezerEvapFanVotedSpeed.speed).toEqual(expectedFreezerEvapFanSpeed);
      expect(actualFreshFoodDamperVotedPosition.position).toEqual(expectedFreshFoodDamperPosition);

      expect(actualCompressorVotedSpeed.care).toEqual(true);
      expect(actualCondenserFanVotedSpeed.care).toEqual(true);
      expect(actualFreezerEvapFanVotedSpeed.care).toEqual(true);
      expect(actualFreshFoodDamperVotedPosition.care).toEqual(true);
   }

   it("should scroll through each block", async () => {
      for (let i = 0; i < block.length; i++) {
         await providedTheGridLinesAreAdjustedTo(block[i]);
         await afterTheGridRuns();
         await theGridBlockShouldBe(i);
      }
   });

   it("should publish the correct grid votes for blocks 0,1", async () => {
      for (let i = 0; i <= 1; i++) {
         await providedTheGridLinesAreAdjustedTo(block[i]);
         await providedThePulldownInMediumCompressorSpeedIs(false);
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_Freezer");
         await theCoolingSpeedShouldBe("CoolingSpeed_PullDown");
         await theGridAreaShouldBe("GridArea_1");
         await thePulldownStateShouldBe(true);
         await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_SuperHigh", "FanSpeed_Low", "DamperPosition_Closed");
      }
   });

   it("should publish the correct grid votes for blocks 2,3", async () => {
      for (let i = 2; i <= 3; i++) {
         await providedTheGridLinesAreAdjustedTo(block[i]);
         await providedTheCoolingModeIs("CoolingMode_Freezer");
         await providedThePulldownInMediumCompressorSpeedIs(false);
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_Freezer");
         await theCoolingSpeedShouldBe("CoolingSpeed_PullDown");
         await thePulldownStateShouldBe(true);
         await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_SuperHigh", "FanSpeed_Low", "DamperPosition_Closed");
      }
   });

   it("should publish the correct grid votes for blocks 4,5,6", async () => {
      for (let i = 4; i <= 6; i++) {
         await providedTheGridLinesAreAdjustedTo(block[i]);
         await providedTheCoolingModeIs("CoolingMode_Freezer");
         await providedThePulldownInMediumCompressorSpeedIs(false);
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_FreshFood");
         await theCoolingSpeedShouldBe("CoolingSpeed_PullDown");
         await theGridAreaShouldBe("GridArea_2");
         await thePulldownStateShouldBe(true);
         await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_SuperHigh", "FanSpeed_Low", "DamperPosition_Open");
      }
   });

   it("should publish the correct grid votes for blocks 7,8,14", async () => {
      let gridBlockNumbers = [7, 8, 14];
      for (let i = 0; i < gridBlockNumbers.length; i++) {
         await providedTheGridLinesAreAdjustedTo(block[gridBlockNumbers[i]]);
         await providedTheCoolingModeIs("CoolingMode_Freezer");
         await afterTheGridRuns();

         await theGridBlockShouldBe(gridBlockNumbers[i]);
         await theCoolingModeShouldBe("CoolingMode_Freezer");
         await theCoolingSpeedShouldBe("CoolingSpeed_High");
         await theGridAreaShouldBe("GridArea_1");
         await thePulldownStateShouldBe(false);
         await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Closed");
      }
   });

   it("should publish the correct grid votes for blocks 9,10", async () => {
      for (let i = 9; i <= 10; i++) {
         await providedTheGridLinesAreAdjustedTo(block[i]);
         await providedTheCoolingModeIs("CoolingMode_FreshFood");
         await providedTheCoolingSpeedIs("CoolingSpeed_Off");
         await providedTheSingleEvapPulldownAndTripMitigationIs(active);
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_Freezer");
         await theCoolingSpeedShouldBe("CoolingSpeed_High");
         await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Closed");

         await providedTheCoolingModeIs("CoolingMode_FreshFood");
         await providedTheCoolingSpeedIs("CoolingSpeed_Off");
         await providedTheSingleEvapPulldownAndTripMitigationIs(inActive);
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_Freezer");
         await theCoolingSpeedShouldBe("CoolingSpeed_Mid");
         await theGridVotesShouldBe("CompressorSpeed_Medium", "FanSpeed_Medium", "FanSpeed_Medium", "DamperPosition_Closed");
      }
   });

   it("should publish the correct grid votes for blocks 11,12,13", async () => {
      for (let i = 11; i <= 13; i++) {
         await providedTheGridLinesAreAdjustedTo(block[i]);
         await providedTheCoolingModeIs("CoolingMode_Freezer");
         await providedTheSingleEvapPulldownAndTripMitigationIs(active);
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_FreshFood");
         await theCoolingSpeedShouldBe("CoolingSpeed_Mid");
         await theGridAreaShouldBe("GridArea_2");
         await theGridVotesShouldBe("CompressorSpeed_Medium", "FanSpeed_High", "FanSpeed_Medium", "DamperPosition_Open");

         await providedTheCoolingModeIs("CoolingMode_Freezer");
         await providedTheSingleEvapPulldownAndTripMitigationIs(inActive);
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_FreshFood");
         await theCoolingSpeedShouldBe("CoolingSpeed_High");
         await theGridAreaShouldBe("GridArea_2");
         await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Open");
      }
   });

   it("should publish the correct grid votes for block 15", async () => {
      await providedTheGridLinesAreAdjustedTo(block[15]);
      await providedTheCoolingModeIs("CoolingMode_Freezer");
      await providedTheCoolingSpeedIs("CoolingSpeed_Off");
      await afterTheGridRuns();

      await theGridBlockShouldBe(15);
      await theCoolingModeShouldBe("CoolingMode_Freezer");
      await theCoolingSpeedShouldBe("CoolingSpeed_Mid");
      await theGridAreaShouldBe("GridArea_1");
      await theGridVotesShouldBe("CompressorSpeed_Medium", "FanSpeed_Medium", "FanSpeed_Medium", "DamperPosition_Closed");

      await providedTheCoolingSpeedIs("CoolingSpeed_High");
      await afterTheGridRuns();

      await theGridBlockShouldBe(15);
      await theCoolingModeShouldBe("CoolingMode_Freezer");
      await theCoolingSpeedShouldBe("CoolingSpeed_High");
      await theGridAreaShouldBe("GridArea_1");
      await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Closed");
   });

   it("should publish the correct grid votes for blocks 16,17", async () => {
      for (let i = 16; i <= 17; i++) {
         await providedTheGridLinesAreAdjustedTo(block[i]);
         await providedTheCoolingModeIs("CoolingMode_Freezer");
         await providedTheCoolingSpeedIs("CoolingSpeed_Off");
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_Freezer");
         await theCoolingSpeedShouldBe("CoolingSpeed_Mid");
         await thePulldownStateShouldBe(false);
         await theGridVotesShouldBe("CompressorSpeed_Medium", "FanSpeed_Medium", "FanSpeed_Medium", "DamperPosition_Closed");

         await providedTheCoolingSpeedIs("CoolingSpeed_High");
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_Freezer");
         await theCoolingSpeedShouldBe("CoolingSpeed_High");
         await thePulldownStateShouldBe(false);
         await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Closed");
      }
   });

   it("should publish the correct grid votes for block 18", async () => {
      await providedTheGridLinesAreAdjustedTo(block[18]);
      await providedTheCoolingModeIs("CoolingMode_Freezer");
      await providedTheCoolingSpeedIs("CoolingSpeed_Off");
      await providedTheSingleEvapPulldownAndTripMitigationIs(active);
      await afterTheGridRuns();

      await theGridBlockShouldBe(18);
      await theCoolingModeShouldBe("CoolingMode_FreshFood");
      await theCoolingSpeedShouldBe("CoolingSpeed_High");
      await theGridAreaShouldBe("GridArea_2");
      await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Open");

      await providedTheCoolingSpeedIs("CoolingSpeed_Off");
      await providedTheSingleEvapPulldownAndTripMitigationIs(inActive);
      await afterTheGridRuns();

      await theGridBlockShouldBe(18);
      await theCoolingModeShouldBe("CoolingMode_FreshFood");
      await theCoolingSpeedShouldBe("CoolingSpeed_Mid");
      await theGridAreaShouldBe("GridArea_2");
      await theGridVotesShouldBe("CompressorSpeed_Medium", "FanSpeed_Medium", "FanSpeed_Medium", "DamperPosition_Open");
   });

   it("should publish the correct grid votes for blocks 19,20,27,34,41", async () => {
      let gridBlockNumbers = [19, 20, 27, 34, 41];
      for (let i = 0; i < gridBlockNumbers.length; i++) {
         await providedTheCoolingModeIs("CoolingMode_Freezer");
         await providedTheCoolingSpeedIs("CoolingSpeed_Off");
         await providedTheGridLinesAreAdjustedTo(block[gridBlockNumbers[i]]);
         await afterTheGridRuns();

         await theGridBlockShouldBe(gridBlockNumbers[i]);
         await theCoolingModeShouldBe("CoolingMode_FreshFood");
         await theCoolingSpeedShouldBe("CoolingSpeed_High");
         await theGridAreaShouldBe("GridArea_2");
         await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Open");
      }
   });

   it("should publish the correct grid votes for block 21", async () => {
      await providedTheGridLinesAreAdjustedTo(block[21]);
      await providedTheCoolingModeIs("CoolingMode_Freezer");
      await providedTheCoolingSpeedIs("CoolingSpeed_Off");
      await afterTheGridRuns();

      await theGridBlockShouldBe(21);
      await theCoolingModeShouldBe("CoolingMode_Freezer");
      await theCoolingSpeedShouldBe("CoolingSpeed_Low");
      await theGridAreaShouldBe("GridArea_1");
      await thePulldownStateShouldBe(false);
      await theGridVotesShouldBe("CompressorSpeed_Low", "FanSpeed_Low", "FanSpeed_Low", "DamperPosition_Closed");
   });

   it("should publish the correct grid votes for block 22", async () => {
      await providedTheGridLinesAreAdjustedTo(block[22]);
      await providedTheCoolingModeIs("CoolingMode_Freezer");
      await providedTheCoolingSpeedIs("CoolingSpeed_Off");
      await afterTheGridRuns();

      await theGridBlockShouldBe(22);
      await theCoolingModeShouldBe("CoolingMode_Freezer");
      await theCoolingSpeedShouldBe("CoolingSpeed_Low");
      await theGridAreaShouldBe("GridArea_1");
      await thePulldownStateShouldBe(false);
      await theGridVotesShouldBe("CompressorSpeed_Low", "FanSpeed_Low", "FanSpeed_Low", "DamperPosition_Closed");

      await providedTheCoolingSpeedIs("CoolingSpeed_Low");
      await afterTheGridRuns();

      await theCoolingSpeedShouldBe("CoolingSpeed_Low");
      await theGridAreaShouldBe("GridArea_1");
      await thePulldownStateShouldBe(false);
      await theGridVotesShouldBe("CompressorSpeed_Low", "FanSpeed_Low", "FanSpeed_Low", "DamperPosition_Closed");
   });

   it("should publish the correct grid votes for blocks 23,24", async () => {
      for (let i = 23; i <= 24; i++) {
         await providedTheGridLinesAreAdjustedTo(block[i]);
         await providedTheCoolingModeIs("CoolingMode_Freezer");
         await providedTheCoolingSpeedIs("CoolingSpeed_Off");
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_Freezer");
         await theCoolingSpeedShouldBe("CoolingSpeed_Low");
         await thePulldownStateShouldBe(false);
         await theGridVotesShouldBe("CompressorSpeed_Low", "FanSpeed_Low", "FanSpeed_Low", "DamperPosition_Closed");

         await providedTheCoolingModeIs("CoolingMode_FreshFood");
         await providedTheCoolingSpeedIs("CoolingSpeed_High");
         await afterTheGridRuns();

         await theCoolingSpeedShouldBe("CoolingSpeed_High");
         await theCoolingModeShouldBe("CoolingMode_FreshFood");
         await thePulldownStateShouldBe(false);
         await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Open");
      }
   });

   it("should publish the correct grid votes for blocks 25,32,39", async () => {
      let gridBlockNumbers = [25, 32, 39];
      for (let i = 0; i < gridBlockNumbers.length; i++) {
         await providedTheGridLinesAreAdjustedTo(block[gridBlockNumbers[i]]);
         await providedTheCoolingModeIs("CoolingMode_Freezer");
         await providedTheCoolingSpeedIs("CoolingSpeed_Off");
         await afterTheGridRuns();

         await theGridBlockShouldBe(gridBlockNumbers[i]);
         await theCoolingModeShouldBe("CoolingMode_FreshFood");
         await theCoolingSpeedShouldBe("CoolingSpeed_Low");
         await theGridAreaShouldBe("GridArea_2");
         await thePulldownStateShouldBe(false);
         await theGridVotesShouldBe("CompressorSpeed_Low", "FanSpeed_Low", "FanSpeed_Low", "DamperPosition_Open");

         await providedTheCoolingSpeedIs("CoolingSpeed_High");
         await afterTheGridRuns();

         await theCoolingSpeedShouldBe("CoolingSpeed_High");
         await theCoolingModeShouldBe("CoolingMode_FreshFood");
         await theGridAreaShouldBe("GridArea_2");
         await thePulldownStateShouldBe(false);
         await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Open");
      }
   });

   it("should publish the correct grid votes for blocks 26,33,40", async () => {
      let gridBlockNumbers = [26, 33, 40];
      for (let i = 0; i < gridBlockNumbers.length; i++) {
         await providedTheGridLinesAreAdjustedTo(block[gridBlockNumbers[i]]);
         await providedTheCoolingModeIs("CoolingMode_Freezer");
         await providedTheCoolingSpeedIs("CoolingSpeed_Off");
         await afterTheGridRuns();

         await theGridBlockShouldBe(gridBlockNumbers[i]);
         await theCoolingModeShouldBe("CoolingMode_FreshFood");
         await theCoolingSpeedShouldBe("CoolingSpeed_Mid");
         await theGridAreaShouldBe("GridArea_2");
         await thePulldownStateShouldBe(false);
         await theGridVotesShouldBe("CompressorSpeed_Medium", "FanSpeed_Medium", "FanSpeed_Medium", "DamperPosition_Open");

         await providedTheCoolingSpeedIs("CoolingSpeed_High");
         await afterTheGridRuns();

         await theCoolingModeShouldBe("CoolingMode_FreshFood");
         await theCoolingSpeedShouldBe("CoolingSpeed_High");
         await theGridAreaShouldBe("GridArea_2");
         await thePulldownStateShouldBe(false);
         await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Open");
      }
   });

   it("should publish the correct grid votes for blocks 28,29,35", async () => {
      let gridBlockNumbers = [28, 29, 35];
      for (let i = 0; i < gridBlockNumbers.length; i++) {
         await providedTheGridLinesAreAdjustedTo(block[gridBlockNumbers[i]]);
         await providedTheCoolingModeIs("CoolingMode_Freezer");
         await providedTheCoolingSpeedIs("CoolingSpeed_Off");
         await afterTheGridRuns();

         await theGridBlockShouldBe(gridBlockNumbers[i]);
         await theCoolingModeShouldBe("CoolingMode_Freezer");
         await theCoolingSpeedShouldBe("CoolingSpeed_High");
         await theGridAreaShouldBe("GridArea_1");
         await thePulldownStateShouldBe(false);
         await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Closed");
      }
   });

   it("should publish the correct grid votes for block 30", async () => {
      await providedTheGridLinesAreAdjustedTo(block[30]);
      await providedTheCoolingModeIs("CoolingMode_Freezer");
      await providedTheCoolingSpeedIs("CoolingSpeed_High");
      await afterTheGridRuns();

      await theGridBlockShouldBe(30);
      await theCoolingModeShouldBe("CoolingMode_Freezer");
      await theCoolingSpeedShouldBe("CoolingSpeed_High");
      await thePulldownStateShouldBe(false);
      await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Closed");
   });

   it("should publish the correct grid votes for block 31", async () => {
      await providedTheGridLinesAreAdjustedTo(block[31]);
      await providedTheCoolingModeIs("CoolingMode_Freezer");
      await providedTheCoolingSpeedIs("CoolingSpeed_High");
      await afterTheGridRuns();

      await theGridBlockShouldBe(31);
      await theCoolingModeShouldBe("CoolingMode_FreshFood");
      await theCoolingSpeedShouldBe("CoolingSpeed_High");
      await thePulldownStateShouldBe(false);
      await theGridVotesShouldBe("CompressorSpeed_High", "FanSpeed_High", "FanSpeed_High", "DamperPosition_Open");

      await providedTheCoolingSpeedIs("CoolingSpeed_Off");
      await afterTheGridRuns();

      await theCoolingModeShouldBe("CoolingMode_Freezer");
      await theCoolingSpeedShouldBe("CoolingSpeed_Off");
      await thePulldownStateShouldBe(false);
      await theGridVotesShouldBe("CompressorSpeed_Off", "FanSpeed_Off", "FanSpeed_Off", "DamperPosition_Closed");
   });

   it("should publish the correct grid votes for block 36", async () => {
      await providedTheGridLinesAreAdjustedTo(block[36]);
      await providedTheCoolingModeIs("CoolingMode_FreshFood");
      await providedTheCoolingSpeedIs("CoolingSpeed_High");
      await afterTheGridRuns();

      await theGridBlockShouldBe(36);
      await theCoolingModeShouldBe("CoolingMode_Freezer");
      await theCoolingSpeedShouldBe("CoolingSpeed_Low");
      await thePulldownStateShouldBe(false);
      await theGridAreaShouldBe("GridArea_1");
      await theGridVotesShouldBe("CompressorSpeed_Low", "FanSpeed_Low", "FanSpeed_Low", "DamperPosition_Closed");

      await providedTheCoolingSpeedIs("CoolingSpeed_Off");
      await afterTheGridRuns();

      await theCoolingModeShouldBe("CoolingMode_Freezer");
      await theCoolingSpeedShouldBe("CoolingSpeed_Off");
      await thePulldownStateShouldBe(false);
      await theGridAreaShouldBe("GridArea_1");
      await theGridVotesShouldBe("CompressorSpeed_Off", "FanSpeed_Off", "FanSpeed_Off", "DamperPosition_Closed");
   });

   it("should publish the correct grid votes for block 37", async () => {
      await providedTheGridLinesAreAdjustedTo(block[37]);
      await providedTheCoolingModeIs("CoolingMode_FreshFood");
      await providedTheCoolingSpeedIs("CoolingSpeed_High");
      await afterTheGridRuns();

      await theGridBlockShouldBe(37);
      await theCoolingModeShouldBe("CoolingMode_FreshFood");
      await theCoolingSpeedShouldBe("CoolingSpeed_Low");
      await thePulldownStateShouldBe(false);
      await theGridVotesShouldBe("CompressorSpeed_Low", "FanSpeed_Low", "FanSpeed_Low", "DamperPosition_Open");

      await providedTheCoolingSpeedIs("CoolingSpeed_Off");
      await afterTheGridRuns();

      await theCoolingModeShouldBe("CoolingMode_FreshFood");
      await theCoolingSpeedShouldBe("CoolingSpeed_Off");
      await thePulldownStateShouldBe(false);
      await theGridVotesShouldBe("CompressorSpeed_Off", "FanSpeed_Off", "FanSpeed_Low", "DamperPosition_Open");
   });

   it("should publish the correct grid votes for block 38", async () => {
      await providedTheGridLinesAreAdjustedTo(block[38]);
      await providedTheCoolingModeIs("CoolingMode_FreshFood");
      await providedTheCoolingSpeedIs("CoolingSpeed_High");
      await afterTheGridRuns();

      await theGridBlockShouldBe(38);
      await theCoolingModeShouldBe("CoolingMode_FreshFood");
      await theCoolingSpeedShouldBe("CoolingSpeed_Low");
      await thePulldownStateShouldBe(false);
      await theGridVotesShouldBe("CompressorSpeed_Low", "FanSpeed_Low", "FanSpeed_Low", "DamperPosition_Open");

      await providedTheCoolingSpeedIs("CoolingSpeed_Off");
      await afterTheGridRuns();

      await theCoolingModeShouldBe("CoolingMode_Freezer");
      await theCoolingSpeedShouldBe("CoolingSpeed_Off");
      await thePulldownStateShouldBe(false);
      await theGridVotesShouldBe("CompressorSpeed_Off", "FanSpeed_Off", "FanSpeed_Off", "DamperPosition_Closed");
   });

   it("should publish the correct grid votes for blocks 42, 43", async () => {
      for (let i = 42; i <= 43; i++) {
         await providedTheGridLinesAreAdjustedTo(block[i]);
         await providedTheCoolingModeIs("CoolingMode_FreshFood");
         await providedTheCoolingSpeedIs("CoolingSpeed_High");
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_Freezer");
         await theCoolingSpeedShouldBe("CoolingSpeed_Off");
         await thePulldownStateShouldBe(false);
         await theGridAreaShouldBe("GridArea_1");
         await theGridVotesShouldBe("CompressorSpeed_Off", "FanSpeed_Off", "FanSpeed_Off", "DamperPosition_Closed");
      }
   });

   it("should publish the correct grid votes for blocks 44, 45", async () => {
      for (let i = 44; i <= 45; i++) {
         await providedTheGridLinesAreAdjustedTo(block[i]);
         await providedTheCoolingModeIs("CoolingMode_FreshFood");
         await providedTheCoolingSpeedIs("CoolingSpeed_High");
         await providedTheGridAreaIs("GridArea_1");
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_Freezer");
         await theCoolingSpeedShouldBe("CoolingSpeed_Off");
         await thePulldownStateShouldBe(false);
         await theGridVotesShouldBe("CompressorSpeed_Off", "FanSpeed_Off", "FanSpeed_Off", "DamperPosition_Closed");

         await providedTheGridAreaIs("GridArea_2");
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_FreshFood");
         await theCoolingSpeedShouldBe("CoolingSpeed_Off");
         await thePulldownStateShouldBe(false);
         await theGridVotesShouldBe("CompressorSpeed_Off", "FanSpeed_Off", "FanSpeed_Low", "DamperPosition_Open");
      }
   });

   it("should publish the correct grid votes for blocks 46, 47", async () => {
      for (let i = 46; i <= 47; i++) {
         await providedTheGridLinesAreAdjustedTo(block[i]);
         await providedTheCoolingModeIs("CoolingMode_Freezer");
         await providedTheCoolingSpeedIs("CoolingSpeed_High");
         await afterTheGridRuns();

         await theGridBlockShouldBe(i);
         await theCoolingModeShouldBe("CoolingMode_FreshFood");
         await theCoolingSpeedShouldBe("CoolingSpeed_Low");
         await theGridAreaShouldBe("GridArea_2");
         await theGridVotesShouldBe("CompressorSpeed_Low", "FanSpeed_Low", "FanSpeed_Low", "DamperPosition_Open");
      }
   });

   it("should publish the correct grid votes for block 48", async () => {
      await providedTheGridLinesAreAdjustedTo(block[48]);
      await providedTheCoolingModeIs("CoolingMode_Freezer");
      await providedTheCoolingSpeedIs("CoolingSpeed_High");
      await afterTheGridRuns();

      await theGridBlockShouldBe(48);
      await theCoolingModeShouldBe("CoolingMode_FreshFood");
      await theCoolingSpeedShouldBe("CoolingSpeed_Mid");
      await theGridAreaShouldBe("GridArea_2");
      await theGridVotesShouldBe("CompressorSpeed_Medium", "FanSpeed_Medium", "FanSpeed_Medium", "DamperPosition_Open");
   });
})
