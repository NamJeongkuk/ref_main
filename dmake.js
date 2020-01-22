'use strict';

const kpit_rx_base = require('./tools/kpit-rx/dmake');
const iar_stm8_base = require('./tools/iar-stm8-wine/dmake');

module.exports = {
  'dugnutt-target.mk': {
    ...kpit_rx_base,
    options: {
      ...kpit_rx_base.options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/boot-loaders/build'
      ]
    }
  },
  'truk-target.mk': {
    ...kpit_rx_base,
    options: {
      ...kpit_rx_base.options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/boot-loaders/build'
      ]
    }
  },
  'bonzalez-target.mk': {
    ...iar_stm8_base,
    options: {
      ...iar_stm8_base.options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/boot-loaders/build'
      ]
    }
  },
  'chamgerlain-target.mk': {
    ...iar_stm8_base,
    options: {
      ...iar_stm8_base.options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/boot-loaders/build'
      ]
    }
  },
  'wesrey-target.mk': {
    ...kpit_rx_base,
    options: {
      ...kpit_rx_base.options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/boot-loaders/build'
      ]
    }
  }
}
