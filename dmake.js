'use strict';

const kpit_rx_base = require('./tools/kpit-rx/dmake');

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
  },
  'micro-rl78-target.mk': {
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
  'micro-rx130-target.mk': {
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
