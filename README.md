# LuxLogger with TSL2561

Tags: TSL2561, FEATHER 32U4 ADALOGGER, Arduino, I2C, SPI, luminosity

Simple data logger based on ADAFRUIT FEATHER 32U4 ADALOGGER with luminosity sensor TSL2561 on an ADAFRUIT breakout board. Programming language is C++ (Arduino sketch). Four different functions are available by selecting them with the help of a 4 Bit DIP switch. 100 text files with acquired data can be stored on the SD card.

Functionality:
- Read sensor data and store it as numeric value (lux) on SD card.
- Read all data from all text files from SD card and send it via USB serial to a terminal program.
- Read directory from SD card and send it via USB serial to a terminal program.
- Delete all stored text files on SD card.

Used this hardware:
- https://www.adafruit.com/product/439
- https://www.adafruit.com/product/2795

Used this software:
- Arduino IDE v1.8.13 https://www.arduino.cc
- https://github.com/adafruit/Adafruit_TSL2561
- https://github.com/adafruit/Adafruit_TSL2561/blob/master/examples/sensorapi/sensorapi.ino
