//Test cutdown (15 second timer) code
//Built on the Adafruit GPS parsing library
//www.adafruit.com
/***********************************
This is our GPS library
Adafruit invest
s time and resources providing this open source code,
please support Adafruit and open
-
source hardware by purchasing
products from Adafruit!
Written by Limor Fried/Ladyada for Adafruit Industries.
BSD license, check license.txt for more information
All tex
t above must be included in any redistribution
****************************************/
#include <SPI.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <avr/sleep.h>
//   Connect the GPS TX (transmit) pin to Digital 3
//   Connect the GPS RX (receive) pin to Digital 2
SoftwareSerial mySerial(8,7);
Adafruit_GPS GPS(&mySerial);

//  print raw gps data
#define LOG_FIXONLY false
#define GPSECHO  false

//this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); 

// Func prototype keeps Arduino 0023 happy
//pin 12 sends cutdown signal to balloon
int trigger_out = 12;
int cutdown_trys = 0;
int chipSelect =8;
int ledPin = 9;
File logfile;

// read a Hex value and return the decimal equivalent
uint8_t parseHex(char c) {
  if (c < '0')
    return 0;
  if (c <= '9')
    return c - '0';
  if (c < 'A')
    return 0;
  if (c <= 'F')
    return (c - 'A') + 10;
}
  
// blink out an error code
void error(uint8_t errno) {
  /*
  if (SD.errorCode()) {
    putstring("SD error: ");
    Serial.print(card.errorCode(), HEX);
    Serial.print(',');
    Serial.println(card.errorData(), HEX);
  }
  */
    
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++) {
      delay(200);
    }
  }
}

void setup() {
  
  // for Leonardos, if you want to debug 
  // SD issues, uncomment this line
  // to see serial output
  //while (!Serial);
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  
  Serial.begin(115200);
  Serial.println("\r\nUltimate GPSlogger Shield");
  pinMode(ledPin, OUTPUT);
  pinMode(9, OUTPUT);

  Serial.println("Not assuming sd card is present or saving logs.");
//  // see if the card is present and can be initialized:
//  //if (!SD.begin(chipSelect, 11, 12, 13)) {
//    if (!SD.begin(chipSelect)) {      // if you're using an UNO, you can use this line instead
//    Serial.println("Card init. failed!");
//    error(2);
//  }
//  char filename[15];
//  strcpy(filename, "GPSLOG00.TXT");
//  for (uint8_t i = 0; i < 100; i++) {
//    filename[6] = '0' + i/10;
//    filename[7] = '0' + i%10;
//    
//    // create if does not exist, do not open existing, write, sync after write
//    if (! SD.exists(filename)) {
//      break;
//    }
//  }
//  logfile = SD.open(filename, FILE_WRITE);
//  if( ! logfile ) {
//    Serial.print("Couldnt create "); 
//    Serial.println(filename);
//    error(3);
//  }
//  Serial.print("Writing to "); 
//  Serial.println(filename);
  pinMode(trigger_out, OUTPUT);
//  Serial.begin(9600);
  Serial.println("Balloon cutdown!");
  
  GPS.begin(9600); 
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  //GPS spits out RMCGGA (see http://aprs.gids.nl/nmea/)
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   
  // 1 Hz update rate
  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);
  delay(500);
  Serial.print("\nSTARTING LOGGING....");
  if (GPS.LOCUS_StartLogger())
  Serial.println(" STARTED!");
  else
  Serial.println(" no response :(");
  delay(1000);  
}

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  if (GPSECHO)
  if (c) UDR0 = c;      
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

uint32_t timer = millis();
uint32_t starttimer = millis();

void loop() {   
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
  }
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    // Don't call lastNMEA more than once between parse calls!  Calling lastNMEA 
    // will clear the received flag and can cause very subtle race conditions if
    // new data comes in before parse is called again.
    char *stringptr = GPS.lastNMEA();
    
    if (!GPS.parse(stringptr))   // this also sets the newNMEAreceived() flag to false
    return;  // we can fail to parse a sentence in which case we should just wait for another
    // Sentence parsed! 
    Serial.println("OK");
    if (LOG_FIXONLY && !GPS.fix) {
      Serial.print("No Fix");
      return;
    }
  }
  
  // if millis() or timer wraps around, reset it
  if (timer > millis())  timer = millis();
  // just check for 15 seconds, then activate cutdown
  if(millis() - starttimer > 15000){
    digitalWrite(trigger_out, HIGH);
    delay(5000);
    Serial.println("Cutdown");
    Serial.println("Attempt");
    Serial.println(cutdown_trys);
    cutdown_trys = cutdown_trys + 1;
    digitalWrite(trigger_out, LOW);
  }
}
      
      

