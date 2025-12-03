// Copyright (c) 2025 Ryder Robots
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/**
 * ### Commands
 * | ID      | CONSTANT        |  SENSOR   | DESCRIPTION              |
 * | ------  | --------------  | --------- | ------------------------ |
 * | 200     | MSP_SET_RAW_RC. | Motors.   | Sets motors              |
 *
 *
 * ### Monitor Commands
 *
 * | ID.     | CONSTANT        |  SENSOR   | DESCRIPTION              |
 * | ------  | --------------  | --------- | ------------------------ |
 * | 102     | MSP_RAW_IMU.    | IMU       | Monitor IMU details      |
 * | 104     | MSP_MOTOR       | MOTORS    | Set, or monitor motors.  |
 * | 105     | MSP_RAW_SENSORS | Range     | Range sensors            |
 */

#include "rr_ble_mousebot.h"

void setup()
{
  // start serial driver.
  // Serial.begin(115200);
}


void loop()
{
}


