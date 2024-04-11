'use strict';

const board = require('../setup/board');

const states = {
  PrechillPrep: async () => {
    await board.write('Erd_FreezerThermistor_IsValidOverrideRequest', true);
    await board.write('Erd_FreezerThermistor_IsValidOverrideValue', true);
    await board.write('Erd_FreezerThermistor_IsValidResolved', true);

    await board.write('Erd_FreshFoodThermistor_IsValidOverrideRequest', true);
    await board.write('Erd_FreshFoodThermistor_IsValidOverrideValue', true);
    await board.write('Erd_FreshFoodThermistor_IsValidResolved', true);

    await board.write('Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideRequest', true);
    await board.write('Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideValue', true);
    await board.write('Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved', true);

    await board.write('Erd_FreezerEvapThermistor_IsValidOverrideRequest', true);
    await board.write('Erd_FreezerEvapThermistor_IsValidOverrideValue', true);
    await board.write('Erd_FreezerEvapThermistor_IsValidResolved', true);

    await board.write('Erd_FreshFoodEvapThermistor_IsValidOverrideRequest', true);
    await board.write('Erd_FreshFoodEvapThermistor_IsValidOverrideValue', true);
    await board.write('Erd_FreshFoodEvapThermistor_IsValidResolved', true);

    await board.write('Erd_ConvertibleCompartmentEvapThermistor_IsValidOverrideRequest', true);
    await board.write('Erd_ConvertibleCompartmentEvapThermistor_IsValidOverrideValue', true);
    await board.write('Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved', true);

    await board.write('Erd_FreshFood_FilteredTemperatureOverrideRequest', true);
    await board.write('Erd_FreshFood_FilteredTemperatureOverrideValueInDegFx100', 3200 + 1);
    await board.write('Erd_FreshFood_FilteredTemperatureResolvedInDegFx100', 3200 + 1);

    await board.write('Erd_Freezer_FilteredTemperatureOverrideRequest', true);
    await board.write('Erd_Freezer_FilteredTemperatureOverrideValueInDegFx100', -600 + 1);
    await board.write('Erd_Freezer_FilteredTemperatureResolvedInDegFx100', -600 + 1);

    await board.write('Erd_ConvertibleCompartmentCabinet_FilteredTemperatureOverrideRequest', true);
    await board.write(
      'Erd_ConvertibleCompartmentCabinet_FilteredTemperatureOverrideValueInDegFx100',
      3201
    );
    await board.write(
      'Erd_ConvertibleCompartmentCabinet_FilteredTemperatureResolvedInDegFx100',
      3201
    );

    await board.write('Erd_DefrostTestRequest', 'DefrostTestRequest_Prechill');
    await board.write('Erd_CoolingMode', 'CoolingMode_Freezer');

    await board.expect('Erd_DefrostHsmState').toEqual('DefrostHsmState_PrechillPrep');
  },
};

const isInState = async (state) => {
  return await states[state]();
};

const given = { isInState };

module.exports = {
  given,
  provided: given,
  when: given,
};
