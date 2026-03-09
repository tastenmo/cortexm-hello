set pagination off
set breakpoint pending on
set mem inaccessible-by-default off

define bmp_attach
  target extended-remote /dev/ttyACM0
  monitor tpwr enable
  monitor swd_scan
  attach 1
  monitor halt
end

document bmp_attach
Attach to RP2040 core 0 through Black Magic Probe with target power enabled.
end

define bmp_flash
  bmp_attach
  load
  monitor halt
end

document bmp_flash
Attach and flash the current ELF, then halt.
end

define bmp_main
  bmp_flash
  break main
end

document bmp_main
Attach, flash, and set a breakpoint at main.
end