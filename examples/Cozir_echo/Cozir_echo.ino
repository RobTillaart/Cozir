//
//    FILE: Cozir_echo.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo of Cozir sensor
//     URL: https://github.com/RobTillaart/Cozir
//          http://forum.arduino.cc/index.php?topic=91467.0
//

// Note: this sketch needs a MEGA or a Teensy that supports a second
//       Serial port named Serial1
// The purpose of this sketch is to talk lowest level to the COZIR sensor.
// one can send the low level commands (datasheet) directly to the device.
// Note: this can cause (unwanted) side effects, so use with care.


#include "Arduino.h"
#include "cozir.h"    // not perse needed

COZIR czr(&Serial1);


void setup()
{
  Serial1.begin(9600);

  Serial.begin(9600);
  Serial.println(__FILE__);
  Serial.println();

}


void loop()
{
  if (Serial.available())
  {
    Serial1.write(Serial.read());
  }
  if (Serial1.available())
  {
    Serial.write(Serial1.read());
  }
}


// -- END OF FILE --