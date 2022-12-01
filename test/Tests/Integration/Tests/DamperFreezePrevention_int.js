"use strict";

const delay = require("javascript-common").util.delay;
const { msPerSec, msPerMin, secondsPerMin } = require("../support/constants");
const constants = require("../support/constants");

const after = (time) => ({
   inMsec: async () => {
      await delay(time);
   },
   inSec: async () => {
      await delay(time * msPerSec);
   },
   inMinutes: async () => {
      await delay(time * msPerMin);
   },
});

describe("DamperFreezePrevention", () => {
   const damperFreezePreventionFsmState = {
      idle: "DamperFreezePreventionFsmState_Idle",
      monitoringTemperatureChange: "DamperFreezePreventionFsmState_MonitoringTemperatureChange",
      damperHeaterOn: "DamperFreezePreventionFsmState_DamperHeaterOn",
      moveDamper: "DamperFreezePreventionFsmState_MoveDamper"
   };

   const damperPosition = {
      closed: "DamperPosition_Closed",
      open: "DamperPosition_Open"
   };

   const voteCare = {
      dontCare: "Vote_DontCare",
      care: "Vote_Care"
   };

   const someTargetTemperatureInDegFx100 = 3200;

   // Parametric values
   const damperHeaterOnTimeInMinutes = 2;
   const minimumTemperatureChangeTimeInMinutes = 3;
   const sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 = 500;
   const targetCompartmentMinimumTemperatureChangeInDegFx100 = 10;

   beforeEach(async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });

   const providedTheBoardHasBeenReset = async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   };

   const theDamperFreezePreventionFsmStateShouldBe = async (state) => {
      const actual = await rockhopper.read("Erd_DamperFreezePreventionFsmState");
      expect(actual).toEqual(state);
   };

   const currentPositionIs = async (damperPosition) => {
      await rockhopper.write("Erd_FreshFoodDamperCurrentPosition", damperPosition);
   };

   const damperPositionShouldBeVoted = async (damperPosition) => {
      const vote = await rockhopper.read("Erd_FreshFoodDamperPosition_DamperFreezePreventionVote");
      expect(vote.position).toEqual(damperPosition);
      expect(vote.care).toEqual(vote.care);
   };

   const damperPositionShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_FreshFoodDamperCurrentPosition");
      expect(actual).toEqual(expected);
   };

   const providedTheTargetTemperatureIs = async (temperature) => {
      await rockhopper.write("Erd_FreshFood_FilteredTemperatureOverrideRequest", true);
      await rockhopper.write("Erd_FreshFood_FilteredTemperatureOverrideValueInDegFx100", temperature);
   };

   const whenTheTargetTemperatureIs = async (temperature) => {
      await providedTheTargetTemperatureIs(temperature);
   };

   const providedTheTargetThermistorIsValid = async () => {
      await rockhopper.write("Erd_FreshFoodThermistor_IsValidOverrideRequest", true);
      await rockhopper.write("Erd_FreshFoodThermistor_IsValidOverrideValue", true);
   };

   const whenTheTargetThermistorIsValid = async () => {
      await providedTheTargetThermistorIsValid();
   };

   const providedTheTargetThermistorIsInvalid = async () => {
      await rockhopper.write("Erd_FreshFoodThermistor_IsValidOverrideRequest", true);
      await rockhopper.write("Erd_FreshFoodThermistor_IsValidOverrideValue", false);
   };

   const whenTheTargetThermistorIsInvalid = async () => {
      await providedTheTargetThermistorIsInvalid();
   };

   const providedTheSourceThermistorIsValid = async () => {
      await rockhopper.write("Erd_FreezerThermistor_IsValidOverrideRequest", true);
      await rockhopper.write("Erd_FreezerThermistor_IsValidOverrideValue", true);
   };

   const whenTheSourceThermistorIsValid = async () => {
      await providedTheSourceThermistorIsValid();
   };

   const providedTheSourceThermistorIsInvalid = async () => {
      await rockhopper.write("Erd_FreezerThermistor_IsValidOverrideRequest", true);
      await rockhopper.write("Erd_FreezerThermistor_IsValidOverrideValue", false);
   };

   const whenTheSourceThermistorIsInvalid = async () => {
      await providedTheSourceThermistorIsInvalid();
   };

   const providedTheSourceTemperatureIs = async(temperature) => {
      await rockhopper.write("Erd_Freezer_FilteredTemperatureOverrideRequest", true);
      await rockhopper.write("Erd_Freezer_FilteredTemperatureOverrideValueInDegFx100", temperature);
   };

   const whenTheSourceTemperatureIs = async(temperature) => {
      await providedTheSourceTemperatureIs(temperature);
   };

   const providedTheDamperFreezePreventionIsInMonitoringTemperatureChange = async () => {
      await providedTheTargetThermistorIsValid();
      await providedTheSourceThermistorIsValid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100  - 1);
      await providedTheBoardHasBeenReset();

      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.monitoringTemperatureChange);
   };

   const whenTheDamperOpens = async () => {
      await rockhopper.write("Erd_FreshFoodDamperCurrentPosition", damperPosition.open);

      let vote = { position: "DamperPosition_Open", care: "Vote_Care" };
      await rockhopper.write("Erd_FreshFoodDamperPosition_FactoryVote", vote);
   };

   const whenTheDamperCloses = async () => {
      await rockhopper.write("Erd_FreshFoodDamperCurrentPosition", damperPosition.closed);

      let vote = { position: "DamperPosition_Closed", care: "Vote_Care" };
      await rockhopper.write("Erd_FreshFoodDamperPosition_FactoryVote", vote);
   };

   const damperHeaterPercentageDutyCycleVoteShouldBe = async (state) => {
      const vote = await rockhopper.read("Erd_FreshFoodDamperHeater_DamperFreezePreventionVote");
      expect(vote.dutyCycle).toEqual(state);
      expect(vote.care).toEqual(voteCare.care);
   };

   const damperPositionVoteShouldBeDontCare = async () => {
      const vote = await rockhopper.read("Erd_FreshFoodDamperPosition_DamperFreezePreventionVote");
      expect(vote.care).toEqual(voteCare.dontCare);
   };

   const providedTheDamperHeaterIsOnAndDamperIsOpen = async () => {
      await providedTheTargetTemperatureIs(someTargetTemperatureInDegFx100);
      await providedTheDamperFreezePreventionIsInMonitoringTemperatureChange();

      await whenTheDamperOpens();
      await providedTheTargetTemperatureIs(someTargetTemperatureInDegFx100 - (targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));

      await after(minimumTemperatureChangeTimeInMinutes).inMinutes();
      await after(10).inSec();
      await damperHeaterPercentageDutyCycleVoteShouldBe(100);
   };

   const providedTheDamperHeaterIsOnAndDamperIsClosed = async () => {
      await providedTheTargetTemperatureIs(someTargetTemperatureInDegFx100);
      await providedTheDamperFreezePreventionIsInMonitoringTemperatureChange();
      await whenTheDamperOpens();

      await whenTheDamperCloses();
      await providedTheTargetTemperatureIs(someTargetTemperatureInDegFx100 + (targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));

      await after(minimumTemperatureChangeTimeInMinutes).inMinutes();
      await after(10).inSec();
      await damperHeaterPercentageDutyCycleVoteShouldBe(100);
   };

   const providedTheDamperHeaterHasBeenTurnedOffAndDamperVotedOpen = async () => {
      await providedTheDamperHeaterIsOnAndDamperIsClosed();

      await after(damperHeaterOnTimeInMinutes).inMinutes();
      await after(1).inSec();
      await damperHeaterPercentageDutyCycleVoteShouldBe(0);
      await damperPositionShouldBeVoted(damperPosition.open);
   };

   const providedTheDamperHeaterHasBeenTurnedOffAndDamperVotedClosed = async () => {
      await providedTheDamperHeaterIsOnAndDamperIsOpen();

      await after(damperHeaterOnTimeInMinutes).inMinutes();
      await after(1).inSec();
      await damperHeaterPercentageDutyCycleVoteShouldBe(0);
      await damperPositionShouldBeVoted(damperPosition.closed);
   };

   const providedTheDamperPositionHasBeenVotedDontCareAndInMonitoringTemperatureChangeState = async () => {
      await providedTheDamperHeaterHasBeenTurnedOffAndDamperVotedClosed();

      await whenTheDamperCloses();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.monitoringTemperatureChange);
      await damperPositionVoteShouldBeDontCare();
   };

   it("should initialize into Idle when target thermistor is invalid and source thermistor is valid and source temperature is less than max source temperature for freeze prevention", async ()=> {
      await providedTheTargetThermistorIsInvalid();
      await providedTheSourceThermistorIsValid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should initialize into Idle when target thermistor is valid and source thermistor is valid and source temperature is equal to max source temperature for freeze prevention", async ()=> {
      await providedTheTargetThermistorIsValid();
      await providedTheSourceThermistorIsValid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should initialize into Idle when target thermistor is valid and source thermistor is valid and source temperature is greater than max source temperature for freeze prevention", async ()=> {
      await providedTheTargetThermistorIsValid();
      await providedTheSourceThermistorIsValid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 + 1);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should initialize into Idle when target thermistor is valid and source thermistor is invalid and source temperature is less than max source temperature for freeze prevention", async ()=> {
      await providedTheTargetThermistorIsValid();
      await providedTheSourceThermistorIsInvalid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should initialize into Monitoring Temperature Change when target thermistor is valid and source thermistor is valid and source temperature is less than max source temperature for freeze prevention", async ()=> {
      await providedTheTargetThermistorIsValid();
      await providedTheSourceThermistorIsValid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.monitoringTemperatureChange);
   });

   it("should transition from Idle to Monitoring Temperature Change when target thermistor is valid and source temperature is less than max source temperature for freeze prevention and source thermistor is valid", async() => {
      await providedTheTargetThermistorIsInvalid();
      await providedTheSourceThermistorIsValid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);

      await whenTheTargetThermistorIsValid();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.monitoringTemperatureChange);
   });

   it("should transition from Idle to Monitoring Temperature Change when source temperature is less than max source temperature for freeze prevention and target thermistor is valid and source thermistor is valid", async ()=> {
      await providedTheTargetThermistorIsValid();
      await providedTheSourceThermistorIsValid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);

      await whenTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.monitoringTemperatureChange);
   });

   it("should transition from Idle to Monitoring Temperature Change when source thermistor is valid and temperature is less than max source temperature for freeze prevention and target thermistor is valid", async ()=> {
      await providedTheTargetThermistorIsValid();
      await providedTheSourceThermistorIsInvalid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);

      await whenTheSourceThermistorIsValid();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.monitoringTemperatureChange);
   });

   it("should not transition from Idle to Monitoring Temperature Change when source temperature is less than max source temperature for freeze prevention and target thermistor is valid and source thermistor is invalid", async () => {
      await providedTheTargetThermistorIsValid();
      await providedTheSourceThermistorIsInvalid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);

      await whenTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should not transition from Idle to Monitoring Temperature Change when source thermistor changes from invalid to valid and target thermistor is invalid and source temperature is less than max source temperature for freeze prevention", async () => {
      await providedTheTargetThermistorIsInvalid();
      await providedTheSourceThermistorIsInvalid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);

      await whenTheSourceThermistorIsValid();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should not transition from Idle to Monitoring Temperature Change when source temperature is less than max source temperature for freeze prevention and target thermistor is invalid and source thermistor is valid", async () => {
      await providedTheTargetThermistorIsInvalid();
      await providedTheSourceThermistorIsValid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);

      await whenTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should not transition from Idle to Monitoring Temperature Change when target thermistor changes from invalid to valid and source temperature is less than max source temperature for freeze prevention and source thermistor is invalid", async () => {
      await providedTheTargetThermistorIsInvalid();
      await providedTheSourceThermistorIsInvalid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);

      await whenTheTargetThermistorIsValid();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should not transition from Idle to Monitoring Temperature Change when target thermistor changes from invalid to valid and source temperature is equal to max source temperature for freeze prevention and source thermistor is valid", async () => {
      await providedTheTargetThermistorIsInvalid();
      await providedTheSourceThermistorIsValid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);

      await whenTheTargetThermistorIsValid();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should not transition from Idle to Monitoring Temperature Change when source thermistor changes from invalid to valid and source temperature is equal to max source temperature for freeze prevention and target thermistor is valid", async () => {
      await providedTheTargetThermistorIsValid();
      await providedTheSourceThermistorIsInvalid();
      await providedTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      await providedTheBoardHasBeenReset();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);

      await whenTheSourceThermistorIsValid();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should transition from Monitoring Temperature Change to Idle when target thermistor is invalid", async() => {
      await providedTheDamperFreezePreventionIsInMonitoringTemperatureChange();

      await whenTheTargetThermistorIsInvalid();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should transition from Monitoring Temperature Change to Idle when target thermistor is invalid", async() => {
      await providedTheDamperFreezePreventionIsInMonitoringTemperatureChange();

      await whenTheSourceThermistorIsInvalid();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should transition from Monitoring Temperature Change to Idle when source temperature is equal to max source temperature for freeze prevention", async() => {
      await providedTheDamperFreezePreventionIsInMonitoringTemperatureChange();

      await whenTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should transition from Monitoring Temperature Change to Idle when source temperature is greater than max source temperature for freeze prevention", async() => {
      await providedTheDamperFreezePreventionIsInMonitoringTemperatureChange();

      await whenTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 + 1);
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should turn on damper heater when temperature does not decrease by minimum temperature change after damper opens", async() => {
      await providedTheTargetTemperatureIs(someTargetTemperatureInDegFx100);
      await providedTheDamperFreezePreventionIsInMonitoringTemperatureChange();

      await whenTheDamperOpens();
      await whenTheTargetTemperatureIs(someTargetTemperatureInDegFx100 - (targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));

      await after(minimumTemperatureChangeTimeInMinutes).inMinutes();
      await after(10).inSec();
      await damperHeaterPercentageDutyCycleVoteShouldBe(100);
   });

   it("should turn on damper heater when temperature does not increase by minimum temperature change after damper closes", async() => {
      await providedTheTargetTemperatureIs(someTargetTemperatureInDegFx100);
      await providedTheDamperFreezePreventionIsInMonitoringTemperatureChange();
      await whenTheDamperOpens();

      await whenTheDamperCloses();
      await whenTheTargetTemperatureIs(someTargetTemperatureInDegFx100 + (targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));

      await after(minimumTemperatureChangeTimeInMinutes).inMinutes();
      await after(10).inSec();
      await damperHeaterPercentageDutyCycleVoteShouldBe(100);
   });

   it("should turn off damper heater after damper heater on time", async() => {
      await providedTheDamperHeaterIsOnAndDamperIsOpen();

      await after(damperHeaterOnTimeInMinutes).inMinutes();
      await after(1).inSec();
      await damperHeaterPercentageDutyCycleVoteShouldBe(0);
   });

   it("should vote to open damper if damper is closed when damper heater is turned off", async() => {
      await providedTheDamperHeaterIsOnAndDamperIsClosed();

      await after(damperHeaterOnTimeInMinutes).inMinutes();
      await after(1).inSec();
      await damperHeaterPercentageDutyCycleVoteShouldBe(0);
      await damperPositionShouldBeVoted(damperPosition.open);
   });

   it("should vote to close damper if damper is open when damper heater is turned off", async() => {
      await providedTheDamperHeaterIsOnAndDamperIsOpen();

      await after(damperHeaterOnTimeInMinutes).inMinutes();
      await after(1).inSec();
      await damperHeaterPercentageDutyCycleVoteShouldBe(0);
      await damperPositionShouldBeVoted(damperPosition.closed);
   });

   it("should vote don't care for damper position when current damper position changes to open after voting for open position then transition to Monitoring Temperature Change", async() => {
      await providedTheDamperHeaterHasBeenTurnedOffAndDamperVotedOpen();

      await whenTheDamperOpens();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.monitoringTemperatureChange);
      await damperPositionVoteShouldBeDontCare();
   });

   it("should vote don't care for damper position when current damper position changes to open after voting for closed position then transition to Monitoring Temperature Change", async() => {
      await providedTheDamperHeaterHasBeenTurnedOffAndDamperVotedClosed();

      await whenTheDamperCloses();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.monitoringTemperatureChange);
      await damperPositionVoteShouldBeDontCare();
   });

   it("should transition to Idle after voting don't care for damper position if target thermistor is invalid", async() => {
      await providedTheDamperPositionHasBeenVotedDontCareAndInMonitoringTemperatureChangeState();

      await whenTheTargetThermistorIsInvalid();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should transition to Idle after voting don't care for damper position if source thermistor is invalid", async() => {
      await providedTheDamperPositionHasBeenVotedDontCareAndInMonitoringTemperatureChangeState();

      await whenTheSourceThermistorIsInvalid();
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should transition to Idle after voting don't care for damper position if source temperature is equal to max source temperature for freeze prevention", async() => {
      await providedTheDamperPositionHasBeenVotedDontCareAndInMonitoringTemperatureChangeState();

      await whenTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });

   it("should transition to Idle after voting don't care for damper position if source temperature is greater than max source temperature for freeze prevention", async() => {
      await providedTheDamperPositionHasBeenVotedDontCareAndInMonitoringTemperatureChangeState();

      await whenTheSourceTemperatureIs(sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 + 1);
      await theDamperFreezePreventionFsmStateShouldBe(damperFreezePreventionFsmState.idle);
   });
});
