return {
  Application = {
    ram = {
      map = 'build/chamgerlain/chamgerlain.map',
      type = 'sdcc',
      total = 2 * 1024
    },
    rom = {
      map = 'build/chamgerlain/chamgerlain.map',
      type = 'sdcc',
      total = (32 * 1024) - (1024 + 64)
    }
  }
}
