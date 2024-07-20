# LED Matrix Games

This project uses the [PxMatrix](https://github.com/2dom/PxMatrix/tree/master) library to display some games on a 64x64 LED matrix. They run on an ESP32 and are controlled by an arcade stick and 2 buttons per player, as well as an additional Start button.

The code was written to be able to easily support any size LED matrix and a variable number of players.

Currently a Snake game and Tetris have been implemented. Both support 1-player, 2-player, and an auto-playing screensaver mode.

There is also an emulator included based on [Adafruit_GFX_dummy_display](https://github.com/croutor/Adafruit_GFX_dummy_display) with some modifications. This allows testing the code on a PC without needing to flash to the ESP32 every time. To build the executable, cd into src/Adafruit_GFX_emulator/ and run make. I'm on Windows so I used MSYS2, I think the Makefile should build on Linux but I haven't tested it.

I use an ESP32-WROOM-32D development board and will add wiring instructions eventually.
