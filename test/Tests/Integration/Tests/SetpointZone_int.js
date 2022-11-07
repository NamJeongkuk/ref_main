"use strict";

const delay = require("javascript-common").util.delay;
const { msPerSec, msPerMin } = require("../support/constants");
const constants = require("../support/constants");

describe("SetpointZone", () => {

   beforeEach(async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });

   const providedTheVotedTemperatureOfThe = () => ({
      FreezerCompartmentIs: async (temperature, care) => {
         let votedTemperature = { temperature: temperature, care: care };
         await rockhopper.write("Erd_FreezerSetpoint_FactoryVote", votedTemperature)
      },
      FreshFoodCompartmentIs: async (temperature, care) => {
         let votedTemperature = { temperature: temperature, care: care };
         await rockhopper.write("Erd_FreshFoodSetpoint_FactoryVote", votedTemperature)
      },
      ConvertibleCompartmentCompartmentIs: async (temperature, care) => {
         let votedTemperature = { temperature: temperature, care: care };
         await rockhopper.write("Erd_ConvertibleCompartmentSetpoint_FactoryVote", votedTemperature)
      }
   });

   const theSetpointZoneFor = () => ({
      FreezerCompartmentShouldBe: async (setpointZone) => {
         const actual = await rockhopper.read("Erd_FreezerSetpointZone");
         expect(actual).toEqual(setpointZone);
      },
      FreshFoodCompartmentShouldBe: async (setpointZone) => {
         const actual = await rockhopper.read("Erd_FreshFoodSetpointZone");
         expect(actual).toEqual(setpointZone);
      },
      ConvertibleCompartmentCompartmentShouldBe: async (setpointZone) => {
         const actual = await rockhopper.read("Erd_ConvertibleCompartmentSetpointZone");
         expect(actual).toEqual(setpointZone);
      }
   });

   it("should set setpoint zone for freezer compartment", async () => {
      await providedTheVotedTemperatureOfThe().FreezerCompartmentIs(500, true);
      await theSetpointZoneFor().FreezerCompartmentShouldBe("SetpointZone_Warm");

      await providedTheVotedTemperatureOfThe().FreezerCompartmentIs(250, true);
      await theSetpointZoneFor().FreezerCompartmentShouldBe("SetpointZone_Middle");

      await providedTheVotedTemperatureOfThe().FreezerCompartmentIs(0, true);
      await theSetpointZoneFor().FreezerCompartmentShouldBe("SetpointZone_Cold");
   });

   it("should set setpoint zone for fresh food compartment", async () => {
      await providedTheVotedTemperatureOfThe().FreshFoodCompartmentIs(500, true);
      await theSetpointZoneFor().FreshFoodCompartmentShouldBe("SetpointZone_Warm");

      await providedTheVotedTemperatureOfThe().FreshFoodCompartmentIs(250, true);
      await theSetpointZoneFor().FreshFoodCompartmentShouldBe("SetpointZone_Middle");

      await providedTheVotedTemperatureOfThe().FreshFoodCompartmentIs(0, true);
      await theSetpointZoneFor().FreshFoodCompartmentShouldBe("SetpointZone_Cold");
   });

   xit("should set setpoint zone for convertible compartment", async () => {
      await providedTheVotedTemperatureOfThe().ConvertibleCompartmentCompartmentIs(500, true);
      await theSetpointZoneFor().ConvertibleCompartmentCompartmentShouldBe("SetpointZone_Warm");

      await providedTheVotedTemperatureOfThe().ConvertibleCompartmentCompartmentIs(400, true);
      await theSetpointZoneFor().ConvertibleCompartmentCompartmentShouldBe("SetpointZone_Cold");

      await providedTheVotedTemperatureOfThe().ConvertibleCompartmentCompartmentIs(0, true);
      await theSetpointZoneFor().ConvertibleCompartmentCompartmentShouldBe("SetpointZone_Cold");
   });

})
