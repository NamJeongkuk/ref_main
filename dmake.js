'use strict';

const kpit_rx_base = require('./tools/kpit-rx/dmake');

module.exports = {
  'rockhopper-target.mk': {
    ...kpit_rx_base,
    options: {
      ...kpit_rx_base.options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/rockhopper-boot-loader/build',
        'lib/rockhopper-boot-loader/lib/boot-loader-updater/build'
      ]
    }
  }
}
