// from https://duino4projects.com/an-fm-stereo-broadcaster-pll-using-arduino/
//  By: Cairn; caimaver (at) yahoo dot com
//  Date: 22 Mar. 2008
//  MODs by: Mike Yancey, May 12 - 14, 2008
//  Description: Uses an Arduino or compatible to control an 
//               NS73M FM Transmitter module (available from 
//               Sparkfun) on the I2C bus.

// Modified by Professor Marshall, 3/2022 for ESP32 and M5StickC-Plus.
// Modified by Nathanael Covarrubias, 4/2023 for I2C
// Unpolished, but gets the NS73M on the air as a proof-of-concept.

#include <Wire.h>
#include <M5StickCPlus.h>

#define upButton 39
// up button on pin 6
#define downButton 37                // down button on pin 5
#define setButton 26                 // set button to toggle on/off air

#define topFM  107900000            // Top of the FM Dial Range in USA
#define botFM   87500000            // Bottom of the FM Dial Range in USA
#define incrFM    200000            // FM Channel Increment in USA

long frequency = 87500000;          // the default initial frequency in Hz
long newFrequency = 0;
boolean gOnAir = false;             // Initially, NOT On The Air...

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.printf("NS73M FM Transmitter");
  
  
  // Setup the Up/Down and SET buttons
  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  pinMode(setButton, INPUT);
 
  Serial.begin(115200);                 //for debugging
 
  // Attempt to read the last saved frequency from EEPROM
  ///newFrequency = loadFrequency();

  // Test if outside our FM Range...
  ///if ( newFrequency < botFM || newFrequency > topFM ) {
    // Sorry - haven't saved before - use the default.
    ///frequency = 97300000;
//  }
//  else {
    // We have a valid frequency!
    ///frequency = newFrequency;
 // }  

  M5.Lcd.setCursor(10, 35);
  M5.Lcd.printf("FM Stereo Broadcaster");
    
  displayFrequency( frequency);


  Wire.begin(0, 25);//0 is sda and 26 is scl
  // M5StickC-Plus
  // White  33 Clock SCL
  // Yellow 32 Data  SDA


  transmitter_setup( frequency );
}



void loop() {
//
//  if (digitalRead(upButton) == LOW) {
//    frequency += incrFM;                              // 200kHz steps for North American channel spacing
//    frequency = constrain( frequency, botFM, topFM);  // Keeps us in range...
//
//    Serial.println( frequency, DEC );
//    transmitter_standby( frequency );
//  }
//
//  if (digitalRead(downButton) == LOW) {
//    frequency -= incrFM;                              // 200kHz steps for North American channel spacing
//    frequency = constrain( frequency, botFM, topFM);  // Keeps us in range...
//    
//    Serial.println( frequency, DEC );
//    transmitter_standby( frequency );
//  }
  
  if (digitalRead(setButton) == LOW) {
    // Create a 'toggle' - pressing set while OnAir - set's StandBy and
    // if we're already StandBy, set the Frequency and go OnAir... 
    if ( gOnAir ) {
      transmitter_standby( frequency );
    }
    else {
      set_freq( frequency );
      //saveFrequency( frequency );     // Save the Frequency to EEPROM Memory
      delay(1000);
    }
  }
  // Lather, Rinse, Repeat....
}



void transmitter_setup( long initFrequency )
{
  i2c_send(0x0E, B00000101); //Software reset

  i2c_send(0x01, B10110100); //Register 1: forced subcarrier, pilot tone on
  
  i2c_send(0x02, B00000011); //Register 2: Unlock detect off, 2mW Tx Power

  set_freq( initFrequency);

  i2c_send(0x00, B10100001); //Register 0: 200mV audio input, 75us pre-emphasis on, crystal off, power on
  
  i2c_send(0x0E, B00000101); //Software reset
  
  i2c_send(0x06, B00011110); //Register 6: charge pumps at 320uA and 80 uA
}

void transmitter_standby( long aFrequency )
{
  i2c_send(0x00, B10100000); //Register 0: 200mV audio input, 75us pre-emphasis on, crystal off, power OFF
  
  displayFrequency( aFrequency );
  
  delay(100);
  gOnAir = false;
}

void set_freq( long aFrequency )
{
  int new_frequency;

  // New Range Checking... Implement the (experimentally determined) VFO bands:
  if (aFrequency < 88500000) {                       // Band 3
    i2c_send(0x08, B00011011);
    //Serial.println("Band 3");
  }  
  else if (aFrequency < 97900000) {                 // Band 2
    i2c_send(0x08, B00011010);
    //Serial.println("Band 2");
  }
  else if (aFrequency < 103000000) {                  // Band 1 
    i2c_send(0x08, B00011001);
    //Serial.println("Band 1");
  }
  else {
    // Must be OVER 103.000.000,                    // Band 0
    i2c_send(0x08, B00011000);
    //Serial.println("Band 0");
  }


  new_frequency = (aFrequency + 304000) / 8192;
  byte reg3 = new_frequency & 255;                  //extract low byte of frequency register
  byte reg4 = new_frequency >> 8;                   //extract high byte of frequency register
  i2c_send(0x03, reg3);                             //send low byte
  i2c_send(0x04, reg4);                             //send high byte
  
  i2c_send(0x0E, B00000101);                        //software reset  

  Serial.print("Frequency changed to ");
  Serial.println(aFrequency, DEC);

  i2c_send(0x00, B10100001); //Register 0: 200mV audio input, 75us pre-emphasis on, crystal off, power ON

  M5.Lcd.setCursor(10, 70);
  M5.Lcd.printf("On Air");
  Serial.println("On Air");
  
//  lcd.cursorTo(2, 0); 
//  lcd.printIn( " On Air ");
  gOnAir = true;
}

void i2c_send(byte reg, byte data)
{ 
    Wire.beginTransmission(0x66);               // transmit to device 1100111
    Wire.write(reg);                                 // sends register address
    Wire.write(data);                                // sends register data
    Wire.endTransmission();                         // stop transmitting
    delay(5);                                       // allow register to set
    //how do we ask if it was sent sucessfully?
}

void saveFrequency ( long aFrequency ) 
{
  ///long memFrequency = 0;                   // For use in Read / Write to EEProm

  Serial.print( "Fake Saving: " );
  Serial.println(aFrequency, DEC);

  //memFrequency = aFrequency / 10000;
  ///EEPROM.write( 0, memFrequency / 256);   // right-most byte
  ///EEPROM.write( 1, memFrequency - (memFrequency / 256) * 256 );   // next to right-most byte
}

long loadFrequency ()
{
  long memFrequency = 0;                   // For use in Read / Write to EEProm

  ///memFrequency = EEPROM.read(0) * 256 + EEPROM.read(1);
  ///memFrequency *= 10000;

  //Serial.print("Retrieving: " );
  //Serial.println(memFrequency, DEC);
  return  memFrequency;
}


void displayFrequency( long aFrequency)
{

}
