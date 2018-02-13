# RAK811 breakout board

Code from here 
https://github.com/RAKWireless/RAK811_BreakBoard
then adapted to platformio

# Add new board definition

The earlier versions of the board only had 16K ram so stm32l15xx6.ld is default as linker script.
If you want to use the full 32K ram you can change this line before copy
        "ldscript": "/home/olof/rak/STM32L151XBA_FLASH.ld"

If you use, platformio ide, press

    1) Home button
    2) Platforms/ST STM32 button

![instruction](instruction.png)


    cp rak811.json ~/.platformio/platforms/ststm32/boards/rak811.json

If you do not have this directory, .platformio/platforms/ststm32 try, 
    
    pio platform install ststm32


# compile
    
    pio run


# To flash firmware with  stm32flash
In stm32flash_src there is code to flash the device. Build with make

    ./stm32flash -w .pioenvs/rak811/firmware.bin  /dev/ttyUSB0
To reset and start after download, try

    ./stm32flash  -w .pioenvs/rak811/firmware.bin -v -g  0x0  /dev/ttyUSB0

# Start
Now it starts and outputs debug information on UART,

    Move Detected INT1 src:0x48
    [Debug]: tempr: 35 Bat: 3988mv
    [Debug]: ACC X:FF00 Y:0000 Z:FF00
    GpsGetLatestGpsPositionDouble ret = 0


# Startup code
The startup code should have been this,
https://github.com/RAKWireless/RAK811_BreakBoard/tree/master/src/boards/RAK811BreakBoard/cmsis/arm-gcc/boards/RAK811BreakBoard/cmsis/startup_stm32l151xb.s

But instead
    .platformio/packages/framework-cmsis/variants/stm32l1/stm32l151xba/startup_stm32l151xba.S
is used,

Only difference is
    /*bl __libc_init_array*/ /* PlatformIO note: Will work without -nostartfiles */
Probably not important