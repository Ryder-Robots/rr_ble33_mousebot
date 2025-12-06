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

// reserve memory for protobuf, and error messages.
// keep as much memory reserved as possible, so that the chip stays stable.
int result;
mberror::RRBadRequest rr_bad_request;

void setup()
{
  // reserve memory early to stop potential issues later
  rr_buffer::RRBuffer &buf = rr_buffer::RRBuffer::get_instance();
  (void)buf;

  // start serial driver.
  Serial.begin(BAUD_RATE);
}

// Called from within a loop, it will block while data is not available.
// but does not consider processing time.
void loop()
{
  if (Serial.available() == 0)
  {
    delay(DELAY_COEF);
    return;
  }

  // get buffer instance, and clear any junk away.
  rr_buffer::RRBuffer &buf = rr_buffer::RRBuffer::get_instance();
  buf.clear();
  pb_istream_t istream;
  org_ryderrobots_ros2_serial_Response decoded_response;
  org_ryderrobots_ros2_serial_ErrorType etype;

  // Read input.
  size_t bytes_read = Serial.readBytesUntil(TERM_CHAR, buf.buffer(), BUFSIZ);
  if (bytes_read == BUFSIZ && buf.buffer()[BUFSIZ - 1] != TERM_CHAR)
  {
    // return back error code too big
    etype = org_ryderrobots_ros2_serial_ErrorType_ET_MAX_LEN_EXCEED;
    decoded_response = org_ryderrobots_ros2_serial_Response_init_zero;
    result = rr_bad_request.serialize(buf.buffer(), etype);
    istream = pb_istream_from_buffer(buf.buffer(), bytes_read);
    if (result > 0)
    {
      Serial.write(buf.buffer(), result);
      Serial.write(TERM_CHAR);
    }
    delay(DELAY_COEF);
    return;
  }

  // This must be the last line of the loop.
  delay(DELAY_COEF);
}
