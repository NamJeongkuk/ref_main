"use strict";

const delay = require("javascript-common").util.delay;
const { msPerSec, msPerMin } = require("../support/constants");
const constants = require("../support/constants");

const someSetpointOverTheFreezerBoundsInDegF = 7;
const someSetpointUnderTheFreezerBoundsInDegF = -10;
const someSetpointWithinTheFreezerBoundsInDegF = 3;
const someSetpointWithinTheFreezerBoundsInDegFx100 = someSetpointWithinTheFreezerBoundsInDegF * 100;
const someOtherSetpointWithinTheFreezerBoundsInDegF = 4;
const someOtherSetpointWithinTheFreezerBoundsInDegFx100 = someOtherSetpointWithinTheFreezerBoundsInDegF * 100;

const someSetpointOverTheFreshFoodBoundsInDegF = 50;
const someSetpointUnderTheFreshFoodBoundsInDegF = 30;
const someSetpointWithinTheFreshFoodBoundsInDegF = 40;
const someSetpointWithinTheFreshFoodBoundsInDegFx100 = someSetpointWithinTheFreshFoodBoundsInDegF * 100;
const someOtherSetpointWithinTheFreshFoodBoundsInDegF = 41;
const someOtherSetpointWithinTheFreshFoodBoundsInDegFx100 = someOtherSetpointWithinTheFreshFoodBoundsInDegF * 100;

const freshFoodDefaultTemperatureInDegF = 37;
const freshFoodDefaultTemperatureInDegFx100 = freshFoodDefaultTemperatureInDegF * 100;

const freezerDefaultTemperatureInDegF = 0;
const freezerDefaultTemperatureInDegFx100 = freezerDefaultTemperatureInDegF * 100;

const defaultStatusValue = 127;

describe("UserSetpointRequest", () => {
   beforeEach(async () => {
      await rx130.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });

   const providedTheSetpointRequestForThe = () => ({
      FreezerCompartmentIs: async (setpointInDegF) => {
         await rx130.write("Erd_FreezerSetpointRequest", setpointInDegF);
      },
      FreshFoodCompartmentIs: async (setpointInDegF) => {
         await rx130.write("Erd_FreshFoodSetpointRequest", setpointInDegF);
      }
   });

   const providedTheSetpointStatusForThe = () => ({
      FreezerCompartmentIs: async (setpointInDegF) => {
         await rx130.write("Erd_FreezerSetpointStatus", setpointInDegF);
      },
      FreshFoodCompartmentIs: async (setpointInDegF) => {
         await rx130.write("Erd_FreshFoodSetpointStatus", setpointInDegF);
      }
   });

   const theUserVotedSetpointForThe = () => ({
      FreezerCompartmentShouldBe: async (setpointInDegFx100) => {
         const actual = await rx130.read("Erd_FreezerSetpoint_UserVote");
         expect(actual.temperature).toEqual(setpointInDegFx100);
      },
      FreshFoodCompartmentShouldBe: async (setpointInDegFx100) => {
         const actual = await rx130.read("Erd_FreshFoodSetpoint_UserVote");
         expect(actual.temperature).toEqual(setpointInDegFx100);
      }
   });

   const theSetpointStatusForThe = () => ({
      FreezerCompartmentShouldBe: async (setpointInDegF) => {
         const actual = await rx130.read("Erd_FreezerSetpointStatus");
         expect(actual).toEqual(setpointInDegF);
      },
      FreshFoodCompartmentShouldBe: async (setpointInDegF) => {
         const actual = await rx130.read("Erd_FreshFoodSetpointStatus");
         expect(actual).toEqual(setpointInDegF);
      }
   });

   it("it should vote for freezer setpoint if the request is within the bounds", async () => {
      await providedTheSetpointRequestForThe().FreezerCompartmentIs(someSetpointWithinTheFreezerBoundsInDegF);

      await theUserVotedSetpointForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegFx100);
      await theSetpointStatusForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegF);
   });

   it("it should vote for freezer setpoint after a reset occurred", async () => {
      await providedTheSetpointRequestForThe().FreezerCompartmentIs(someSetpointWithinTheFreezerBoundsInDegF);
      await theUserVotedSetpointForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegFx100);
      await theSetpointStatusForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegF);

      await rx130.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);

      await theUserVotedSetpointForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegFx100);
      await theSetpointStatusForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegF);
   });

   it("it should vote for default freezer setpoint if there is no stored setpoint status", async () => {
      await providedTheSetpointStatusForThe().FreezerCompartmentIs(defaultStatusValue);
      await delay(constants.oneSecondInMsec);

      await rx130.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);

      await theUserVotedSetpointForThe().FreezerCompartmentShouldBe(freezerDefaultTemperatureInDegFx100);
      await theSetpointStatusForThe().FreezerCompartmentShouldBe(freezerDefaultTemperatureInDegF);
   });

   it("it should vote for freezer setpoint if the request is over the bounds", async () => {
      await providedTheSetpointRequestForThe().FreezerCompartmentIs(someSetpointWithinTheFreezerBoundsInDegF);
      await theUserVotedSetpointForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegFx100);
      await theSetpointStatusForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegF);

      await providedTheSetpointRequestForThe().FreezerCompartmentIs(someSetpointOverTheFreezerBoundsInDegF);
      await theUserVotedSetpointForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegFx100);
      await theSetpointStatusForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegF);
   });

   it("it should vote for freezer setpoint if the request is under the bounds", async () => {
      await providedTheSetpointRequestForThe().FreezerCompartmentIs(someSetpointWithinTheFreezerBoundsInDegF);
      await theUserVotedSetpointForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegFx100);
      await theSetpointStatusForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegF);

      await providedTheSetpointRequestForThe().FreezerCompartmentIs(someSetpointUnderTheFreezerBoundsInDegF);
      await theUserVotedSetpointForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegFx100);
      await theSetpointStatusForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegF);
   });

   it("should be able to make multiple freezer requests", async () => {
      await providedTheSetpointRequestForThe().FreezerCompartmentIs(someSetpointWithinTheFreezerBoundsInDegF);
      await theUserVotedSetpointForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegFx100);
      await theSetpointStatusForThe().FreezerCompartmentShouldBe(someSetpointWithinTheFreezerBoundsInDegF);

      await providedTheSetpointRequestForThe().FreezerCompartmentIs(someOtherSetpointWithinTheFreezerBoundsInDegF);
      await theUserVotedSetpointForThe().FreezerCompartmentShouldBe(someOtherSetpointWithinTheFreezerBoundsInDegFx100);
      await theSetpointStatusForThe().FreezerCompartmentShouldBe(someOtherSetpointWithinTheFreezerBoundsInDegF);
   });

   it("it should vote for fresh food setpoint if the request is within the bounds", async () => {
      await providedTheSetpointRequestForThe().FreshFoodCompartmentIs(someSetpointWithinTheFreshFoodBoundsInDegF);

      await theUserVotedSetpointForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegFx100);
      await theSetpointStatusForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegF);
   });

   it("it should vote for fresh food setpoint after a reset occurred", async () => {
      await providedTheSetpointRequestForThe().FreshFoodCompartmentIs(someSetpointWithinTheFreshFoodBoundsInDegF);
      await theUserVotedSetpointForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegFx100);
      await theSetpointStatusForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegF);

      await rx130.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);

      await theUserVotedSetpointForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegFx100);
      await theSetpointStatusForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegF);
   });

   it("it should vote for default fresh food setpoint if there is no stored setpoint status", async () => {
      await providedTheSetpointStatusForThe().FreshFoodCompartmentIs(defaultStatusValue);
      await delay(constants.oneSecondInMsec);

      await rx130.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);

      await theUserVotedSetpointForThe().FreshFoodCompartmentShouldBe(freshFoodDefaultTemperatureInDegFx100);
      await theSetpointStatusForThe().FreshFoodCompartmentShouldBe(freshFoodDefaultTemperatureInDegF);
   });

   it("it should vote for fresh food setpoint if the request is over the bounds", async () => {
      await providedTheSetpointRequestForThe().FreshFoodCompartmentIs(someSetpointWithinTheFreshFoodBoundsInDegF);
      await theUserVotedSetpointForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegFx100);
      await theSetpointStatusForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegF);


      await providedTheSetpointRequestForThe().FreshFoodCompartmentIs(someSetpointOverTheFreshFoodBoundsInDegF);
      await theUserVotedSetpointForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegFx100);
      await theSetpointStatusForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegF);
   });

   it("it should vote for fresh food setpoint if the request is under the bounds", async () => {
      await providedTheSetpointRequestForThe().FreshFoodCompartmentIs(someSetpointWithinTheFreshFoodBoundsInDegF);
      await theUserVotedSetpointForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegFx100);
      await theSetpointStatusForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegF);

      await providedTheSetpointRequestForThe().FreshFoodCompartmentIs(someSetpointUnderTheFreshFoodBoundsInDegF);
      await theUserVotedSetpointForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegFx100);
      await theSetpointStatusForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegF);
   });

   it("should be able to make multiple fresh food requests", async () => {
      await providedTheSetpointRequestForThe().FreshFoodCompartmentIs(someSetpointWithinTheFreshFoodBoundsInDegF);
      await theUserVotedSetpointForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegFx100);
      await theSetpointStatusForThe().FreshFoodCompartmentShouldBe(someSetpointWithinTheFreshFoodBoundsInDegF);

      await providedTheSetpointRequestForThe().FreshFoodCompartmentIs(someOtherSetpointWithinTheFreshFoodBoundsInDegF);
      await theUserVotedSetpointForThe().FreshFoodCompartmentShouldBe(someOtherSetpointWithinTheFreshFoodBoundsInDegFx100);
      await theSetpointStatusForThe().FreshFoodCompartmentShouldBe(someOtherSetpointWithinTheFreshFoodBoundsInDegF);
   });
})
