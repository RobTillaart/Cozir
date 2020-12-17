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
}

unittest_teardown()
{
}


unittest(test_constructor)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";             // the queue of data waiting to be read
  state->serialPort[0].dataOut = "";            // the history of data written

  fprintf(stderr, "\ndefault there is no data on the Serial bus\n");
  assertEqual(-1, Serial.peek());
  assertEqual("", state->serialPort[0].dataIn);
  assertEqual("", state->serialPort[0].dataOut);

  COZIR co(&Serial);

  fprintf(stderr, "\nCalling COZIR.init()\n");
  co.init();
  assertEqual(-1, Serial.peek());
  assertEqual("", state->serialPort[0].dataIn);
  assertEqual("K 2", state->serialPort[0].dataOut);
  
  fprintf(stderr, "\nGetVersionSerial\n");
  co.GetVersionSerial();
  assertEqual("Y", state->serialPort[0].dataOut);
  
  fprintf(stderr, "\nGetConfiguration\n");
  co.GetConfiguration();
  assertEqual("*", state->serialPort[0].dataOut);
}

unittest(test_setOperatingMode)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";             // the queue of data waiting to be read
  state->serialPort[0].dataOut = "";            // the history of data written

  COZIR co(&Serial);

  fprintf(stderr, "\nCalling COZIR.init()\n");
  co.init();
  assertEqual("K 2", state->serialPort[0].dataOut);
  
  fprintf(stderr, "\nSetOperatingMode(CZR_COMMAND)\n");
  co.SetOperatingMode(CZR_COMMAND);
  assertEqual("K 0", state->serialPort[0].dataOut);
  
  fprintf(stderr, "\nSetOperatingMode(CZR_STREAMING)\n");
  co.SetOperatingMode(CZR_STREAMING);
  assertEqual("K 1", state->serialPort[0].dataOut);
  
  fprintf(stderr, "\nSetOperatingMode(CZR_POLLING)\n");
  co.SetOperatingMode(CZR_POLLING);
  assertEqual("K 2", state->serialPort[0].dataOut);
}

unittest(test_read_sensor)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";             // the queue of data waiting to be read
  state->serialPort[0].dataOut = "";            // the history of data written

  COZIR co(&Serial);

  fprintf(stderr, "\nCalling COZIR.init()\n");
  co.init();
  assertEqual("K 2", state->serialPort[0].dataOut);
  
  float Celsius = co.Celsius();
  assertEqual("K 0", state->serialPort[0].dataOut);
  
  float Humidity = co.Humidity();
  assertEqual("K 0", state->serialPort[0].dataOut);

  float Light = co.Light();
  assertEqual("K 0", state->serialPort[0].dataOut);

  float CO2 = co.CO2();
  assertEqual("K 0", state->serialPort[0].dataOut);
}


unittest(test_read_sensor)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";             // the queue of data waiting to be read
  state->serialPort[0].dataOut = "";            // the history of data written

  COZIR co(&Serial);

  fprintf(stderr, "\nCalling COZIR.init()\n");
  co.init();
  assertEqual("K 2", state->serialPort[0].dataOut);
  
  uint16_t FineTuneZeroPoint = co.FineTuneZeroPoint(100, 50);
  assertEqual("F 100 50", state->serialPort[0].dataOut);
  
  uint16_t CalibrateFreshAir = co.CalibrateFreshAir();
  assertEqual("G", state->serialPort[0].dataOut);

  uint16_t CalibrateNitrogen = co.CalibrateNitrogen();
  assertEqual("U", state->serialPort[0].dataOut);

  uint16_t CalibrateKnownGas = co.CalibrateKnownGas(100);
  assertEqual("X 100", state->serialPort[0].dataOut);
}


unittest(test_digi_filter)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";             // the queue of data waiting to be read
  state->serialPort[0].dataOut = "";            // the history of data written

  COZIR co(&Serial);

  fprintf(stderr, "\nCalling COZIR.init()\n");
  co.init();
  assertEqual("K 2", state->serialPort[0].dataOut);
  
  co.SetDigiFilter(42);
  assertEqual("A 42", state->serialPort[0].dataOut);
  
  uint8_t digifilter = co.GetDigiFilter();
  assertEqual("a", state->serialPort[0].dataOut);
}


unittest(test_streaming_mode)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";             // the queue of data waiting to be read
  state->serialPort[0].dataOut = "";            // the history of data written

  COZIR co(&Serial);

  fprintf(stderr, "\nCalling COZIR.init()\n");
  co.init();
  fprintf(stderr, "\nSetOperatingMode(CZR_STREAMING)\n");
  co.SetOperatingMode(CZR_STREAMING);
  assertEqual("K 1", state->serialPort[0].dataOut);
  
  co.SetOutputFields(CZR_HUMIDITY | CZR_RAWTEMP | CZR_RAWCO2);
  assertEqual("M 4226", state->serialPort[0].dataOut);
  
  co.GetRecentFields();
  assertEqual("Q", state->serialPort[0].dataOut);
}

unittest(test_eeprom)
{
  GodmodeState* state = GODMODE();
  state->serialPort[0].dataIn = "";             // the queue of data waiting to be read
  state->serialPort[0].dataOut = "";            // the history of data written

  COZIR co(&Serial);

  fprintf(stderr, "\nCalling COZIR.init()\n");
  co.init();
  
  co.SetEEPROM(100, 42);
  assertEqual("P 100 42", state->serialPort[0].dataOut);
  
  uint8t val = co.GetEEPROM(100);
  assertEqual("p 100", state->serialPort[0].dataOut);
}





unittest_main()

// --------
