"use strict";

const delay = require("javascript-common").util.delay;
const After = require('../support/After.js');
const { msPerSec, msPerMin } = require("../support/constants");
const constants = require("../support/constants");

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

const aPercentageDutyCycle = 10;
const anotherPercentageDutyCycle = 90;
const aPwmDutyCycleInCounts = 6553;
const anotherPwmDutyCycleInCounts = 58981;

describe("HeaterPwmOutput,", () => {

   const providedTheFreshFoodDamperHeaterPercentageFactoryVoteIs = async (dutyCyclePercentage) => {
      let requestedVote = { percentageDutyCycle: dutyCyclePercentage, care: 'Vote_Care' };
      await rockhopper.write("Erd_FreshFoodDamperHeater_FactoryVote", requestedVote);
   };

   const providedTheFillTubeHeaterPercentageFactoryVoteIs = async (dutyCyclePercentage) => {
      let requestedVote = { percentageDutyCycle: dutyCyclePercentage, care: 'Vote_Care' };
      await rockhopper.write("Erd_FillTubeHeater_FactoryVote", requestedVote);
   };

   const theFreshFoodDamperHeaterPwmDutyCycleShouldBe = async (expectedDutyCycle) => {
      const actual = await rockhopper.read("Erd_FreshFoodDamperHeaterPwmDutyCycle")
      expect(actual).toEqual(expectedDutyCycle);
   };

   const theFillTubeHeaterPwmDutyCycleShouldBe = async (expectedDutyCycle) => {
      const actual = await rockhopper.read("Erd_FillTubeHeater_Pwm")
      expect(actual).toEqual(expectedDutyCycle);
   };

   it("should place correct duty cycle in fresh food damper heater duty cycle erd when percentage vote changes", async () => {
      await providedTheFreshFoodDamperHeaterPercentageFactoryVoteIs(aPercentageDutyCycle);
      await delay(500);
      await theFreshFoodDamperHeaterPwmDutyCycleShouldBe(aPwmDutyCycleInCounts);

      await delay(500);
      await providedTheFreshFoodDamperHeaterPercentageFactoryVoteIs(anotherPercentageDutyCycle);
      await delay(500);
      await theFreshFoodDamperHeaterPwmDutyCycleShouldBe(anotherPwmDutyCycleInCounts);
   });

   it("should place correct duty cycle in fill tube heater duty cycle erd when percentage vote changes", async () => {
      await providedTheFillTubeHeaterPercentageFactoryVoteIs(aPercentageDutyCycle);
      await delay(500);
      await theFillTubeHeaterPwmDutyCycleShouldBe(aPwmDutyCycleInCounts);

      await delay(500);
      await providedTheFillTubeHeaterPercentageFactoryVoteIs(anotherPercentageDutyCycle);
      await delay(500);
      await theFillTubeHeaterPwmDutyCycleShouldBe(anotherPwmDutyCycleInCounts);
   });

});
