#include <AUnit.h>
#include <mberror.hpp>

using namespace mberror;

void setup()
{
    aunit::TestRunner::run();
}

test(test_bad_request)
{

    std::uint8_t buf[BUFSIZ];
    auto ostream = pb_ostream_from_buffer(buf, BUFSIZ);
    RRBadRequest rr_bad_request(ostream);
    org_ryderrobots_ros2_serial_ErrorType test_etype = org_ryderrobots_ros2_serial_ErrorType_ET_MAX_LEN_EXCEED;

    int result = rr_bad_request.serialize(test_etype);

    assertEqual(0, result);
    assertEqual(0, buf[0]); // Verify protobuf starts correctly
    assertEqual(0, buf[1]); // Verify data written (op_code)

    // Decode to verify contents
    org_ryderrobots_ros2_serial_Response decoded_response = org_ryderrobots_ros2_serial_Response_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buf, sizeof(buf));
    bool decode_status = pb_decode(&istream, &org_ryderrobots_ros2_serial_Response_msg, &decoded_response);

    assertTrue(decode_status);
    // assertEqual(rr_ble::BAD_REQUEST, decoded_response.op);  // Verify op_code
    assertEqual(test_etype, decoded_response.data.bad_request.etype); // Verify etype
}

void loop()
{
    // Run tests once in loop
    // run_unity_tests();
    delay(1000);
}