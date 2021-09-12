# lds-006-firmware
This is a (currently very early) attempt at writing an alternate firmware for the LDS-006 LIDAR.

Information about the module can be found [here](https://0x416c6578.github.io/lds-006/overview.html).

This project uses the PlatformIO build system, with most of the setup taken from [this repo](https://github.com/maxgerhardt/pio-gd32f130c6) (huge thanks to the original developer, this saved a lot of headache). I made a custom configuration for the GD32F130F6P6, but most things stayed the similar to the original project.

## High Level TODOs
- [ ] Setup unused UART with an LED for some interaction and for debugging
- [ ] Confirm that UART send and receive is working correctly
- [ ] Figure out how IR data is received and sent to the CPU
- [ ] Parse data and send over serial
- [ ] Figure out motor control
- [ ] Allow control of motor speed over serial