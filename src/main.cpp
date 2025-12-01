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
  Serial.begin(115200);

  // Create initial
  for (int i = 0; i < 500; i++)
  {
    if (Serial)
    {
      break;
    }
    delay(100);
  }
}

void loop()
{
  uint8_t buffer[BUFSIZ];

  // initilize output buffer.
  pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  pb_istream_t istream;

  if (Serial.available())
  {
    // Do something here,
    byte buf[BUFSIZ];
    int msize = Serial.readBytesUntil(TERM_CHAR, buf, BUFSIZ);

    // Read bytes from input, if they exceed reserved buffer size then 
    // throw an error.
    if (msize == 0 || buf[msize - 1] != TERM_CHAR)
    {
      
      org_ryderrobots_ros2_serial_BadRequest bad_request = org_ryderrobots_ros2_serial_BadRequest_init_zero;
      bad_request.etype = org_ryderrobots_ros2_serial_ErrorType_ET_MAX_LEN_EXCEED;
      org_ryderrobots_ros2_serial_Response response = org_ryderrobots_ros2_serial_Response_init_zero;
      response.op = BAD_REQUEST;
      response.data.bad_request = bad_request;
      if (!pb_encode(&ostream, &org_ryderrobots_ros2_serial_Response_msg, &response)) {
        // not sure what to do if the error message fails.
      }

      Serial.write(buffer, ostream.bytes_written);
      return;
    }

    istream = pb_istream_from_buffer(buf, msize);
    org_ryderrobots_ros2_serial_Request request = org_ryderrobots_ros2_serial_Request_init_zero;
    if (!pb_decode(&istream, org_ryderrobots_ros2_serial_Request_fields, &request)) {
      // throw error
      return;
    }

    // Use factory object, C version of one, and start processing the message

    // Write response back to serial port.
  }
}
