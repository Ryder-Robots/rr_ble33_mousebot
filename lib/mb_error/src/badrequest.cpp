// Copyright (c) 2025 Ryder Robots
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <badrequest.hpp>

// int bad_request(org_ryderrobots_ros2_serial_ErrorType etype, uint8_t *buf, size_t bsz) {
//   org_ryderrobots_ros2_serial_BadRequest bad_request =
//       org_ryderrobots_ros2_serial_BadRequest_init_zero;
//   bad_request.etype = org_ryderrobots_ros2_serial_ErrorType_ET_MAX_LEN_EXCEED;
//   org_ryderrobots_ros2_serial_Response response =
//       org_ryderrobots_ros2_serial_Response_init_zero;
//   response.op = BAD_REQUEST;
//   response.data.bad_request = bad_request;

//   pb_ostream_t ostream = pb_ostream_from_buffer(buf, bsz);
//   pb_encode(&ostream, &org_ryderrobots_ros2_serial_Response_msg, &response);
//   return 0;
// }