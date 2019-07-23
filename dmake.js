'use strict';

module.exports = {
  'dugnutt-target.mk': {
    ...require('./Tools/kpit-rx/dmake'),
    options: {
      ...require('./Tools/kpit-rx/dmake').options,
      outputDirectories: [
        'build',
        'artifacts'
      ]
    }
  }
}
