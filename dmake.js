'use strict';

const kpit_rx_base = require('./tools/kpit-rx/dmake');
const kpit_rl78_base = require('./tools/kpit-rl78/dmake');
const llvm_rl78_base = require('./tools/llvm-rl78/dmake');

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
  'micro-rl78g13-target.mk': {
    ...kpit_rl78_base,
    options: {
      ...kpit_rl78_base.options,
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
  },
  'nano-rl78g12-target.mk': {
    ...llvm_rl78_base,
    options: {
      ...llvm_rl78_base.options,
      outputDirectories: [
        'build',
        'artifacts'
      ]
    }
  },
  'nano-rl78g13-target.mk': {
    ...llvm_rl78_base,
    options: {
      ...llvm_rl78_base.options,
      outputDirectories: [
        'build',
        'artifacts'
      ]
    }
  }
}
