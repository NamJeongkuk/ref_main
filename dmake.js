'use strict';

const kpit_rx_base = require('./tools/kpit-rx/dmake');
const iar_stm8_base = require('./tools/iar-stm8-wine/dmake');

module.exports = {
  'rx130-target.mk': {
    ...kpit_rx_base,
    options: {
      ...kpit_rx_base.options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/boot-loaders/build',
        'lib/boot-loaders/lib/boot-loader-updater/build'
      ]
    }
  },
  'rx231-target.mk': {
    ...kpit_rx_base,
    options: {
      ...kpit_rx_base.options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/boot-loaders/build',
        'lib/boot-loaders/lib/boot-loader-updater/build'
      ]
    }
  },
  'stm8s-target.mk': {
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
  'stm8s-cap-touch-target.mk': {
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
  'rx130-cap-touch-client-target.mk': {
    ...kpit_rx_base,
    options: {
      ...kpit_rx_base.options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/boot-loaders/build',
        'lib/boot-loaders/lib/boot-loader-updater/build'
      ]
    }
  },
  'mini-rx130-target.mk': {
    ...kpit_rx_base,
    options: {
      ...kpit_rx_base.options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/boot-loaders/build',
        'lib/boot-loaders/lib/boot-loader-updater/build'
      ]
    }
  },
  'mini-rx231-target.mk': {
    ...kpit_rx_base,
    options: {
      ...kpit_rx_base.options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/boot-loaders/build',
        'lib/boot-loaders/lib/boot-loader-updater/build'
      ]
    }
  }
}
