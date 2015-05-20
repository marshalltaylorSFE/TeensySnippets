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


void loop()
{
  float xValue = 0;
  float yValue = 0;
  float zValue = 0;
  for ( int i = 0; i < 24; i++ )
  { //Get all parameters 10 times, then average
    xValue += myIMU.readFloatAccelX();
    yValue += myIMU.readFloatAccelY();
    zValue += myIMU.readFloatAccelZ();
    delay(2);
  }
  xValue = xValue / 10;
  yValue = yValue / 10;
  zValue = zValue / 10;
  
  if ( xValue < 0 ) xValue = xValue * float(-1);
  if ( xValue > 4 ) xValue = 1;
  if ( yValue < 0 ) yValue = yValue * float(-1);
  if ( yValue > 4 ) yValue = 1;
  if ( zValue < 0 ) zValue = zValue * float(-1);
  if ( zValue > 4 ) zValue = 1;

  uint8_t xPWM = xValue * 63;
  uint8_t yPWM = yValue * 63;
  uint8_t zPWM = zValue * 63;

  analogWrite(REDPIN, myLedTable.ledAdjust(xPWM));
  analogWrite(GREENPIN, myLedTable.ledAdjust(yPWM));
  analogWrite(BLUEPIN, myLedTable.ledAdjust(zPWM));

}
