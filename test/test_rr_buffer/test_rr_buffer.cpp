#include <AUnit.h>
#include <rr_buffer.hpp>
#include <mberror.hpp>

using namespace rr_buffer;
using namespace mberror;

void setup()
{
    aunit::TestRunner::run();
}

/**
 * same unit test as mb_error, but uses the buffer.
 */
test(test_bad_request_buf)
{
    RRBuffer &buf = RRBuffer::get_instance();
    pb_ostream_t ostream = pb_ostream_from_buffer(buf.buffer(), BUFSIZ);
    RRBadRequest rr_bad_request(ostream);
    org_ryderrobots_ros2_serial_ErrorType test_etype = org_ryderrobots_ros2_serial_ErrorType_ET_MAX_LEN_EXCEED;

    int result = rr_bad_request.serialize(test_etype);

    assertEqual(0, result);
    assertEqual(0, buf.buffer()[0]); // Verify protobuf starts correctly
    assertEqual(0, buf.buffer()[1]); // Verify data written (op_code)

    // Decode to verify contents
    org_ryderrobots_ros2_serial_Response decoded_response = org_ryderrobots_ros2_serial_Response_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buf.buffer(), sizeof(buf));
    bool decode_status = pb_decode(&istream, &org_ryderrobots_ros2_serial_Response_msg, &decoded_response);

    assertTrue(decode_status);
    assertEqual(test_etype, decoded_response.data.bad_request.etype); // Verify etype
}

void loop()
{
    // Run tests once in loop
    delay(1000);
}