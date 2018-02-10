# RAK811 breakout board

Code from here and tried to adapt to platformio
https://github.com/Ebiroll/RAK811_BreakBoard.git

It compiles and flashes but when I got blinky to run
the  speed was way to low,

# Add new board definition

The earlier versions of the board only had 16K ram so stm32l15xx6.ld is default as linker script.
If you want to use the full 32K ram you can change this line before copy
        "ldscript": "/home/olof/rak/STM32L151XBA_FLASH.ld"
cp rak811.json ~/.platformio/platforms/ststm32/boards/rak811.json


# compile
pio run


# To flash firmware with  stm32flash
In stm32flash_src there is code to flash the device
    ./stm32flash -w .pioenvs/rak811/firmware.bin  /dev/ttyUSB0
To reset and start after download, try
    ./stm32flash  -w .pioenvs/rak811/firmware.bin -v -g  0x0  /dev/ttyUSB0

# Start
It does not apear to tun correctly. :-(
in src/main there is also a blinky main that can be used for debugging.
I was able to run it once, then I made some change that made it unusable


# Startup code
The startup code should have been these,
https://github.com/RAKWireless/RAK811_BreakBoard/tree/master/src/boards/RAK811BreakBoard/cmsis/arm-gcc

boards/RAK811BreakBoard/cmsis/startup_stm32l151xb.s

But instead
.platformio/packages/framework-cmsis/variants/stm32l1/stm32l151xba/startup_stm32l151xba.S
is used,

Only difference is
   /*bl __libc_init_array*/ /* PlatformIO note: Will work without -nostartfiles */