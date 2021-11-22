# LDS-006 Custom Firmware
This is a (currently very early) attempt at writing an alternate firmware for the LDS-006 LIDAR.

Information about the module can be found [here](https://0x416c6578.github.io/lds-006/overview.html).

This project uses the PlatformIO build system, with most of the setup taken from [this repo](https://github.com/maxgerhardt/pio-gd32f130c6) (huge thanks to the original developer, this saved a lot of headache). I made a custom configuration for the GD32F130F6P6, but most things stayed the similar to the original project.

## Other Work
User `opravdin` has done some nice work in finding the protocol and implementing a controller for the motor, see [here](https://github.com/opravdin/lds-006-reverse-engineering).

## Building
- Make sure you have the VSCode PlatformIO extension installed
- To build, just run PlatformIO->Build

## Uploading
You need an ST-Link to flash the firmware. Connect the ST-Link to the debug header to the right of the GD32 CPU. The pinout is GND->CLK->DATA->3v3 (GND is the pad with the star shape that connects it to the ground plane **TODO: Add image**).
### OpenOCD
- Start OpenOCD in the `.pio/build/GD32F130F6P6` directory
  - `openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg`
- Telnet to debug server
  - `telnet localhost 4444`
- When telnetted in:
  1. `halt`
  2. `stm32fx mass_erase 0`
  3. `flash write_bank 0 firmware.bin`
  4. `reset`

### PlatformIO
- Much simpler, just run PlatformIO->Upload from VSCode
- If all things are good, it should just upload and reset

## Fixing Bricked CPU
- If for whatever reason you can't access the CPU via OpenOCD, you might need to do the following to fix:
1. Disconnect SWD from USB port
2. Use a wire to pull pin 1 (`BOOT0`) to 3v3 and plug in the SWD
3. Since you are blocking booting from flash, any broken code won't run, so now you should be able to start the debug server fine
4. Still keeping the pin pulled high, telnet into the debug server and run `stm32f1x mass_erase 0` to clear the flash
5. Now you should be able to fix your code and re-flash

## High Level TODOs
- [ ] Setup unused UART with an LED for some interaction and for debugging
- [ ] Confirm that UART send and receive is working correctly
- [ ] Setup preemptive RIOS (mainly for handling watchdog better)
- [ ] Figure out how IR data is received and sent to the CPU
- [ ] Parse data and send over serial
- [ ] Figure out motor control
- [ ] Allow control of motor speed over serial
