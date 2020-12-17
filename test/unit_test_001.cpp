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
  co.init();
  assertEqual(-1, Serial.peek());
  assertEqual("", state->serialPort[0].dataIn);
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.GetVersionSerial()\n");
  co.GetVersionSerial();
  assertEqual("Y\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.GetConfiguration()\n");
  co.GetConfiguration();
  assertEqual("*\r\n", state->serialPort[0].dataOut);
}


unittest(test_setOperatingMode)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";             // the queue of data waiting to be read
  state->serialPort[0].dataOut = "";            // the history of data written

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.SetOperatingMode(CZR_COMMAND)\n");
  co.SetOperatingMode(CZR_COMMAND);
  assertEqual("K 0\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.SetOperatingMode(CZR_STREAMING)\n");
  co.SetOperatingMode(CZR_STREAMING);
  assertEqual("K 1\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.SetOperatingMode(CZR_POLLING)\n");
  co.SetOperatingMode(CZR_POLLING);
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);
}


unittest(test_read_sensor)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";             // the queue of data waiting to be read
  state->serialPort[0].dataOut = "";            // the history of data written

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.Celsius()\n");
  float Celsius = co.Celsius();
  assertEqual("T\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.Humidity()\n");
  float Humidity = co.Humidity();
  assertEqual("H\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.Light()\n");
  float Light = co.Light();
  assertEqual("L\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.CO2()\n");
  float CO2 = co.CO2();
  assertEqual("Z\r\n", state->serialPort[0].dataOut);
}


unittest(test_calibrate)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";             // the queue of data waiting to be read
  state->serialPort[0].dataOut = "";            // the history of data written

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.FineTuneZeroPoint(100, 50)\n");
  uint16_t FineTuneZeroPoint = co.FineTuneZeroPoint(100, 50);
  assertEqual("F 100 50", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.CalibrateFreshAir()\n");
  uint16_t CalibrateFreshAir = co.CalibrateFreshAir();
  assertEqual("G\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.CalibrateNitrogen()\n");
  uint16_t CalibrateNitrogen = co.CalibrateNitrogen();
  assertEqual("U\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.CalibrateKnownGas(100)\n");
  uint16_t CalibrateKnownGas = co.CalibrateKnownGas(100);
  assertEqual("X 100\r\n", state->serialPort[0].dataOut);
}


unittest(test_digi_filter)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";

  COZIR co(&Serial);

  fprintf(stderr, "COZIR.init()\n");
  co.init();
  assertEqual("K 2\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.SetDigiFilter(42)\n");
  co.SetDigiFilter(42);
  assertEqual("A 42\r\n", state->serialPort[0].dataOut);
  
  fprintf(stderr, "COZIR.GetDigiFilter()\n");
  uint8_t digifilter = co.GetDigiFilter();
  assertEqual("a\r\n", state->serialPort[0].dataOut);
}


unittest(test_streaming_mode)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";
  state->serialPort[0].dataOut = "";

  COZIR co(&Serial);
  co.init();

  fprintf(stderr, "COZIR.SetOperatingMode(CZR_STREAMING)\n");
  co.SetOperatingMode(CZR_STREAMING);
  assertEqual("K 1\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.SetOutputFields(CZR_HUMIDITY | CZR_RAWTEMP | CZR_RAWCO2)\n");
  co.SetOutputFields(CZR_HUMIDITY | CZR_RAWTEMP | CZR_RAWCO2);
  assertEqual("M 4226\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.GetRecentFields()\n");
  co.GetRecentFields();
  assertEqual("Q\r\n", state->serialPort[0].dataOut);
}


unittest(test_eeprom)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";             // the queue of data waiting to be read
  state->serialPort[0].dataOut = "";            // the history of data written

  COZIR co(&Serial);
  co.init();

  fprintf(stderr, "COZIR.SetEEPROM(100, 42)\n");
  co.SetEEPROM(100, 42);
  assertEqual("P 100 42\r\n", state->serialPort[0].dataOut);

  fprintf(stderr, "COZIR.GetEEPROM(100)\n");
  uint8_t val = co.GetEEPROM(100);
  assertEqual("p 100\r\n", state->serialPort[0].dataOut);
}



unittest_main()

// --------
