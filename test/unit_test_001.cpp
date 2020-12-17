//
//    FILE: unit_test_001.cpp
//  AUTHOR: Rob Tillaart
//    DATE: 2020-12-03
// PURPOSE: unit tests for the Cozir CO2 library
//          https://github.com/RobTillaart/
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

// supported assertions
// https://github.com/Arduino-CI/arduino_ci/blob/master/cpp/unittest/Assertion.h#L33-L42
// ----------------------------
// assertEqual(expected, actual)
// assertNotEqual(expected, actual)
// assertLess(expected, actual)
// assertMore(expected, actual)
// assertLessOrEqual(expected, actual)
// assertMoreOrEqual(expected, actual)
// assertTrue(actual)
// assertFalse(actual)
// assertNull(actual)
// assertNotNull(actual)

#include <ArduinoUnitTests.h>

#define assertEqualFloat(arg1, arg2, arg3)  assertOp("assertEqualFloat", "expected", fabs(arg1 - arg2), compareLessOrEqual, "<=", "actual", arg3)
#define assertEqualINF(arg)  assertOp("assertEqualINF", "expected", INFINITY, compareEqual, "==", "actual", arg)
#define assertEqualNAN(arg)  assertOp("assertEqualNAN", "expected", true, compareEqual, "==", "actual", isnan(arg))


#include "Arduino.h"
#include "cozir.h"

// NOTE: normally the COZIR lib is tested with software serial, at least in sketches
// to get the unit test up and running I (tried to) use Serial here.


unittest_setup()
{
  fprintf(stderr, "\n");
}

unittest_teardown()
{
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
  
  fprintf(stderr, "COZIR.GetVersionSerial()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.GetVersionSerial();
  assertEqual("Y\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.GetConfiguration()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.GetConfiguration();
  assertEqual("*\r\n", state->serialPort[0].dataOut);
}


unittest(test_setOperatingMode)
{
  GodmodeState* state = GODMODE();

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.SetOperatingMode(CZR_COMMAND)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.SetOperatingMode(CZR_COMMAND);
  assertEqual("K 0\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.SetOperatingMode(CZR_STREAMING)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.SetOperatingMode(CZR_STREAMING);
  assertEqual("K 1\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.SetOperatingMode(CZR_POLLING)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.SetOperatingMode(CZR_POLLING);
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);
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
  state->serialPort[0].dataIn = "T    500\r\n";
  state->serialPort[0].dataOut = "";
  float Celsius = co.Celsius();
  assertEqual("T\r\n", state->serialPort[0].dataOut);
  assertEqual(50.0, Celsius);
  
  state->serialPort[0].dataIn = "T   1257\r\n";
  state->serialPort[0].dataOut = "";
  Celsius = co.Celsius();
  assertEqual("T\r\n", state->serialPort[0].dataOut);
  assertEqual(-25.7, Celsius);
  
  fprintf(stderr, "COZIR.Humidity()\n");
  state->serialPort[0].dataIn = "H 627\r\n";
  state->serialPort[0].dataOut = "";
  float Humidity = co.Humidity();
  assertEqual("H\r\n", state->serialPort[0].dataOut);
  assertEqual(62.7, Humidity);

  fprintf(stderr, "COZIR.Light()\n");
  state->serialPort[0].dataIn = "L 189\r\n";
  state->serialPort[0].dataOut = "";
  float Light = co.Light();
  assertEqual("L\r\n", state->serialPort[0].dataOut);
  assertEqual(189, Light);

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
  
  fprintf(stderr, "COZIR.FineTuneZeroPoint(400, 382)\n");
  state->serialPort[0].dataIn = "32950\r\n";
  state->serialPort[0].dataOut = "";
  uint16_t FineTuneZeroPoint = co.FineTuneZeroPoint(400, 382);
  assertEqual("F 100 50", state->serialPort[0].dataOut);
  assertEqual(32950, FineTuneZeroPoint);
  
  
  fprintf(stderr, "COZIR.CalibrateFreshAir()\n");
  state->serialPort[0].dataIn = "G 32950\r\n";
  state->serialPort[0].dataOut = "";
  uint16_t CalibrateFreshAir = co.CalibrateFreshAir();
  assertEqual("G\r\n", state->serialPort[0].dataOut);
  assertEqual(32950, CalibrateFreshAir);

  fprintf(stderr, "COZIR.CalibrateNitrogen()\n");
  state->serialPort[0].dataIn = "U 32590\r\n";
  state->serialPort[0].dataOut = "";
  uint16_t CalibrateNitrogen = co.CalibrateNitrogen();
  assertEqual("U\r\n", state->serialPort[0].dataOut);
  assertEqual(32590, CalibrateNitrogen);

  fprintf(stderr, "COZIR.CalibrateKnownGas(100)\n");
  state->serialPort[0].dataIn = "X 33012\r\n";
  state->serialPort[0].dataOut = "";
  uint16_t CalibrateKnownGas = co.CalibrateKnownGas(100);
  assertEqual("X 100\r\n", state->serialPort[0].dataOut);
  assertEqual(33012, CalibrateKnownGas);
  
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
  
  fprintf(stderr, "COZIR.SetDigiFilter(42)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.SetDigiFilter(42);
  assertEqual("A 42\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.GetDigiFilter()\n");
  state->serialPort[0].dataIn = "a 42\r\n";
  state->serialPort[0].dataOut = "";
  uint8_t digifilter = co.GetDigiFilter();
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

  fprintf(stderr, "COZIR.SetOperatingMode(CZR_STREAMING)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.SetOperatingMode(CZR_STREAMING);
  assertEqual("K 1\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.SetOutputFields(CZR_HUMIDITY | CZR_RAWTEMP | CZR_RAWCO2)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.SetOutputFields(CZR_HUMIDITY | CZR_RAWTEMP | CZR_RAWCO2);
  assertEqual("M 4226\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.GetRecentFields()\n");
  state->serialPort[0].dataIn = "Q 4226\r\n";
  state->serialPort[0].dataOut = "";
  uint16_t GetRecentFields = co.GetRecentFields();
  assertEqual("Q\r\n", state->serialPort[0].dataOut);
  assertEqual(4226, GetRecentFields);
}


unittest(test_eeprom)
{
  GodmodeState* state = GODMODE();

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.SetEEPROM(100, 42)\n");
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";
  co.SetEEPROM(100, 42);
  assertEqual("P 100 42\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.GetEEPROM(100)\n");
  state->serialPort[0].dataIn = "Q 4226\r\n";
  state->serialPort[0].dataOut = "";
  uint8_t GetEEPROM = co.GetEEPROM(100);
  assertEqual("p 100\r\n", state->serialPort[0].dataOut);
  assertEqual(4226, GetEEPROM);
}



unittest_main()

// --------
