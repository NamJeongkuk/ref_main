"use strict";

const delay = require("javascript-common").util.delay;

describe("FillTubeHeaterNonHarvestControl", () => {

   const constants = require("../support/constants");
   const active = true;
   const inactive = false;
   const parametricallyDefinedDutyCycle = 10;

   const voteState = {
      voteCare: "Vote_Care",
      voteDontCare: "Vote_DontCare"
   };

   const providedThatIceAlgorithmIsActiveIs = async (isActive) => {
      await rockhopper.write("Erd_Freezer_IceRateIsActive", isActive);
   };

   const fillTubeHeaterNonHarvestControlVoteShouldBe = async (requestedPercentage, requestedCare) => {
      let requestedVote = { percentageDutyCycle: requestedPercentage, care: requestedCare };
      let actualVote = await rockhopper.read("Erd_AluminumMoldIceMakerFillTubeHeater_NonHarvestVote");
      expect(actualVote.percentageDutyCycle).toEqual(requestedVote.percentageDutyCycle);
      expect(actualVote.care).toEqual(requestedVote.care);
   };

   beforeEach(async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);

      await providedThatIceAlgorithmIsActiveIs(inactive);
      await providedThatIceAlgorithmIsActiveIs(inactive);
   });

   it("should request parametric percent duty cycle when ice algorithm not active", async () => {
      // after setup
      await fillTubeHeaterNonHarvestControlVoteShouldBe(parametricallyDefinedDutyCycle, voteState.voteCare);
   });

   it("should vote dont care on when ice algorithm is active and vote care again when it becomes inactive", async () => {
      await providedThatIceAlgorithmIsActiveIs(active);
      await fillTubeHeaterNonHarvestControlVoteShouldBe(parametricallyDefinedDutyCycle, voteState.voteDontCare);

      await providedThatIceAlgorithmIsActiveIs(inactive);
      await fillTubeHeaterNonHarvestControlVoteShouldBe(parametricallyDefinedDutyCycle, voteState.voteCare);
   });
});
