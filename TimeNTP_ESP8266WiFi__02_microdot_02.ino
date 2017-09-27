/*
 * TimeNTP_ESP8266WiFi.ino
 * Example showing time sync to NTP time source
 *
 * This sketch uses the ESP8266WiFi library
 */

#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix = Adafruit_7segment();

const char ssid[] = "*******";  //  your network SSID (name)
const char pass[] = "*******";       // your network password

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

//const int timeZone = 1;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)


WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);

//------------------------------------------------------
// microdot setup



#define disp12 0x63    //0x63
#define disp34 0x62    //0x62
#define disp56 0x61    //0x61

byte x = 0;
int x1 = 0;
byte y = 0;
byte rowx = 0;
byte coly = 0;
byte digit = 0;
byte number = 0;
byte colindex = 0;
byte rowindex = 0;
byte asciioffset = 0x10;
byte ampm = 0x21;
unsigned 
long secsSince1900;


byte row[7];

byte col[480] = {                     // even digits (2,4,6) have column bit map for each column of 5x7 matrix
    0x00, 0x00, 0x00, 0x00, 0x00, // (space)  00
    0x00, 0x00, 0x5F, 0x00, 0x00, // !        01
    0x00, 0x07, 0x00, 0x07, 0x00, // "        02
    0x14, 0x7F, 0x14, 0x7F, 0x14, // #        03
    0x24, 0x2A, 0x7F, 0x2A, 0x12, // $        04
    0x23, 0x13, 0x08, 0x64, 0x62, // %        05
    0x36, 0x49, 0x55, 0x22, 0x50, // &        06
    0x00, 0x05, 0x03, 0x00, 0x00, // '        07
    0x00, 0x1C, 0x22, 0x41, 0x00, // (        08
    0x00, 0x41, 0x22, 0x1C, 0x00, // )        09
    0x08, 0x2A, 0x1C, 0x2A, 0x08, // *        0a
    0x08, 0x08, 0x3E, 0x08, 0x08, // +        0b
    0x00, 0x50, 0x30, 0x00, 0x00, // ,        0c
    0x08, 0x08, 0x08, 0x08, 0x08, // -        0d
    0x00, 0x60, 0x60, 0x00, 0x00, // .        0e
    0x20, 0x10, 0x08, 0x04, 0x02, // /        0f
    0x3E, 0x51, 0x49, 0x45, 0x3E, // 0        10
    0x00, 0x42, 0x7F, 0x40, 0x00, // 1        11
    0x42, 0x61, 0x51, 0x49, 0x46, // 2        12
    0x21, 0x41, 0x45, 0x4B, 0x31, // 3        13
    0x18, 0x14, 0x12, 0x7F, 0x10, // 4        14
    0x27, 0x45, 0x45, 0x45, 0x39, // 5        15
    0x3C, 0x4A, 0x49, 0x49, 0x30, // 6        16
    0x01, 0x71, 0x09, 0x05, 0x03, // 7        17
    0x36, 0x49, 0x49, 0x49, 0x36, // 8        18
    0x06, 0x49, 0x49, 0x29, 0x1E, // 9        19
    0x00, 0x36, 0x36, 0x00, 0x00, // :        1a
    0x00, 0x56, 0x36, 0x00, 0x00, // ;        1b
    0x00, 0x08, 0x14, 0x22, 0x41, // <        1c
    0x14, 0x14, 0x14, 0x14, 0x14, // =        1d
    0x41, 0x22, 0x14, 0x08, 0x00, // >        1e
    0x02, 0x01, 0x51, 0x09, 0x06, // ?        1f
    0x32, 0x49, 0x79, 0x41, 0x3E, // @        20
    0x7E, 0x11, 0x11, 0x11, 0x7E, // A        21
    0x7F, 0x49, 0x49, 0x49, 0x36, // B        22
    0x3E, 0x41, 0x41, 0x41, 0x22, // C        23
    0x7F, 0x41, 0x41, 0x22, 0x1C, // D        24
    0x7F, 0x49, 0x49, 0x49, 0x41, // E        25
    0x7F, 0x09, 0x09, 0x01, 0x01, // F        26
    0x3E, 0x41, 0x41, 0x51, 0x32, // G        27
    0x7F, 0x08, 0x08, 0x08, 0x7F, // H        28
    0x00, 0x41, 0x7F, 0x41, 0x00, // I        29
    0x20, 0x40, 0x41, 0x3F, 0x01, // J        2a
    0x7F, 0x08, 0x14, 0x22, 0x41, // K        2b
    0x7F, 0x40, 0x40, 0x40, 0x40, // L        2c
    0x7F, 0x02, 0x04, 0x02, 0x7F, // M        2d
    0x7F, 0x04, 0x08, 0x10, 0x7F, // N        2e
    0x3E, 0x41, 0x41, 0x41, 0x3E, // O        2f
    0x7F, 0x09, 0x09, 0x09, 0x06, // P        30
    0x3E, 0x41, 0x51, 0x21, 0x5E, // Q        31
    0x7F, 0x09, 0x19, 0x29, 0x46, // R        32
    0x46, 0x49, 0x49, 0x49, 0x31, // S        33
    0x01, 0x01, 0x7F, 0x01, 0x01, // T        34
    0x3F, 0x40, 0x40, 0x40, 0x3F, // U        35
    0x1F, 0x20, 0x40, 0x20, 0x1F, // V        36
    0x7F, 0x20, 0x18, 0x20, 0x7F, // W        37
    0x63, 0x14, 0x08, 0x14, 0x63, // X        38
    0x03, 0x04, 0x78, 0x04, 0x03, // Y        39
    0x61, 0x51, 0x49, 0x45, 0x43, // Z        3A
    0x00, 0x00, 0x7F, 0x41, 0x41, // [        3B
    0x02, 0x04, 0x08, 0x10, 0x20, // "\"      3C
    0x41, 0x41, 0x7F, 0x00, 0x00, // ]        3D
    0x04, 0x02, 0x01, 0x02, 0x04, // ^        3E
    0x40, 0x40, 0x40, 0x40, 0x40, // _        3F
    0x00, 0x01, 0x02, 0x04, 0x00, // `        40
    0x20, 0x54, 0x54, 0x54, 0x78, // a        41
    0x7F, 0x48, 0x44, 0x44, 0x38, // b        42
    0x38, 0x44, 0x44, 0x44, 0x20, // c        43
    0x38, 0x44, 0x44, 0x48, 0x7F, // d        44
    0x38, 0x54, 0x54, 0x54, 0x18, // e        45
    0x08, 0x7E, 0x09, 0x01, 0x02, // f        46
    0x08, 0x14, 0x54, 0x54, 0x3C, // g        47
    0x7F, 0x08, 0x04, 0x04, 0x78, // h        48
    0x00, 0x44, 0x7D, 0x40, 0x00, // i        49
    0x20, 0x40, 0x44, 0x3D, 0x00, // j        4A
    0x00, 0x7F, 0x10, 0x28, 0x44, // k        4B
    0x00, 0x41, 0x7F, 0x40, 0x00, // l        4C
    0x7C, 0x04, 0x18, 0x04, 0x78, // m        4D
    0x7C, 0x08, 0x04, 0x04, 0x78, // n        4E
    0x38, 0x44, 0x44, 0x44, 0x38, // o        4F
    0x7C, 0x14, 0x14, 0x14, 0x08, // p        50
    0x08, 0x14, 0x14, 0x18, 0x7C, // q        51
    0x7C, 0x08, 0x04, 0x04, 0x08, // r        52
    0x48, 0x54, 0x54, 0x54, 0x20, // s        53
    0x04, 0x3F, 0x44, 0x40, 0x20, // t        54
    0x3C, 0x40, 0x40, 0x20, 0x7C, // u        55
    0x1C, 0x20, 0x40, 0x20, 0x1C, // v        56
    0x3C, 0x40, 0x30, 0x40, 0x3C, // w        57
    0x44, 0x28, 0x10, 0x28, 0x44, // x        58
    0x0C, 0x50, 0x50, 0x50, 0x3C, // y        59
    0x44, 0x64, 0x54, 0x4C, 0x44, // z        5A
    0x00, 0x08, 0x36, 0x41, 0x00, // {        5B
    0x00, 0x00, 0x7F, 0x00, 0x00, // |        5C
    0x00, 0x41, 0x36, 0x08, 0x00, // }        5D
    0x08, 0x08, 0x2A, 0x1C, 0x08, // ->       5E
    0x08, 0x1C, 0x2A, 0x08, 0x08}; // <-      5F



//------------------------------------------------------
void setup()
{

    matrix.begin(0x70);
    
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  delay(250);
  Serial.println("TimeNTP Example");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);


//----------------------------------------------------
// microdot setup

// setup up display controllers
//-------------------------------------------------------------------

  Wire.beginTransmission(disp12 );    //  write i2c address 0x61
  Wire.write(0x00);                 // write to configuration register
  Wire.write(0x19);                 //write row data
  Wire.endTransmission(); 
  
  Wire.beginTransmission(disp12 );    //  write i2c address 0x61
  Wire.write(0x0D);                 // write to INTENSITY CONTROL REGISTER
  Wire.write(0x09);                 //write row data
  Wire.endTransmission(); 

  Wire.beginTransmission(disp12 );    //  write i2c address 0x61
  Wire.write(0x19);                 // write to INTENSITY CONTROL REGISTER
  Wire.write(0x80);                 //write row data
  Wire.endTransmission(); 

    Wire.beginTransmission(disp34 );    //  write i2c address 0x61
  Wire.write(0x00);                 // write to configuration register
  Wire.write(0x19);                 //write row data
  Wire.endTransmission(); 
  
  Wire.beginTransmission(disp34);    //  write i2c address 0x61
  Wire.write(0x0D);                 // write to INTENSITY CONTROL REGISTER
  Wire.write(0x09);                 //write row data
  Wire.endTransmission(); 

  Wire.beginTransmission(disp34);    //  write i2c address 0x61
  Wire.write(0x19);                 // write to INTENSITY CONTROL REGISTER
  Wire.write(0x80);                 //write row data

    Wire.beginTransmission(disp56 );    //  write i2c address 0x61
  Wire.write(0x00);                 // write to configuration register
  Wire.write(0x19);                 //write row data
  Wire.endTransmission(); 
  
  Wire.beginTransmission(disp56);    //  write i2c address 0x61
  Wire.write(0x0D);                 // write to INTENSITY CONTROL REGISTER
  Wire.write(0x09);                 //write row data
  Wire.endTransmission(); 

  Wire.beginTransmission(disp56);    //  write i2c address 0x61
  Wire.write(0x19);                 // write to INTENSITY CONTROL REGISTER
  Wire.write(0x80);                 //write row data
  Wire.endTransmission(); 

  delay(15);


//-------------------------------------------------
  
}

time_t prevDisplay = 0; // when the digital clock was displayed

void loop()      {
      microdot();
    
       if ((minute() == 0 ) & (second() == 0) ) {
          
       // if ((second() == 0) ) {
             getNtpTime();
             digitalClockDisplay();
             delay(1000);
            }

     }

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();
  Serial.println(secsSince1900);
}

void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 3500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
     // unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

//----------------------------------------------------
// display time on microdot

void microdot() {
int lastsecond ;

 //   DateTime now = rtc.now();
//    Serial.print(now.year(), DEC);
//    Serial.print('/');
//    Serial.print(now.month(), DEC);
//    Serial.print('/');
//    Serial.print(now.day(), DEC);
//    Serial.print(" (");
  //  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
//    Serial.print(") ");
//    Serial.print(now.hour(), DEC);
//    Serial.print(':');
//    Serial.print(now.minute(), DEC);
//    Serial.print(':');
//    Serial.print(now.second(), DEC);

//    Serial.println();

 //   if( lastsecond != second()) 
 //     if(lastminute != minute())

 //   {

  // lastsecond = second();
//      lastminute = minute();

// for (byte number = 0; number<10; number++) {

      for (byte digit = 1; digit < 7; digit++) {   

    switch (digit){

    case 1:
     if (hour() > 12) {
      x = (hour() - 12)/10 + asciioffset;
      ampm = 0x30;
     }
      else
      {
           x = hour()/10 +  asciioffset;
           ampm= 0x21;
      }
   
      
      break;
      
    case 2:
     if (hour() > 12) {
      x = (hour() - 12) % 10 + asciioffset;

     }
      else
      {
           x = hour()%10 +  asciioffset;
    
      }
   
      break;

    case 3:       //display blinking  colon 
    if (second()% 2 == 1) {
           x = 0x1a;
    }
    else
    {
         x = 0x00;
    }
       break;

    case 4:
       x = minute() /10 +  asciioffset;
      break;

    case 5:
      x = minute() % 10 +  asciioffset;
      break;

    case 6:
      x = ampm;
    
      break;
    }
    
     writedisp(digit, x);

          

 
    }     // end send digits

 //   } // end new second if

   }     // end digit count



      //Serial.print(hours);
      //Serial.print(":");
      //Serial.print(mins);
      //Serial.print(":");
      //Serial.println(secs);
      //  writedisp12(hours);
      //writecpolon()
      //writedisp34(mins);
      //writedisp56(secs);
        
    //  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    //  delay(50);                       // wait for a second
    //  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW




//----------------------------------------------

//byte writedisp12(byte dig int x) { 

void writedisp (byte digit, byte x){
  
  switch(digit){

    case 1:
      // digit 1
      Wire.beginTransmission(disp12 );    //  write i2c 
      Wire.write(0x0E);                 // write to DIGIT 1

        for (int coly = 0; coly<5; coly++ )
              {
                  Wire.write (col[x*5 + coly]);                 //write row data 
               }

        Wire.endTransmission();

          Wire.beginTransmission(disp12);    //  write i2c address 0x61
          Wire.write(0x0c);                 // write to update display register
          Wire.write(0xff);                 //write row data
          Wire.endTransmission();
    break;

   case 2:
      // digit 2
      coltorowconv(x);                    // convert column dot matrix to row map
      Wire.beginTransmission(disp12 );    //  write i2c 
      Wire.write(0x01);                 // write to DIGIT 1
        for (int rowx = 0; rowx<7; rowx++ )
            {
                Wire.write (row[rowx]);                 //write row data 
            }

      Wire.endTransmission();

      Wire.beginTransmission(disp12);    //  write i2c address 0x61
      Wire.write(0x0c);                 // write to update display register
      Wire.write(0xff);                 //write row data
      Wire.endTransmission();
    break;

   case 3:
     // digit 3  
      Wire.beginTransmission(disp34);    //  write i2c 
      Wire.write(0x0E);                 // write to DIGIT 1
       for (int coly = 0; coly<5; coly++ )
            {
                Wire.write (col[x*5 + coly]);                 //write row data 
            }

      Wire.endTransmission();

        Wire.beginTransmission(disp34);    //  write i2c address 0x61
        Wire.write(0x0c);                 // write to update display register
        Wire.write(0xff);                 //write row data
        Wire.endTransmission();
    break;
    
  case 4:                   // convert column dot matrix to row map
      // digit 4
      coltorowconv(x);
      Wire.beginTransmission(disp34 );    //  write i2c 
      Wire.write(0x01);                 // write to DIGIT 1
      for (int rowx = 0; rowx<7; rowx++ )
            {
                Wire.write (row[rowx]);              //write row data 
            }

      Wire.endTransmission();

        Wire.beginTransmission(disp34);    //  write i2c address 0x61
        Wire.write(0x0c);                 // write to update display register
        Wire.write(0xff);                 //write row data
        Wire.endTransmission();
     break;

  case 5:
      // digit 5
        Wire.beginTransmission(disp56 );    //  write i2c 
        Wire.write(0x0E);                 // write to DIGIT 1
        for (int coly = 0; coly<5; coly++ )
              {
                Wire.write (col[x*5 + coly ]);                 //write row data 
              }
        Wire.endTransmission();

        Wire.beginTransmission(disp56);    //  write i2c address 0x61
        Wire.write(0x0c);                 // write to update display register
        Wire.write(0xff);                 //write row data
        Wire.endTransmission();
     break;

  case 6:                   // convert column dot matrix to row map
       // digit 6 
        coltorowconv(x);
       Wire.beginTransmission(disp56 );    //  write i2c 
       Wire.write(0x01);                 // write to DIGIT 1
        for (int rowx = 0; rowx<7; rowx++ )
              {
                Wire.write (row[rowx]);                 //write row data 
              }
        Wire.endTransmission();

        Wire.beginTransmission(disp56);    //  write i2c address 0x61
        Wire.write(0x0c);                 // write to update display register
        Wire.write(0xff);                 //write row data
        Wire.endTransmission();
        
    }         // end switch

}         // end writedisp

//---------------------------------------------------------------------
  
void coltorowconv(byte digit) {                   // convert column dot matrix to row map

      for (rowindex = 0; rowindex < 7; rowindex++){
            row[rowindex] = 0;

                row[rowindex] =  (bitRead(col[digit * 5 + 0], rowindex) * 1)
                                +(bitRead(col[digit * 5 + 1], rowindex) * 2)
                                +(bitRead(col[digit * 5 + 2], rowindex) * 4)
                                +(bitRead(col[digit * 5 + 3], rowindex) * 8)
                                +(bitRead(col[digit * 5 + 4], rowindex) * 16);

             }    // end rowindex loop

 // Serial.print ("digit = ");
 // Serial.print (digit);
 //   Serial.print (" ");
 // for (rowindex = 0; rowindex < 7; rowindex++){
 //   Serial.print ( row[rowindex], HEX);
 //   Serial.print (" ");
    
//  }   // end print summary loop
 //   Serial.println(" ");
}    //end void loop

