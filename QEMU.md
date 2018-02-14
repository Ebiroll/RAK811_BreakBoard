# Running the RAK board in qeum
Clone and build this
https://github.com/Ebiroll/qemu-xtensa-esp32


./qemu-system-arm   -serial file:uart1.log  -d unimp  -serial tcp::12344,server,nowait -serial tcp::12345,server,nowait -monitor stdio -machine pebble-bb2  -cpu cortex-m3
 -S -s  -pflash .pioenvs/rak811/firmware.bin