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
  }
}
