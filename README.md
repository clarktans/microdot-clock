# microdot-clock
NTP Clock with arduino and MicroDot Phat

the code for the NTP Clock is as follows, the hardware is an Adafruit Huzzah w/ ESP8266 and a MicroDot Phat
the key point to remember is that with each pair of 5x7 matrixes one uses row bit mapped data and the other uses column bit mapped data. so to save space there is column data bit mapped data and when a row data set is required a short routine converts the column data to row data and that is sent via I2C.
