#include "SparkFunLSM6DS3.h"
#include "Wire.h"
#include "SPI.h"
#include "ledTuning.h"

LSM6DS3 myIMU;

#define REDPIN 9
#define GREENPIN 16
#define BLUEPIN 17

LedTable myLedTable( 3 );

void setup() {
  // put your setup code here, to run once:

  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  digitalWrite(REDPIN, 1);
  digitalWrite(GREENPIN, 1);
  digitalWrite(BLUEPIN, 1);

  delay(2000);

  analogWrite(REDPIN, 30);
  analogWrite(GREENPIN, 30);
  analogWrite(BLUEPIN, 30);


  Serial.begin(57600);
  delay(3000);
  Serial.println("Began sketch");

  myIMU.settings.gyroEnabled = 0;  //Can be 0 or 1

  myIMU.settings.accelEnabled = 1;
  myIMU.settings.accelRange = 4;      //Max G force readable.  Can be: 2, 4, 8, 16
  myIMU.settings.accelSampleRate = 1666;  //Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666, 3332, 6664, 13330
  myIMU.settings.accelBandWidth = 50;  //Hz.  Can be: 50, 100, 200, 400;

  myIMU.settings.commInterface = SPI_MODE;

  myIMU.begin();

  myLedTable.calcTable();

  for ( int i = 0; i < 24; i++ )
  {
    Serial.println(myIMU.readRegister(i), HEX);
  }
  //  for( int i = 0; i < 256; i++ )
  //  {
  //    Serial.println(myLedTable.ledAdjust(i));
  //  }

  analogWrite(REDPIN, myLedTable.ledAdjust(255));
  analogWrite(GREENPIN, myLedTable.ledAdjust(0));
  analogWrite(BLUEPIN, myLedTable.ledAdjust(255));
  //while(1);
}

float lastZValue = 0;

void loop()
{
  float zValue = 0;

  delay(10);

  zValue = myIMU.readFloatAccelZ();
  zValue -= 1;
  
  if ( zValue < -2 ) zValue = -2;
  if ( zValue > 2 ) zValue = 2;
  
  float dZValue = zValue - lastZValue;
  lastZValue = zValue;

  uint8_t control = (zValue * 63) + 128;
  
  if( dZValue > .2 )
  {
    //Red
    analogWrite(REDPIN, myLedTable.ledAdjust( control ));    
    analogWrite(BLUEPIN, 0);
  }

  if( dZValue < -.2 )
  {
    //Blue
    analogWrite(BLUEPIN, myLedTable.ledAdjust( 255 - control ));    
    analogWrite(REDPIN, 0);
  }

}
