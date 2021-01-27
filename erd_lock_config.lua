return {
  common_erds = {
    error_message = 'You have changed a common ERD to all appliances. Please revert your changes.',
    erds = {
      '0x0001',
      '0x0002',
      '0x0008',
      '0x0030',
      '0x0031',
      '0x0032',
      '0x0035',
      '0x0038',
      '0x0039',
      '0x003A',
      '0x003B',
      '0x003C',
      '0x007F',
      '0x008F',
      '0x0090',
      '0x0092'
    }
  },
  production = {
    error_message = 'Do not change these ERDs. Adding values to an enumeration or giving meaning to unused fields is valid. Make sure to update the erd-specification repo.',
    erds = {
      '0x0700',
      '0x0701',
      '0x0702',
      '0x0703'
    }
  }
}
