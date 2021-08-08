//
//    FILE: unit_test_001.cpp
//  AUTHOR: Rob Tillaart
//    DATE: 2020-12-03
// PURPOSE: unit tests for the Cozir CO2 library
//          https://github.com/RobTillaart/Cozir
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

// supported assertions
// https://github.com/Arduino-CI/arduino_ci/blob/master/cpp/unittest/Assertion.h#L33-L42
// ----------------------------
// assertEqual(expected, actual);               // a == b
// assertNotEqual(unwanted, actual);            // a != b
// assertComparativeEquivalent(expected, actual);    // abs(a - b) == 0 or (!(a > b) && !(a < b))
// assertComparativeNotEquivalent(unwanted, actual); // abs(a - b) > 0  or ((a > b) || (a < b))
// assertLess(upperBound, actual);              // a < b
// assertMore(lowerBound, actual);              // a > b
// assertLessOrEqual(upperBound, actual);       // a <= b
// assertMoreOrEqual(lowerBound, actual);       // a >= b
// assertTrue(actual);
// assertFalse(actual);
// assertNull(actual);

// // special cases for floats
// assertEqualFloat(expected, actual, epsilon);    // fabs(a - b) <= epsilon
// assertNotEqualFloat(unwanted, actual, epsilon); // fabs(a - b) >= epsilon
// assertInfinity(actual);                         // isinf(a)
// assertNotInfinity(actual);                      // !isinf(a)
// assertNAN(arg);                                 // isnan(a)
// assertNotNAN(arg);                              // !isnan(a)


#include <ArduinoUnitTests.h>


#include "Arduino.h"
#include "cozir.h"
#include "SoftwareSerial.h"


// NOTE: normally the COZIR lib is tested with software serial, at least in sketches
// to get the unit test up and running I (tried to) use Serial here.

// aspects of software serial
bool bigEndian = false;

unittest_setup()
{
  fprintf(stderr, "\n");
}

unittest_teardown()
{
}


unittest(test_software_serial)
{
  int receivePin = 4;
  int transmitPin = 5;

  GodmodeState* state = GODMODE();

  SoftwareSerial sws(receivePin, transmitPin);
  COZIR co(&sws);

  fprintf(stderr, "COZIR.init()\n");
  co.init();
  assertEqual("K 2\r\n", state->digitalPin[transmitPin].toAscii(1, bigEndian));
  delay(1300);
  assertTrue(co.isInitialized());
}


unittest(test_constructor)
{
  GodmodeState* state = GODMODE();

  state->serialPort[0].dataIn = "";             // the queue of data waiting to be read
  state->serialPort[0].dataOut = "";            // the history of data written

  fprintf(stderr, "default there is no data on the Serial bus\n");
  assertEqual(-1, Serial.peek());
  assertEqual("", state->serialPort[0].dataIn);
  assertEqual("", state->serialPort[0].dataOut);

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.getVersionSerial()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.getVersionSerial();
  assertEqual("K 0\r\nY\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.getConfiguration()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.getConfiguration();
  assertEqual("K 0\r\n*\r\n", state->serialPort[0].dataOut);
}


unittest(test_setOperatingMode)
{
  GodmodeState* state = GODMODE();

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.init();
  // init() sets CZR_POLLING
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);
  assertEqual(co.getOperatingMode(), CZR_POLLING);

  fprintf(stderr, "COZIR.SetOperatingMode(CZR_COMMAND)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.setOperatingMode(CZR_COMMAND);
  assertEqual("K 0\r\n", state->serialPort[0].dataOut);
  assertEqual(co.getOperatingMode(), CZR_COMMAND);

  fprintf(stderr, "COZIR.SetOperatingMode(CZR_STREAMING)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.setOperatingMode(CZR_STREAMING);
  assertEqual("K 1\r\n", state->serialPort[0].dataOut);
  assertEqual(co.getOperatingMode(), CZR_STREAMING);

  fprintf(stderr, "COZIR.SetOperatingMode(CZR_POLLING)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.setOperatingMode(CZR_POLLING);
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);
  assertEqual(co.getOperatingMode(), CZR_POLLING);
}


unittest(test_read_sensor)
{
   GodmodeState* state = GODMODE();

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.Celsius()\n");
  state->serialPort[0].dataIn = "T    750\r\n";   // 1000 = 0°C
  state->serialPort[0].dataOut = "";
  float celsius = co.celsius();
  assertEqual("T\r\n", state->serialPort[0].dataOut);
  assertEqualFloat(-25.0, celsius, 0.0001);

  state->serialPort[0].dataIn = "T   1257\r\n";
  state->serialPort[0].dataOut = "";
  celsius = co.celsius();
  assertEqual("T\r\n", state->serialPort[0].dataOut);
  assertEqualFloat(25.7, celsius, 0.0001);

  fprintf(stderr, "COZIR.humidity()\n");
  state->serialPort[0].dataIn = "H 627\r\n";
  state->serialPort[0].dataOut = "";
  float humidity = co.humidity();
  assertEqual("H\r\n", state->serialPort[0].dataOut);
  assertEqualFloat(62.7, humidity, 0.0001);

  fprintf(stderr, "COZIR.light()\n");
  state->serialPort[0].dataIn = "L 189\r\n";
  state->serialPort[0].dataOut = "";
  float light = co.light();
  assertEqual("L\r\n", state->serialPort[0].dataOut);
  assertEqual(189, light);

  fprintf(stderr, "COZIR.CO2()\n");
  state->serialPort[0].dataIn = "Z 432\r\n";
  state->serialPort[0].dataOut = "";
  float CO2 = co.CO2();
  assertEqual("Z\r\n", state->serialPort[0].dataOut);
  assertEqual(432, CO2);
}


unittest(test_calibrate)
{
  GodmodeState* state = GODMODE();

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.fineTuneZeroPoint(400, 382)\n");
  state->serialPort[0].dataIn = "F 32950\r\n";
  state->serialPort[0].dataOut = "";
  uint16_t fineTuneZeroPoint = co.fineTuneZeroPoint(400, 382);
  assertEqual("F 400 382\r\n", state->serialPort[0].dataOut);
  assertEqual(32950, fineTuneZeroPoint);


  fprintf(stderr, "COZIR.calibrateFreshAir()\n");
  state->serialPort[0].dataIn = "G 32950\r\n";
  state->serialPort[0].dataOut = "";
  uint16_t calibrateFreshAir = co.calibrateFreshAir();
  assertEqual("G\r\n", state->serialPort[0].dataOut);
  assertEqual(32950, calibrateFreshAir);

  fprintf(stderr, "COZIR.calibrateNitrogen()\n");
  state->serialPort[0].dataIn = "U 32590\r\n";
  state->serialPort[0].dataOut = "";
  uint16_t calibrateNitrogen = co.calibrateNitrogen();
  assertEqual("U\r\n", state->serialPort[0].dataOut);
  assertEqual(32590, calibrateNitrogen);

  fprintf(stderr, "COZIR.calibrateKnownGas(100)\n");
  state->serialPort[0].dataIn = "X 33012\r\n";
  state->serialPort[0].dataOut = "";
  uint16_t calibrateKnownGas = co.calibrateKnownGas(100);
  assertEqual("X 100\r\n", state->serialPort[0].dataOut);
  assertEqual(33012, calibrateKnownGas);

}


unittest(test_digi_filter)
{
  GodmodeState* state = GODMODE();

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.setDigiFilter(42)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.setDigiFilter(42);
  assertEqual("A 42\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.getDigiFilter()\n");
  state->serialPort[0].dataIn = "a 42\r\n";
  state->serialPort[0].dataOut = "";
  uint8_t digifilter = co.getDigiFilter();
  assertEqual("a\r\n", state->serialPort[0].dataOut);
  assertEqual(42, digifilter);
}


unittest(test_streaming_mode)
{
  GodmodeState* state = GODMODE();

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);
  assertEqual(co.getOutputFields(), CZR_NONE);

  fprintf(stderr, "COZIR.setOperatingMode(CZR_STREAMING)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.setOperatingMode(CZR_STREAMING);
  assertEqual("K 1\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.setOutputFields(CZR_HUMIDITY | CZR_RAWTEMP | CZR_RAWCO2)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.setOutputFields(CZR_HUMIDITY | CZR_RAWTEMP | CZR_RAWCO2);
  assertEqual("M 4226\r\n", state->serialPort[0].dataOut);
  assertEqual(co.getOutputFields(), CZR_HTC);

  fprintf(stderr, "COZIR.inOutputFields()\n");
  assertTrue(co.inOutputFields(CZR_HUMIDITY));
  assertTrue(co.inOutputFields(CZR_RAWTEMP));
  assertTrue(co.inOutputFields(CZR_RAWCO2));
  assertFalse(co.inOutputFields(CZR_LIGHT));
  assertFalse(co.inOutputFields(~CZR_HTC));

  fprintf(stderr, "COZIR.getRecentFields()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.getRecentFields();
  assertEqual("Q\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.clearOutputFields()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.clearOutputFields();
  assertEqual("M 1\r\n", state->serialPort[0].dataOut);
  fprintf(stderr, "COZIR.inOutputFields()\n");
  assertTrue(co.inOutputFields(CZR_NONE));
  assertFalse(co.inOutputFields(~CZR_NONE));

  fprintf(stderr, "COZIR.getRecentFields()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.getRecentFields();
  assertEqual("Q\r\n", state->serialPort[0].dataOut);
  // USER MUST READ RETURNED DATA AS IT CAN BE QUITE A LOT.
}


unittest(test_eeprom_I)
{
  GodmodeState* state = GODMODE();

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);

  // needs redo
  fprintf(stderr, "COZIR._setEEPROM(10, 42)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co._setEEPROM(10, 42);
  assertEqual("P 10 42\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR._getEEPROM(100)\n");
  state->serialPort[0].dataIn = "p 42\r\n";
  state->serialPort[0].dataOut = "";
  uint8_t getEEPROM = co._getEEPROM(100);
  assertEqual("p 100\r\n", state->serialPort[0].dataOut);
  assertEqual(42, getEEPROM);
}


unittest(test_eeprom_II)
{
  GodmodeState* state = GODMODE();

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.setAutoCalibrationPreload()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.setAutoCalibrationPreload(123);
  assertEqual("P 3 0\r\nP 4 123\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.getAutoCalibrationPreload()\n");
  state->serialPort[0].dataIn = "p 01\r\np 42\r\n";
  state->serialPort[0].dataOut = "";
  uint16_t ACP = co.getAutoCalibrationPreload();
  assertEqual("p 3\r\np 3\r\n", state->serialPort[0].dataOut);
  assertEqual(298, ACP);

/*
  // TODO
  co.setAutoCalibrationOn();
  co.setAutoCalibrationOff();
  co.getAutoCalibration();
  co.setAutoCalibrationBackgroundConcentration(uint16_t value);
  co.getAutoCalibrationBackgroundConcentration();
  co.setAmbientConcentration(uint16_t value);
  co.getAmbientConcentration();
  co.setBufferClearTime(uint16_t value);
  co.getBufferClearTime();
*/
}



unittest(test_PPM)
{
  GodmodeState* state = GODMODE();

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.getPPMFactor()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.getPPMFactor();
  assertEqual(".\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "\n===========================================\n\n");
}



unittest_main()

// --------
