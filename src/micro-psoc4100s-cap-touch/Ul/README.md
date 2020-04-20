Place buffers that cannot be checked by the UL RAM Checks (such as buffers used by DMA) into `.noUlRamChecks`. Example:
```c
static uint8_t receiveBuffer[ReceiveBufferSize] __attribute__ ((section (".noUlRamChecks")));
```

Then place the section at the start of the RAM by pasting this code into the linker file above the `.data` section:

```
.noUlRamChecks (NOLOAD) :
{
  __no_ul_ram_checks_start = .;
  *(.noUlRamChecks)
  __no_ul_ram_checks_end = .;
} >ram
```

Note: Any RAM that is moved into this section is no longer covered by Cypress startup code. That means that if a buffer needs zeroed out, the module must do it manually, since the startup code will no longer do it. Also, `static bool testing = true __attribute__ ((section (".noUlRamChecks")));`, is not valid since `testing` will not be initialized to true, the initial value is undefined. Set it to `true` during the module's initialization call.
