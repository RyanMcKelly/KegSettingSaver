#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>  // RGB LCD Shield communications
#include <Adafruit_MCP23017.h>
#include "RTClib.h"

// A simple data logger for the Arduino analog pins

// how many milliseconds between grabbing data and logging it. 1000 ms is once a second
#define LOG_INTERVAL  1000 // mills between entries (reduce to take more/faster data)

// how many milliseconds before writing the logged data permanently to disk
// set it to the LOG_INTERVAL to write each time (safest)
// set it to 10*LOG_INTERVAL to write all data every 10 datareads, you could lose up to 
// the last 10 reads if power is lost but it uses less power and is much faster!
#define SYNC_INTERVAL 1000 // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()

#define ECHO_TO_SERIAL   1 // echo data to serial port
#define WAIT_TO_START    1 // Wait for serial input in setup()

// These defines make it easy to set the backlight color
#define OFF 0x0
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

// LCD shield uses the I2C SCL and SDA pins (analog 4&5):
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

//Record time of last button press for Menu:
unsigned long lastInputTime = 0; // last button press

// the PWMable digital pins that connect to the LEDs
#define pinPanelMeter_1 3
#define pinPanelMeter_2 5
#define pinPanelMeter_3 6
#define pinPanelMeter_4 9

// The analog pins that connect to the load cells
#define loadCell_1 0           // analog 0
#define loadCell_2 1           // analog 0
#define loadCell_3 2           // analog 0
#define loadCell_4 3           // analog 0

RTC_DS1307 RTC; // define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File settingsFile;

//Struct for load cell:
typedef struct
{
  int tareValue; //Reading from load cell with empty keg on it
  int fullValue; //Reading from load cell when set to full
  
} CellSettings;

int nKegs = 4;
CellSettings cellSettings[4];

void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);
  
  lcd.setBacklight(RED);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Error:");
  lcd.setCursor(0,1);
  lcd.print(str);
  
  // red LED indicates error
//  digitalWrite(redLEDpin, HIGH);

  while(1);
}

void loadParameters()
{
  // Load from SD Card:

}

void saveParameters()
{
  // Save tare/status to SD Card:

}

void setup(void)
{
  Serial.begin(9600);
  Serial.println();
  
  // connect to RTC, activate clock.
  Wire.begin();  
  if (!RTC.begin()) {
    #if ECHO_TO_SERIAL
        Serial.println("RTC failed");
    #endif  //ECHO_TO_SERIAL
  }
    // following line sets the RTC to the date & time this sketch was compiled
    // uncomment it & upload to set the time, date and start run the RTC!
    //RTC.adjust(DateTime(__DATE__, __TIME__));
  
  lcd.begin(16, 2);         // initialize display colums and rows
  
  int time = millis();
  lcd.print("Monitor, Active!");
  time = millis() - time;
  Serial.print("Took "); Serial.print(time); Serial.println(" ms");
  lcd.setBacklight(WHITE);
  delay(3000);  // Splash screen
  
  // use debugging LEDs on SDCard Shield
//  pinMode(redLEDpin, OUTPUT);
//  pinMode(greenLEDpin, OUTPUT);
  
#if WAIT_TO_START
  Serial.println("Type any character to start");
  while (!Serial.available());
#endif //WAIT_TO_START

  // initialize the SD card
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(chipSelect, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
  Serial.println("card initialized.");
  
  //Check to see if settings present
  
  //If settings present, load settings:
  
  //Else, if settings not present, create new settings file
//  // create a new file
//  char filename[] = "LOGGER00.CSV";
//  for (uint8_t i = 0; i < 100; i++) {
//    filename[6] = i/10 + '0';
//    filename[7] = i%10 + '0';
//    if (! SD.exists(filename)) {
//      // only open a new file if it doesn't exist
//      settingsFile = SD.open(filename, FILE_WRITE); 
//      break;  // leave the loop!
//    }
//  }
//  
//  if (! settingsFile) {
//    error("couldnt create file");
//  }
//  
//  Serial.print("Logging to: ");
//  Serial.println(filename);


  

  settingsFile.println("millis,stamp,datetime,light,temp,vcc");    
#if ECHO_TO_SERIAL
  Serial.println("millis,stamp,datetime,light,temp,vcc");
#endif //ECHO_TO_SERIAL
 
  // If you want to set the aref to something other than 5v
//  analogReference(EXTERNAL);
}

void loop(void)
{
  DateTime now;
  
  lcd.setCursor(0, 1);
  lcd.print(millis()/1000);

//  cellSettings[0].tareValue = 0;
//  cellSettings[0].fullValue = 498;

  uint8_t buttons = lcd.readButtons();

  if (buttons) {
    lcd.clear();
    lcd.setCursor(0,0);
    if (buttons & BUTTON_UP) {
      lcd.print("UP ");
      lcd.setBacklight(RED);
    }
    if (buttons & BUTTON_DOWN) {
      lcd.print("DOWN ");
      lcd.setBacklight(YELLOW);
    }
    if (buttons & BUTTON_LEFT) {
      lcd.print("LEFT ");
      lcd.setBacklight(GREEN);
    }
    if (buttons & BUTTON_RIGHT) {
      lcd.print("RIGHT ");
      lcd.setBacklight(TEAL);
    }
    if (buttons & BUTTON_SELECT) {
      lcd.print("SELECT ");
      lcd.setBacklight(VIOLET);
    }
  }


  // delay for the amount of time we want between readings
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
   
  // log milliseconds since starting
  uint32_t m = millis();
//  settingsFile.print(m);           // milliseconds since start
//  settingsFile.print(", ");    
#if ECHO_TO_SERIAL
  Serial.print(m);         // milliseconds since start
  Serial.print(", ");  
#endif

  // fetch the time
  now = RTC.now();
  // log time
//  settingsFile.print(now.unixtime()); // seconds since 1/1/1970
//  settingsFile.print(", ");
//  settingsFile.print('"');
//  settingsFile.print(now.year(), DEC);
//  settingsFile.print("/");
//  settingsFile.print(now.month(), DEC);
//  settingsFile.print("/");
//  settingsFile.print(now.day(), DEC);
//  settingsFile.print(" ");
//  settingsFile.print(now.hour(), DEC);
//  settingsFile.print(":");
//  settingsFile.print(now.minute(), DEC);
//  settingsFile.print(":");
//  settingsFile.print(now.second(), DEC);
//  settingsFile.print('"');
#if ECHO_TO_SERIAL
  Serial.print(now.unixtime()); // seconds since 1/1/1970
  Serial.print(", ");
  Serial.print('"');
  Serial.print(now.year(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print('"');
#endif //ECHO_TO_SERIAL

//  analogRead(photocellPin);
//  delay(10); 
//  int photocellReading = analogRead(photocellPin);  
//  
//  // Log the estimated 'VCC' voltage by measuring the internal 1.1v ref
//  analogRead(BANDGAPREF); 
//  delay(10);
//  int refReading = analogRead(BANDGAPREF); 
//  float supplyvoltage = (bandgap_voltage * 1024) / refReading; 
//  
//  logFile.print(supplyvoltage);
//#if ECHO_TO_SERIAL
//  Serial.println();
//  Serial.print(Supply Voltage: ");   
//  Serial.print(supplyvoltage);
//  Serial.println();
//#endif // ECHO_TO_SERIAL
//
//  settingsFile.println();
#if ECHO_TO_SERIAL
  Serial.println();
#endif // ECHO_TO_SERIAL

//  digitalWrite(greenLEDpin, LOW);

  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();
  
  // blink LED to show we are syncing data to the card & updating FAT!
//  digitalWrite(redLEDpin, HIGH);
//  settingsFile.flush();
//  digitalWrite(redLEDpin, LOW);
  
}

