# video_walkie_talkie_esp32
esp32 cam and cheap yellow display for video walkie talkie
[Explanatory video]([https://www.youtube.com/watch?v=d_h38X4_eQQ](https://youtu.be/rfnsuzQIYbs)) 
[![Demo Video]()]()

## Project code an update of Chris Greening's atomic14 esp32 walkie talkie project.
https://github.com/atomic14/esp32-walkie-talkie

# Setup
## camera_side
Need to define the MAC address of display receive side in the EspNowTransport.cpp file.

The __I2S__ pins are configured from the `src/config.h` file. Need pins for microphone only, can ignore speaker pins in this project (they are used in the display_side project). If you want to use the pdm microphone for the xiao esp32s3 cam, you need to
```
#define I2S_MIC_PDM
```
otherwise, you need to configure your microphone pins.

## display_side
The __I2S__ pins are configured from the `src/config.h` file. You need to set up the pins for your amplifier. Can ignore the microphone pins in this project.

# Parts List (most parts you need two of, one for each hand set)
1. seeed xiao sense esp32S3 cam: see https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/
2. elecrow crowpanel 2.8 inch display with esp32: see https://www.elecrow.com/esp32-display-2-8-inch-hmi-display-spi-tft-lcd-touch-screen.html
3. cheap yellow display: I bought mine from aliexpress, but shop around
4. printed pcb: https://www.pcbway.com/
5. MAX98357A amplifier.
6. MEMS INMP441 microphone (not essential because the pdm microphone on the xiao is quite good)
7. 3d printed box (files attached to this git repo)
8. 18650 battery
9. jumper cables
10. push buttons
11. on/off rocker switch
