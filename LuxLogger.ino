
/* 
 *  Version: 0-4-2
 *  Arduino IDE V1.8.13
 *  Board: Adafruit Feather 32u4 Adalogger https://learn.adafruit.com/adafruit-feather-32u4-adalogger
 *  Sensor: TSL2561 Luminosity Sensor https://learn.adafruit.com/tsl2561
 *  Created:  14.12.2020
 *  Modified: 02.01.2021
 *  Tested:   02.01.2021
 *  Resources used: 22324 Bytes (77%) of Program Memory. Maximum are 28672 Bytes.
 *  Courtesy: Adafruit https://github.com/adafruit/Adafruit_TSL2561/blob/master/examples/sensorapi/sensorapi.ino 
 *  and https://learn.adafruit.com/adafruit-feather-32u4-adalogger/using-the-sd-card?embeds=allow
 *  This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
 *  which provides a common 'type' for sensor data and some helper functions.
   
 *  To use this driver you will also need to download the Adafruit_Sensor library and include
 *  it in your libraries folder.

 *  You should also assign a unique ID to this sensor for use with the Adafruit Sensor API
 *  so that you can identify this particular
 *  sensor in any data logs, etc.  To assign a unique ID, simply provide an appropriate value
 *  in the constructor below (12345 is used by default in this example).
   
 * Connections
 * ===========
 *  Connect SCL to I2C SCL Clock
 *  Connect SDA to I2C SDA Data
 *  Connect VCC/VDD to 3.3V or 5V (depends on sensor's logic level, check the datasheet)
 *  Connect GROUND to common ground

 *  I2C Address
 *  ===========
 *  The address will be different depending on whether you leave
 *  the ADDR pin floating (addr 0x39), or tie it to ground or vcc. 
 *  The default addess is 0x39, which assumes the ADDR pin is floating
 *  (not connected to anything).  If you set the ADDR pin high
 *  or low, use TSL2561_ADDR_HIGH (0x49) or TSL2561_ADDR_LOW
 *  (0x29) respectively.
    
 *  History
 *  =======
 *  2013/JAN/31  - First version (KTOWN)
 *  2020/DEC/14 Something new from OverTheRhine@backhaus-net.de
*/
#include <SPI.h>
#include <SD.h>
#include <Wire.h>                 // I2C bus for sensor comms
#include <Adafruit_Sensor.h>      // Lib for transforming raw data to units
#include <Adafruit_TSL2561_U.h>   // Lib for lux sensor TSL2561

// Set the pins used
#define cardSelect 4
#define sdLED 8       // Internal LED #8 (
#define intLED 13     // Internal LED #13 (red)
#define intDIP_SW4 12 // DIP-Switch #4
#define intDIP_SW3 11 // DIP-Switch #3
#define intDIP_SW2 10 // DIP-Switch #2
#define intDIP_SW1 9  // DIP-Switch #1

File logfile;
//File root;
   
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345); // Create instance with default sensor id (12345) and initiate with floating address pin

/**************************************************************************/
/*
    blink out an error code for ever
    error(2): "Card init. failed!"
    error(3): "Couldnt create " (logfile)
    error(4): "Error opening: " (logfile)
*/
/**************************************************************************/
void error(uint8_t errno) {
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++){ // blink requested code
      digitalWrite(intLED, HIGH);
      delay(100);
      digitalWrite(intLED, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++){ // wait to complete cycle of two seconds
      delay(200);
    }
  }
}

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void) {
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");  
  Serial.println("------------------------------------");
  Serial.println();
  delay(200);
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void configureSensor(void) {
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
  Serial.println();
  delay(200);
}

/**************************************************************************/
/*
    printDirectory function
*/
/**************************************************************************/
void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry){
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    }
    else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

/**************************************************************************/
/*
    SD-Card init function (called during setup()
*/
/**************************************************************************/
void sd_init() {
  Serial.println();
  Serial.println("Card initialization started...");
  if (!SD.begin(cardSelect)) {
    Serial.println("Card initialization failed!");
    error(2);
  }
  Serial.println("Card initialization done!");
  Serial.println();
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup() {
  pinMode(intDIP_SW1, INPUT); // Setup ports for DIP-Switch first
  pinMode(intDIP_SW2, INPUT);
  pinMode(intDIP_SW3, INPUT);
  pinMode(intDIP_SW4, INPUT);
  pinMode(sdLED, OUTPUT);
  pinMode(intLED, OUTPUT);
  uint8_t val;

// Destroy all existing files LUXDAT(xy).TXT (stored on SD-Card); only possible when connected to a terminal
  val = digitalRead(intDIP_SW1);
  if (val == HIGH) {
    Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
    sd_init();
    // create filename, supposing file exist on SD-Card
    char filename[15];
    strcpy(filename, "/LUXDAT00.TXT");
    for (uint8_t i = 0; i < 100; i++) { 
      filename[7] = '0' + i/10;
      filename[8] = '0' + i%10;
      if (SD.exists(filename)) {
        Serial.print("Deleting file: "); Serial.println(filename);
        SD.remove(filename);
      }
      if (SD.exists(filename)) {
        Serial.print(filename); Serial.println(" exists.");
      }
      else {
        Serial.print(filename); Serial.println(" doesn't exist.");
      }
    }
  }

  // Reading SD-Card directory
  val = digitalRead(intDIP_SW2);
  if (val == HIGH) {
    Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("------------------------------------");
    Serial.println("Listing SD card directory...");
    sd_init();
    File root;
    root = SD.open("/");
    printDirectory(root, 0);
    Serial.println("Listing directory done!");
    Serial.println("------------------------------------");
    Serial.println();
  }

// Reading all content of existing files LUXDAT(xy).TXT (storing via terminal program to an ASCII file)
  val = digitalRead(intDIP_SW3);
  if (val == HIGH) {
    Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
    sd_init();
    // create filename, supposing file exist on SD-Card
    char filename[15];
    strcpy(filename, "/LUXDAT00.TXT");
    for (uint8_t i = 0; i < 100; i++) { 
      filename[7] = '0' + i/10;
      filename[8] = '0' + i%10;
      if (SD.exists(filename)){
        File dataFile = SD.open(filename);
        // if the file is available, open and write to serial
        if (dataFile) {
          Serial.print("Reading data from: "); Serial.println(filename); // Write name of file
          while (dataFile.available()) {
            Serial.write(dataFile.read());
          }
        dataFile.close();
        }
        // if the file isn't open, pop up an error:
        else {
        Serial.print("Error opening: "); Serial.println(filename);
        error(4);
        }
      }
    }
  }
// Create a new logfile (default)  
  Serial.begin(9600);
  //Serial.println("Light Sensor Test"); Serial.println();
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
    error(2);
  }
  char filename[15];
  strcpy(filename, "/LUXDAT00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (!SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if(!logfile) {
    Serial.print("Couldnt create "); Serial.println(filename);
    error(3);
  }
  Serial.print("Writing to "); Serial.println(filename);
  Serial.println("Ready!");
  Serial.println();
  
  /* Initialise the sensor */
  //use tsl.begin() to default to Wire, 
  //tsl.begin(&Wire2) directs api to use Wire2, etc.
  if(!tsl.begin()) {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Warning, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  /* Display some basic information on this sensor */
  displaySensorDetails();
  /* Setup the sensor gain and integration time */
  configureSensor();
  /* We're ready to go! */
  Serial.println("Starting measuring!");
  Serial.println();
  logfile.println("Data;Unit[lux]"); // Write header for data listing
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete
*/
/**************************************************************************/
void loop() {
  uint8_t val;
  val = digitalRead(intDIP_SW4);
  if (val == HIGH) {
    /* Get a new sensor event */ 
    sensors_event_t event;
    tsl.getEvent(&event);
    /* Display the results (light is measured in lux) */
    if (event.light) {
      Serial.print(event.light); Serial.println(" lux");
      digitalWrite(sdLED, HIGH);
      logfile.print(event.light); logfile.println(";lux"); logfile.flush(); // storing value and unit divided by delimiter
      digitalWrite(sdLED, LOW);
    }
    else {
      /* If event.light = 0 lux the sensor is probably saturated
      and no reliable data could be generated! */
      Serial.println("Sensor overload");
      digitalWrite(sdLED, HIGH);
      logfile.print("Sensor overload"); logfile.print(";"); logfile.println("lux"); logfile.flush(); // storing message and unit divided by delimiter
      digitalWrite(sdLED, LOW);
    }
  }
  else {
    Serial.println("Logger not enabled, waiting...");
    digitalWrite(intLED, HIGH);
    delay(200);
    digitalWrite(intLED, LOW);
    //delay(200);
  }
  delay(59987); // 60.000 ms minus 13 ms integration time lux sensor
}
