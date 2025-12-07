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

const unsigned long LOOP_INTERVAL = 10;

mb_operations::MBOperationsFactory fact;

/**
 * Read bytes up to TERM_CHAR, or when -1 is returned.
 *
 * returns the number of bytes.
 *
 * ibuf() from buffer singleton will be populated with one of these.
 *
 * all bytes up to return value, with last value as TERM_CHAR
 * all bytes up to return value, with last value as -1
 * all bytes up to return value, where ibuf = BUFSIZ
 */
size_t read_serial()
{
  size_t r = 0;
  if (Serial.available())
  {
    int c;
    auto &buf = rr_buffer::RRBuffer::get_instance();
    for (; r < BUFSIZ; r++)
    {
      c = Serial.read();
      buf.ibuf_ptr()[r] = static_cast<std::uint8_t>(c);
      if (c < 0 || static_cast<std::uint8_t>(c) == TERM_CHAR)
      {
        break;
      }

      if (!Serial.available())
      {
        break;
      }
    }
  }
  return r;
}

void setup()
{
  // reserve memory early to stop potential issues later
  rr_buffer::RRBuffer &buf = rr_buffer::RRBuffer::get_instance();
  (void)buf;

  fact.init();

  // start serial driver.
  Serial.begin(BAUD_RATE);

  // create watchdog
  wdt::Wdt::get_instance().init();
}

// Called from within a loop, it will block while data is not available.
// but does not consider processing time.
void loop()
{
  wdt::Wdt::get_instance().reset();

  static unsigned long last_serial = 0;
  if (millis() - last_serial < 5)
    return;
  last_serial = millis();

  if (Serial.available() == 0)
  {
    return;
  }
  // read input
  auto &buf = rr_buffer::RRBuffer::get_instance();
  auto ostream = pb_ostream_from_buffer(buf.obuf_ptr(), BUFSIZ);

  size_t bytes_read = read_serial();
  if (bytes_read == 0)
  {
    buf.clear();
    return;
  }

  org_ryderrobots_ros2_serial_ErrorType etype;
  if (bytes_read == BUFSIZ && buf.ibuf_ptr()[BUFSIZ - 1] != TERM_CHAR)
  {
    // return back error code too big
    mberror::RRBadRequest rr_bad_request(ostream);
    etype = org_ryderrobots_ros2_serial_ErrorType_ET_MAX_LEN_EXCEED;
    size_t result = rr_bad_request.serialize(etype);
    if (result > 0)
    {
      Serial.write(buf.obuf_ptr(), result);
      Serial.write(TERM_CHAR);
    }
    buf.clear();
    return;
  }

  auto istream = pb_istream_from_buffer(buf.ibuf_ptr(), bytes_read);
  org_ryderrobots_ros2_serial_Request req = org_ryderrobots_ros2_serial_Request_init_zero;
  if (!pb_decode(&istream, org_ryderrobots_ros2_serial_Request_fields, &req))
  {
    // operation can not be deserialized.
    mberror::RRBadRequest rr_bad_request(ostream);
    etype = org_ryderrobots_ros2_serial_ErrorType_ET_INVALID_REQUEST;
    size_t result = rr_bad_request.serialize(etype);
    if (result > 0)
    {
      Serial.write(buf.obuf_ptr(), result);
      Serial.write(TERM_CHAR);
    }
    buf.clear();
    return;
  }

  auto status = org_ryderrobots_ros2_serial_Status::org_ryderrobots_ros2_serial_Status_UNKNOWN;
  mb_operations::MbOperationHandler *handler = fact.get_op_handler(req, status);

  if (status == org_ryderrobots_ros2_serial_Status::org_ryderrobots_ros2_serial_Status_READY)
  {
    if (!pb_encode(&ostream, org_ryderrobots_ros2_serial_Response_fields, &handler->perform_op(req)))
    {
      // operation can not be deserialized.
      mberror::RRBadRequest rr_bad_request(ostream);
      etype = org_ryderrobots_ros2_serial_ErrorType_ET_UNKNOWN;
      size_t result = rr_bad_request.serialize(etype);
      if (result > 0)
      {
        Serial.write(buf.obuf_ptr(), result);
        Serial.write(TERM_CHAR);
      }
      buf.clear();
      return;
    }
    Serial.write(buf.obuf_ptr(), ostream.bytes_written);
    Serial.write(TERM_CHAR);
  }
  else
  {
    mberror::RRBadRequest rr_bad_request(ostream);
    etype = org_ryderrobots_ros2_serial_ErrorType_ET_SERIAL_FAILURE;
    size_t result = rr_bad_request.serialize(etype);
    if (result > 0)
    {
      Serial.write(buf.obuf_ptr(), result);
      Serial.write(TERM_CHAR);
    }
  }

  // This must be the last line of the loop.
  buf.clear();
}
