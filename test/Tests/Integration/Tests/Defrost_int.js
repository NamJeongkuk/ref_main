"use strict";

const delay = require("javascript-common").util.delay;
const After = require('../support/After.js');
const { msPerSec, msPerMin, secondsPerMin, on, off } = require("../support/constants");
const constants = require("../support/constants");

describe("Defrost", () => {
  const defrostHsmState = {
    defrostHsmStatePowerUp: "DefrostHsmState_PowerUp",
    defrostHsmStateIdle: "DefrostHsmState_Idle",
    defrostHsmStatePrechillPrep: "DefrostHsmState_PrechillPrep",
    defrostHsmStatePrechill: "DefrostHsmState_Prechill",
    defrostHsmStateHeaterOnEntry: "DefrostHsmState_HeaterOnEntry",
    defrostHsmStateHeaterOn: "DefrostHsmState_HeaterOn",
    defrostHsmStateDwell: "DefrostHsmState_Dwell",
    defrostHsmStatePostDwell: "DefrostHsmState_PostDwell",
    defrostHsmStateDisabled: "DefrostHsmState_Disabled"
  };

  const defrostState = {
    defrostStateIdle: "DefrostState_Idle",
    defrostStatePrechill: "DefrostState_Prechill",
    defrostStateHeaterOn: "DefrostState_HeaterOn",
    defrostStateDwell: "DefrostState_Dwell",
    defrostStatePostDwell: "DefrostState_PostDwell",
    defrostStateDisabled: "DefrostState_Disabled"
  };

  const compressorState = {
    compressorStateOffAndReadyToChange: "CompressorState_OffAndReadyToChange",
    compressorStateOnAndReadyToChange: "CompressorState_OnAndReadyToChange",
    compressorStateMinimumOnTime: "CompressorState_MinimumOnTime",
    compressorStateMinimumOffTime: "CompressorState_MinimumOffTime"
  };

  const compressorSpeed = {
    compressorSpeedOff: "CompressorSpeed_Off",
    compressorSpeedOn: "CompressorSpeed_On",
    compressorSpeedSuperLow: "CompressorSpeed_SuperLow",
    compressorSpeedLow: "CompressorSpeed_Low",
    compressorSpeedMedium: "CompressorSpeed_Medium",
    compressorSpeedHigh: "CompressorSpeed_High",
    compressorSpeedSuperHigh: "CompressorSpeed_SuperHigh"
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

  const damperPosition = {
    damperPositionClosed: "DamperPosition_Closed",
    damperPositionOpen: "DamperPosition_Open"
  };

  const heaterState = {
    heaterStateOff: "HeaterState_Off",
    heaterStateOn: "HeaterState_On"
  };

  const voteState = {
    voteCare: "Vote_Care",
    voteDontCare: "Vote_DontCare"
  };

  const powerUpDelayInMsec = 5 * constants.msPerSec;

  //minimum_time_between_defrosts_abnormal_run_time_in_minutes * seconds_per_minute
  //6 * 60 = 3600
  const minimumDefrostReadySatisfactionTimeInMinutes = 6 * 60;
  const minimumDefrostReadyOnTimeInSec =
    minimumDefrostReadySatisfactionTimeInMinutes * 60;
  const maxTimeBetweenDefrostsInMinutes = 32 * 60;

  const somePrechillConditionTimeInMinutes = 3;
  const someTemperatureThatLiesInTheMiddleOfTheBounds = 4000;
  const someLowerTemperatureBound = 0;
  const someEepromDefrostHeaterOnTime = 1;
  const defrostHeaterOnDelayAfterCompressorOffInSeconds = 2;
  const freezerDefrostTerminationTemperatureInDegfx100 = 5900;
  const freezerDefrostHeaterMaxOnTimeInMinutes = 60;
  const dwellFreshFoodDamperPosition = damperPosition.damperPositionClosed;
  const dwellTimeInMinutes = 7;
  const dwellTimeInSeconds = dwellTimeInMinutes * constants.secondsPerMin + 1;
  const postDwellTimeInMinutes = 10;

  const postDwellFreezerEvapExitTemperatureInDegFx100 = -1000;
  const postDwellCompressorSpeed = compressorSpeed.compressorSpeedLow;
  const postDwellDamperPosition = damperPosition.damperPositionClosed;

  beforeEach(async () => {
    await rockhopper.write("Erd_Reset", 1);
    await delay(constants.oneSecondInMsec * 5);
  });

  const providedTheFilteredFreezerCabinetTemperatureIs = async (tempDegFx100) => {
    await rockhopper.write("Erd_Freezer_FilteredTemperatureOverrideRequest", true);
    await rockhopper.write("Erd_Freezer_FilteredTemperatureOverrideValueInDegFx100", tempDegFx100);
  };

  const providedTheFilteredFreezerEvaporatorTemperatureIs = async (tempDegFx100) => {
    await rockhopper.write("Erd_FreezerEvap_FilteredTemperatureOverrideRequest", true);
    await rockhopper.write("Erd_FreezerEvap_FilteredTemperatureOverrideValueInDegFx100", tempDegFx100);
  }

  const providedTheEepromDefrostStateAtStartUpIs = async (state) => {
    await rockhopper.write("Erd_DefrostState", state);
    await after(powerUpDelayInMsec).inMsec();
    await rockhopper.write("Erd_Reset", 2);
    await after(powerUpDelayInMsec).inMsec();
  };

  const providedDefrostIsEnabledAndInIdleState = async () => {
    await providedTheFilteredFreezerCabinetTemperatureIs(300);
    await theCompressorStateIs(compressorState.compressorStateMinimumOnTime);

    await providedTheEepromDefrostStateAtStartUpIs(defrostState.defrostStateIdle);
    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateIdle);
  };

  const providedDefrostIsEnabledAndInHeaterOnEntryState = async () => {
    await providedTheEepromDefrostStateAtStartUpIs(defrostState.defrostStateHeaterOn);
    await providedTheFilteredResolvedTemperatureFor().TheFreezerEvapThermistorIs(freezerDefrostTerminationTemperatureInDegfx100 - 1);

    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateHeaterOnEntry);
  };

  const providedDefrostIsEnabledAndInHeaterOnState = async () => {
    await providedDefrostIsEnabledAndInHeaterOnEntryState();

    await after(defrostHeaterOnDelayAfterCompressorOffInSeconds).inSec();
    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateHeaterOn);
  };

  const providedDefrostIsEnabledAndInDwellState = async () => {
    await providedTheFilteredFreezerCabinetTemperatureIs(300);

    await providedTheEepromDefrostStateAtStartUpIs(defrostState.defrostStateDwell);
    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateDwell);
  };

  const providedDefrostIsEnabledAndInPostDwellState = async () => {
    await providedTheFilteredFreezerCabinetTemperatureIs(300);

    await providedTheEepromDefrostStateAtStartUpIs(defrostState.defrostStateDwell);
    await after(dwellTimeInMinutes).inMinutes();
    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePostDwell);
  };

  const providedTheDefrostReadySatisfactionTimeIs = async (timeInMinutes) => {
    await rockhopper.write("Erd_TimeInMinutesUntilReadyToDefrost", timeInMinutes);
  };

  const disableMinimumCompressorTimesShouldBe = async (state) => {
    const actual = await rockhopper.read("Erd_DisableMinimumCompressorTimes");
    expect(actual).toEqual(state);
  }

  const theDefrostHsmStateShouldBe = async (state) => {
    const actual = await rockhopper.read("Erd_DefrostHsmState");
    expect(actual).toEqual(state);
  };

  const theFreezerDefrostHeaterRelayShouldBe = async (state) => {
    const actual = await rockhopper.read("Erd_FreezerDefrostHeaterRelay");
    expect(actual).toEqual(state);
  };

  const theCompressorStateIs = async (state) => {
    await rockhopper.write("Erd_CompressorState", state);
  };

  const after = (time) => ({
    inMsec: async () => {
      await After(time);
    },
    inSec: async () => {
      await After(time * msPerSec);
    },
    inMinutes: async () => {
      await After(time * msPerMin);
    },
  });

  const providedThereWasNoPreviousAbnormalDefrost = async () => {
    await rockhopper.write("Erd_FreezerDefrostWasAbnormal", false);
    await rockhopper.write("Erd_FreshFoodDefrostWasAbnormal", false);
    await rockhopper.write("Erd_ConvertibleCompartmentDefrostWasAbnormal", false);
  };

  const providedFreezerEvaporatorThermistorIs = () => ({
    valid: async () => {
      await rockhopper.write("Erd_FreezerEvapThermistor_IsValidOverrideRequest", true);
      await rockhopper.write("Erd_FreezerEvapThermistor_IsValidOverrideValue", true);
    },
    invalid: async () => {
      await rockhopper.write("Erd_FreezerEvapThermistor_IsValidOverrideValue", false);
      await rockhopper.write("Erd_FreezerEvapThermistor_IsValidOverrideRequest", false);
    }
  });

  const providedFreshFoodEvaporatorThermistorIs = () => ({
    valid: async () => {
      await rockhopper.write("Erd_FreshFoodEvapThermistor_IsValidOverrideRequest", true);
      await rockhopper.write("Erd_FreshFoodEvapThermistor_IsValidOverrideValue", true);
    },
    invalid: async () => {
      await rockhopper.write("Erd_FreshFoodEvapThermistor_IsValidOverrideValue", false);
      await rockhopper.write("Erd_FreshFoodEvapThermistor_IsValidOverrideRequest", false);
    }
  });

  const providedFreezerThermistorIs = () => ({
    valid: async () => {
      await rockhopper.write("Erd_FreezerThermistor_IsValidOverrideRequest", true);
      await rockhopper.write("Erd_FreezerThermistor_IsValidOverrideValue", true);
    },
    invalid: async () => {
      await rockhopper.write("Erd_FreezerThermistor_IsValidOverrideValue", false);
      await rockhopper.write("Erd_FreezerThermistor_IsValidOverrideRequest", false);
    }
  });

  const providedFreshFoodThermistorIs = () => ({
    valid: async () => {
      await rockhopper.write("Erd_FreshFoodThermistor_IsValidOverrideRequest", true);
      await rockhopper.write("Erd_FreshFoodThermistor_IsValidOverrideValue", true);
    },
    invalid: async () => {
      await rockhopper.write("Erd_FreshFoodThermistor_IsValidOverrideValue", false);
      await rockhopper.write("Erd_FreshFoodThermistor_IsValidOverrideRequest", false);
    }
  });

  const providedTheCompressor = () => ({
    isOn: async () => {
      await rockhopper.write("Erd_CompressorIsOn", true);
    },
    isOff: async () => {
      await rockhopper.write("Erd_CompressorIsOn", false);
    }
  });

  const providedTheCoolingModeIs = async (coolingMode) => {
    await rockhopper.write("Erd_CoolingMode", coolingMode);
  };

  const providedCompartmentWasNotTooWarmOnPowerUp = async () => {
    await rockhopper.write("Erd_FreezerFilteredTemperatureTooWarmAtPowerUp", false);
  };

  const providedTimeThatPrechillConditionsAreMetIs = async (timeInMinutes) => {
    await rockhopper.write("Erd_TimeThatPrechillConditionsAreMetInMinutes", timeInMinutes);
  };

  const whenTheCompressorHasBeenOnFor = (timeInSec) => ({
    seconds: async () => {
      await rockhopper.write("Erd_DefrostCompressorOnTimeInSeconds", timeInSec);
    }
  });

  const whenTheDoorAccelerationTimeForThe = () => ({
    FreshFoodCompartmentIs: async (timeInSec) => {
      await rockhopper.write("Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds", timeInSec);
    },
    FreezerCompartmentIs: async (timeInSec) => {
      await rockhopper.write("Erd_DefrostFreezerScaledDoorAccelerationInSeconds", timeInSec);
    },
    ConvertibleCompartmentIs: async (timeInSec) => {
      await rockhopper.write("Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds", timeInSec);
    }
  });

  const providedTheFilteredResolvedTemperatureFor = () => ({
    TheFreshFoodThermistorIs: async (temperature) => {
      await rockhopper.write("Erd_FreshFood_FilteredTemperatureResolvedInDegFx100", temperature);
    },
    TheFreezerThermistorIs: async (temperature) => {
      await rockhopper.write("Erd_Freezer_FilteredTemperatureResolvedInDegFx100", temperature);
    },
    TheFreezerEvapThermistorIs: async (temperature) => {
      await rockhopper.write("Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100", temperature);
    }
  });

  const providedTheEepromDefrostHeaterOnTimeForThe = () => ({
    FreezerDefrostHeaterIs: async (timeInMinutes) => {
      await rockhopper.write("Erd_Eeprom_FreezerDefrostHeaterOnTimeInMinutes", timeInMinutes);
    }
  });

  const theHeaterOnTimeForThe = () => ({
    FreezerDefrostHeaterShouldBe: async (expected) => {
      const actual = await rockhopper.read("Erd_FreezerDefrostHeaterOnTimeInMinutes");
      expect(actual).toEqual(expected);
    },
  });

  const theVoteFor = () => ({
    CompressorSpeedShouldBe: async (expectedCare, expectedSpeed = null) => {
      const actual = await rockhopper.read("Erd_CompressorSpeed_DefrostVote");
      if (expectedSpeed != null) {
        expect(actual.speed).toEqual(expectedSpeed);
      }
      expect(actual.care).toEqual(expectedCare);
    },
    FreezerEvapFanSpeedShouldBe: async (expectedCare, expectedSpeed = null) => {
      const actual = await rockhopper.read("Erd_FreezerEvapFanSpeed_DefrostVote");
      if (expectedSpeed != null) {
        expect(actual.speed).toEqual(expectedSpeed);
      }
      expect(actual.care).toEqual(expectedCare);
    },
    CondenserFanSpeedShouldBe: async (expectedCare, expectedSpeed = null) => {
      const actual = await rockhopper.read("Erd_CondenserFanSpeed_DefrostVote");
      if (expectedSpeed != null) {
        expect(actual.speed).toEqual(expectedSpeed);
      }
      expect(actual.care).toEqual(expectedCare);
    },
    DamperPositionShouldBe: async (expectedCare, expectedPosition = null) => {
      const actual = await rockhopper.read("Erd_FreshFoodDamperPosition_DefrostVote");
      if (expectedPosition != null) {
        expect(actual.position).toEqual(expectedPosition);
      }
      expect(actual.care).toEqual(expectedCare);
    },
    FreezerDefrostHeaterShouldBe: async (expectedCare, expectedState = null) => {
      const actual = await rockhopper.read("Erd_FreezerDefrostHeater_DefrostVote");
      if (expectedState != null) {
        expect(actual.state).toEqual(expectedState);
      }
      expect(actual.care).toEqual(expectedCare);
    },
    IceCabinetFanSpeedShouldBe: async (expectedCare, expectedSpeed = null) => {
      const actual = await rockhopper.read("Erd_IceCabinetFanSpeed_DefrostVote");
      if (expectedSpeed != null) {
        expect(actual.speed).toEqual(expectedSpeed);
      }
      expect(actual.care).toEqual(expectedCare);
    }
  });

  const theFreezerDefrostHeaterResolvedVoteShouldBe = () => ({
    OnAndCare: async () => {
      const expected = { state: heaterState.heaterStateOn, care: voteState.voteCare };
      const actual = await rockhopper.read("Erd_FreezerDefrostHeater_ResolvedVote");
      expect(actual).toEqual(expected);
    },
    OffAndCare: async () => {
      const expected = { state: heaterState.heaterStateOff, care: voteState.voteCare };
      const actual = await rockhopper.read("Erd_FreezerDefrostHeater_ResolvedVote");
      expect(actual).toEqual(expected);
    },
    DontCare: async () => {
      const expected = voteState.voteDontCare;
      const actual = await rockhopper.read("Erd_FreezerDefrostHeater_ResolvedVote");
      expect(actual.care).toEqual(expected);
    }
  });

  const providedTheFreezerDefrostHeaterResolvedVoteIs = () => ({
    OnAndDontCare: async (temperature) => {
      const requestedVote = { state: heaterState.heaterStateOn, care: voteState.voteDontCare };
      await rockhopper.write("Erd_FreezerDefrostHeater_ResolvedVote", requestedVote);
    },
    OffAndDontCare: async (temperature) => {
      const requestedVote = { state: heaterState.heaterStateOff, care: voteState.voteDontCare };
      await rockhopper.write("Erd_FreezerDefrostHeater_ResolvedVote", requestedVote);
    }
  });

  it("should enter Idle state when in Idle before power cycle and FZ temp is normal", async () => {
    await providedDefrostIsEnabledAndInIdleState();
  });

  it("should transition to Prechill Prep when defrost timer is satisfied at max time between defrosts", async () => {
    await providedDefrostIsEnabledAndInIdleState();

    await providedFreshFoodEvaporatorThermistorIs().valid();
    await providedFreezerEvaporatorThermistorIs().valid();
    await providedFreshFoodThermistorIs().valid();
    await providedThereWasNoPreviousAbnormalDefrost();
    await providedCompartmentWasNotTooWarmOnPowerUp();
    await providedTheCompressor().isOff();

    await providedTheDefrostReadySatisfactionTimeIs(minimumDefrostReadySatisfactionTimeInMinutes);
    await whenTheCompressorHasBeenOnFor(minimumDefrostReadyOnTimeInSec).seconds();

    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
  });

  it("should transition to Prechill Prep state earlier with door acceleration added to the defrost timer count", async () => {
    await providedDefrostIsEnabledAndInIdleState();

    await providedThereWasNoPreviousAbnormalDefrost();
    await providedCompartmentWasNotTooWarmOnPowerUp();
    await providedFreshFoodThermistorIs().valid();
    await providedTheCompressor().isOff();
    await providedFreshFoodEvaporatorThermistorIs().valid();
    await providedFreezerEvaporatorThermistorIs().valid();

    await providedTheDefrostReadySatisfactionTimeIs(minimumDefrostReadySatisfactionTimeInMinutes * 3);
    await whenTheCompressorHasBeenOnFor(minimumDefrostReadyOnTimeInSec).seconds();
    await whenTheDoorAccelerationTimeForThe().FreshFoodCompartmentIs(minimumDefrostReadyOnTimeInSec);
    await whenTheDoorAccelerationTimeForThe().FreezerCompartmentIs(minimumDefrostReadyOnTimeInSec);
    await whenTheDoorAccelerationTimeForThe().ConvertibleCompartmentIs(minimumDefrostReadyOnTimeInSec);

    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
  });

  it("should transition to heater on entry state when FZ evap thermistor is invalid upon entry to Prechill Prep", async () => {
    await providedDefrostIsEnabledAndInIdleState();

    await providedThereWasNoPreviousAbnormalDefrost();
    await providedCompartmentWasNotTooWarmOnPowerUp();
    await providedFreshFoodThermistorIs().valid();
    await providedTheCompressor().isOff();
    await providedFreshFoodEvaporatorThermistorIs().valid();
    await providedFreezerEvaporatorThermistorIs().invalid();

    await providedTheDefrostReadySatisfactionTimeIs(minimumDefrostReadySatisfactionTimeInMinutes);
    await whenTheCompressorHasBeenOnFor(minimumDefrostReadyOnTimeInSec).seconds();

    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateHeaterOnEntry);

    await theVoteFor().CompressorSpeedShouldBe(true, compressorSpeed.compressorSpeedOff);
    await theVoteFor().FreezerEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);
    await theVoteFor().CondenserFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);
  });

  it("should transition from prechill prep to prechill and vote loads", async () => {
    await providedDefrostIsEnabledAndInIdleState();

    await providedThereWasNoPreviousAbnormalDefrost();
    await providedCompartmentWasNotTooWarmOnPowerUp();
    await providedFreshFoodThermistorIs().valid();
    await providedFreezerEvaporatorThermistorIs().valid();

    await providedTheFilteredResolvedTemperatureFor().TheFreshFoodThermistorIs(someTemperatureThatLiesInTheMiddleOfTheBounds);
    await providedTheFilteredResolvedTemperatureFor().TheFreezerThermistorIs(someLowerTemperatureBound);
    await providedTheFilteredResolvedTemperatureFor().TheFreezerEvapThermistorIs(someLowerTemperatureBound);

    await providedTheCompressor().isOn();
    await whenTheCompressorHasBeenOnFor(maxTimeBetweenDefrostsInMinutes * secondsPerMin).seconds();
    await providedTheCoolingModeIs(coolingMode.coolingModeFreezer);

    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechill);

    await theVoteFor().CompressorSpeedShouldBe(voteState.voteCare, compressorSpeed.compressorSpeedLow);
    await theVoteFor().FreezerEvapFanSpeedShouldBe(voteState.voteCare, fanSpeed.fanSpeedSuperLow);
    await theVoteFor().DamperPositionShouldBe(voteState.voteCare, damperPosition.damperPositionClosed);
  });

  it("should transition from heater on entry to heater on after defrost heater on delay after compressor is off", async () => {
    await providedDefrostIsEnabledAndInIdleState();

    await providedThereWasNoPreviousAbnormalDefrost();
    await providedCompartmentWasNotTooWarmOnPowerUp();
    await providedFreshFoodThermistorIs().valid();
    await providedTheCompressor().isOff();
    await providedFreshFoodEvaporatorThermistorIs().valid();
    await providedFreezerEvaporatorThermistorIs().invalid();

    await providedTheDefrostReadySatisfactionTimeIs(minimumDefrostReadySatisfactionTimeInMinutes);
    await whenTheCompressorHasBeenOnFor(minimumDefrostReadyOnTimeInSec).seconds();

    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateHeaterOnEntry);

    await after(defrostHeaterOnDelayAfterCompressorOffInSeconds).inSec();

    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateHeaterOn);
  });

  it("should not count heater on time when heater is off", async () => {
    await providedDefrostIsEnabledAndInIdleState();

    await after(1).inMinutes();
    await theHeaterOnTimeForThe().FreezerDefrostHeaterShouldBe(0);

    await after(1).inMinutes();
    await theHeaterOnTimeForThe().FreezerDefrostHeaterShouldBe(0);
  });

  it("should transition from heater on to dwell and reset heater on timer when freezer evaporator thermistor is valid and freezer heater max on time reached", async () => {
    await providedTheEepromDefrostHeaterOnTimeForThe().FreezerDefrostHeaterIs(0);
    await providedDefrostIsEnabledAndInHeaterOnState();
    await providedFreezerEvaporatorThermistorIs().valid();

    await after(freezerDefrostHeaterMaxOnTimeInMinutes).inMinutes();
    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateDwell);
    await theHeaterOnTimeForThe().FreezerDefrostHeaterShouldBe(0);
  });

  it("should continue counting heater on time from eeprom stored time after resetting in heater on state", async () => {
    await providedTheEepromDefrostHeaterOnTimeForThe().FreezerDefrostHeaterIs(someEepromDefrostHeaterOnTime);
    await providedDefrostIsEnabledAndInHeaterOnEntryState();
    await providedFreezerEvaporatorThermistorIs().valid();

    await after((freezerDefrostHeaterMaxOnTimeInMinutes - someEepromDefrostHeaterOnTime) * secondsPerMin + defrostHeaterOnDelayAfterCompressorOffInSeconds).inSec();
    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateDwell);
  });

  it("should set disable minimum compressor on times when entering heater on entry state", async () => {
    await providedDefrostIsEnabledAndInHeaterOnEntryState();
    await disableMinimumCompressorTimesShouldBe(true);
  });

  it("should vote to turn off freezer defrost heater, compressor, and all fans and vote for parametric position for fresh food damper when entering dwell", async () => {
    await providedDefrostIsEnabledAndInDwellState();

    await theVoteFor().FreezerDefrostHeaterShouldBe(true, heaterState.heaterStateOff);
    await theVoteFor().CompressorSpeedShouldBe(true, compressorSpeed.compressorSpeedOff);
    await theVoteFor().FreezerEvapFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);
    await theVoteFor().CondenserFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);
    await theVoteFor().IceCabinetFanSpeedShouldBe(true, fanSpeed.fanSpeedOff);

    await theVoteFor().DamperPositionShouldBe(true, dwellFreshFoodDamperPosition);
  });

  it("should transition to post dwell after dwell time has passed", async () => {
    await providedDefrostIsEnabledAndInDwellState();

    await after(dwellTimeInSeconds).inSec();
    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePostDwell);
  });

  it("should vote to set compressor, condenser fan and damper position and disable minimum compressor times when entering post dwell", async () => {
    await providedDefrostIsEnabledAndInPostDwellState();

    await theVoteFor().CompressorSpeedShouldBe(true, postDwellCompressorSpeed);
    await theVoteFor().CondenserFanSpeedShouldBe(true, fanSpeed.fanSpeedLow);
    await theVoteFor().DamperPositionShouldBe(true, postDwellDamperPosition);
  });

  it("should transition to idle when freezer evaporator temperature is less than or equal to the exit temperature", async () => {
    await providedDefrostIsEnabledAndInPostDwellState();

    await providedTheFilteredFreezerEvaporatorTemperatureIs(postDwellFreezerEvapExitTemperatureInDegFx100 - 1);
    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateIdle);
  });

  it("should transition to idle after post dwell time has passed", async () => {
    await providedDefrostIsEnabledAndInPostDwellState();

    await after(postDwellTimeInMinutes).inMinutes();
    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateIdle);
  });

  it("should vote don't care to freezer defrost heater, compressor, fresh food damper and all fans and enable minimum compressor times when exiting post dwell", async () => {
    await providedDefrostIsEnabledAndInPostDwellState();

    await providedTheFilteredFreezerEvaporatorTemperatureIs(postDwellFreezerEvapExitTemperatureInDegFx100 - 1);
    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateIdle);

    await theVoteFor().FreezerDefrostHeaterShouldBe(false);
    await theVoteFor().CompressorSpeedShouldBe(false);
    await theVoteFor().DamperPositionShouldBe(false);
    await theVoteFor().CondenserFanSpeedShouldBe(false);
    await theVoteFor().FreezerEvapFanSpeedShouldBe(false);
    await theVoteFor().IceCabinetFanSpeedShouldBe(false);

    await disableMinimumCompressorTimesShouldBe(false);
  });

  it("should update freezer defrost heater relay erd when freezer defrost heater resolved vote erd changes", async () => {
    await providedTheEepromDefrostHeaterOnTimeForThe().FreezerDefrostHeaterIs(0);
    await providedDefrostIsEnabledAndInHeaterOnState();
    await providedFreezerEvaporatorThermistorIs().valid();

    await theFreezerDefrostHeaterResolvedVoteShouldBe().OnAndCare();
    await theFreezerDefrostHeaterRelayShouldBe(on);

    await providedTheFilteredFreezerEvaporatorTemperatureIs(freezerDefrostTerminationTemperatureInDegfx100);

    await theFreezerDefrostHeaterResolvedVoteShouldBe().OffAndCare();
    await theFreezerDefrostHeaterRelayShouldBe(off);
  });

  it("should not update freezer defrost heater relay erd when freezer defrost heater resolved vote erd is dont care", async () => {
    await providedTheEepromDefrostHeaterOnTimeForThe().FreezerDefrostHeaterIs(0);
    await providedDefrostIsEnabledAndInHeaterOnState();
    await providedFreezerEvaporatorThermistorIs().valid();

    await theFreezerDefrostHeaterResolvedVoteShouldBe().OnAndCare();
    await theFreezerDefrostHeaterRelayShouldBe(on);

    await providedTheFilteredFreezerEvaporatorTemperatureIs(freezerDefrostTerminationTemperatureInDegfx100);

    await theFreezerDefrostHeaterResolvedVoteShouldBe().OffAndCare();
    await theFreezerDefrostHeaterRelayShouldBe(off);

    await after(dwellTimeInSeconds).inSec();
    await providedTheFreezerDefrostHeaterResolvedVoteIs().OnAndDontCare();
    await theFreezerDefrostHeaterResolvedVoteShouldBe().DontCare();
    await theFreezerDefrostHeaterRelayShouldBe(off);

    await providedTheFreezerDefrostHeaterResolvedVoteIs().OffAndDontCare();
    await theFreezerDefrostHeaterResolvedVoteShouldBe().DontCare();
    await theFreezerDefrostHeaterRelayShouldBe(off);
  });
});
