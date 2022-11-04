"use strict";

describe("AluminumMoldIceMaker,", () => {
   const hsmState = {
      global : "AluminumMoldIceMakerHsmState_Global",
      freeze : "AluminumMoldIceMakerHsmState_Freeze",
      harvest : "AluminumMoldIceMakerHsmState_Harvest",
      fill : "AluminumMoldIceMakerHsmState_Fill",
      harvestFix : "AluminumMoldIceMakerHsmState_HarvestFix",
      harvestFault : "AluminumMoldIceMakerHsmState_HarvestFault",
      thermistorFault : "AluminumMoldIceMakerHsmState_ThermistorFault"
   };

   const theAluminumMoldIceMakerHsmStateShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_AluminumMoldIceMakerHsmState");
      expect(actual).toEqual(expected);
    };

   it("should enter freeze state after initialization", async () => {
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });
});
