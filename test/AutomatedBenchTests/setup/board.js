'use strict';

module.exports = {
  write: (erd, value) => _board.write(erd, value),
  read: (erd) => _board.read(erd),
  print: async (erd) => {
    console.log(`${erd}: ${await _board.read(erd)}`);
  },
  sendSignalViaErd: (erd) => _board.sendSignalViaErd(erd),
  expect: (erd) => {
    return {
      toEqual: async (expected) => {
        const actual = await _board.read(erd);
        expect(actual).toEqual(expected);
      },
      not: {
        toEqual: async (expected) => {
          const actual = await _board.read(erd);
          expect(actual).not.toEqual(expected);
        },
      },
    };
  },
  reset: async () => {
    await _board.sendSignalViaErd('Erd_BroadcastResetRequestSignal');
  },
};
