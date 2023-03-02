"use strict";

const delay = require("javascript-common").util.delay;
const constants = require("../support/constants");
const After = require('../support/After.js');
const { msPerSec } = require("../support/constants");

const dispenseSelection = {
   DispenseSelectionReset: "DispenseSelection_Reset",
   DispenseSelectionColdWater: "DispenseSelection_ColdWater",
   DispenseSelectionCrushedIce: "DispenseSelection_CrushedIce",
   DispenseSelectionCubedIce: "DispenseSelection_CubedIce",
};

describe("DispenseSelectionRequest", () => {
   const dispenseSelectionRequestStatusShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_DispenseSelectionStatus");
      expect(actual).toEqual(expected);
   }

   const dispenseSelectionRequestIs = async (actual) => {
      await rockhopper.write("Erd_DispenseSelectionRequest", actual);
   }

   const dispenseSelectionRequestShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_DispenseSelectionRequest");
      expect(actual).toEqual(expected);
   }

   beforeEach(async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });

   it("should reset dispense selection on init", async () => {
      await dispenseSelectionRequestShouldBe(dispenseSelection.DispenseSelectionReset);
   });

   it("should take dispense selection and reset the dispense selection erd to default", async () => {
      await dispenseSelectionRequestIs(dispenseSelection.DispenseSelectionColdWater);
      await dispenseSelectionRequestShouldBe(dispenseSelection.DispenseSelectionReset);
      await dispenseSelectionRequestStatusShouldBe(dispenseSelection.DispenseSelectionColdWater);
   });

   it("should update dispense status when cubed selection has been requested", async () => {
      await dispenseSelectionRequestIs(dispenseSelection.DispenseSelectionCubedIce);
      await dispenseSelectionRequestShouldBe(dispenseSelection.DispenseSelectionReset);
      await dispenseSelectionRequestStatusShouldBe(dispenseSelection.DispenseSelectionCubedIce);
   });

   it("should update dispense status when crushed selection has been requested", async () => {
      await dispenseSelectionRequestIs(dispenseSelection.DispenseSelectionCrushedIce);
      await dispenseSelectionRequestShouldBe(dispenseSelection.DispenseSelectionReset);
      await dispenseSelectionRequestStatusShouldBe(dispenseSelection.DispenseSelectionCrushedIce);
   });
});
