# LuxLogger with TSL2561

Tags: TSL2561, FEATHER 32U4 ADALOGGER, Arduino, I2C, SPI, luminosity, lux, IR

Simple data logger based on ADAFRUIT FEATHER 32U4 ADALOGGER with luminosity sensor TSL2561 on an ADAFRUIT breakout board. Programming language is C++ (Arduino sketch). Four different functions are available by selecting them with the help of a 4 Bit DIP switch. 100 text files with acquired data can be stored on the SD card.

## Functionality:
* Read luminosity data (lux) from sensor and store it as numeric value on SD card.
* Read broadband- and IR-data (raw) from sensor and store it as numeric values on SD card.  
* Read all data from all text files from SD card and send it via USB serial to terminal program.
* Read directory from SD card and send it via USB serial to terminal program.
* Delete all stored text files on SD card.
* Blink some error codes with the internal LED.

## Used this hardware:
* https://www.adafruit.com/product/439
* https://www.adafruit.com/product/2795

## Used this software:
* Arduino IDE v1.8.13 https://www.arduino.cc
* https://github.com/adafruit/Adafruit_TSL2561
* https://github.com/adafruit/Adafruit_TSL2561/blob/master/examples/sensorapi/sensorapi.ino

## Thanks
I like to thank all the people who contributed most of the code in this project for their cleverness and effort to make things work.
