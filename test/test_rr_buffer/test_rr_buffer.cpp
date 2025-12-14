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

#include <unity.h>

#include <rr_buffer.hpp>
#include <mberror.hpp>

using namespace rr_buffer;
using namespace mberror;

/**
 * same unit test as mb_error, but uses the buffer.
 */
void test_bad_request_buf(void)
{
    RRBuffer &buf = RRBuffer::get_instance();
    pb_ostream_t ostream = pb_ostream_from_buffer(buf.obuf_ptr(), BUFSIZ);
    RRBadRequest rr_bad_request(ostream);
    org_ryderrobots_ros2_serial_ErrorType test_etype = org_ryderrobots_ros2_serial_ErrorType_ET_MAX_LEN_EXCEED;

    int result = rr_bad_request.serialize(test_etype);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(0, buf.obuf_ptr()[0]); // Verify protobuf starts correctly
    TEST_ASSERT_EQUAL(0, buf.obuf_ptr()[1]); // Verify data written (op_code)

    // Decode to verify contents
    org_ryderrobots_ros2_serial_Response decoded_response = org_ryderrobots_ros2_serial_Response_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buf.obuf_ptr(), BUFSIZ);
    bool decode_status = pb_decode(&istream, &org_ryderrobots_ros2_serial_Response_msg, &decoded_response);

    TEST_ASSERT_TRUE(decode_status);
    TEST_ASSERT_EQUAL(test_etype, decoded_response.data.bad_request.etype); // Verify etype
}

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Clean up code if needed
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_bad_request_buf);
    return UNITY_END();
}
