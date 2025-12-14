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

#include <rr_imu.hpp>

namespace mb_operations
{
    void RRImuOpHandler::init()
    {
        filter_.begin(100.0f);
        IMU.begin();

        // allow a small delay for services to become active.
        delay(100);
        if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable())
        {
            status_ = org_ryderrobots_ros2_serial_Status::org_ryderrobots_ros2_serial_Status_READY;
        }
    }

    org_ryderrobots_ros2_serial_Status RRImuOpHandler::status()
    {
        return status_;
    }

    void RRImuOpHandler::euler_to_quaternion(float roll, float pitch, float yaw,
                                             float *q_w, float *q_x, float *q_y, float *q_z)
    {
        float cr = cosf(roll * 0.5f);
        float sr = sinf(roll * 0.5f);
        float cp = cosf(pitch * 0.5f);
        float sp = sinf(pitch * 0.5f);
        float cy = cosf(yaw * 0.5f);
        float sy = sinf(yaw * 0.5f);

        *q_w = cy * cp * cr + sy * sp * sr;
        *q_x = sy * sp * cr - cy * cp * sr;
        *q_y = cy * sp * cr + sy * cp * sr;
        *q_z = cy * cp * sr - sy * sp * cr;
    }

    /*
     * perform monitor request
     */
    void RRImuOpHandler::monitor(const org_ryderrobots_ros2_serial_Request &req, org_ryderrobots_ros2_serial_Response &response)
    {
        response = org_ryderrobots_ros2_serial_Response_init_zero;
        response.op = rr_ble::rr_op_code_t::MSP_RAW_IMU;

        float gx, gy, gz, ax, ay, az, qx, qy, qz, qw;
        IMU.readGyroscope(gx, gy, gz);
        IMU.readAcceleration(ax, ay, az);

        // Rate limit filter updates to match configured sample rate (100Hz)
        unsigned long current_ms = millis();
        if (current_ms - last_update_ms_ >= UPDATE_INTERVAL_MS)
        {
            filter_.updateIMU(gx, gy, gz, ax, ay, az);
            last_update_ms_ = current_ms;
        }

        org_ryderrobots_ros2_serial_MspRawImu payload = org_ryderrobots_ros2_serial_MspRawImu_init_zero;

        // set orientation
        euler_to_quaternion(filter_.getRoll(), filter_.getPitch(), filter_.getYaw(), &qw, &qx, &qy, &qz);
        payload.orientation.x = qx;
        payload.orientation.y = qy;
        payload.orientation.z = qz;
        payload.orientation.w = qw;
        payload.has_orientation = true;

        // angular velocity
        payload.angular_velocity.x = gx;
        payload.angular_velocity.y = gy;
        payload.angular_velocity.z = gz;
        payload.has_angular_velocity = true;

        // linera acceleration.
        payload.linear_acceleration.x = ax;
        payload.linear_acceleration.y = ay;
        payload.linear_acceleration.z = az;
        payload.has_linear_acceleration = true;
        response.data.msp_raw_imu = payload;
        response.which_data = org_ryderrobots_ros2_serial_Response_msp_raw_imu_tag;
    }

    void RRImuOpHandler::perform_op(const org_ryderrobots_ros2_serial_Request &req, org_ryderrobots_ros2_serial_Response &response)
    {
        response = org_ryderrobots_ros2_serial_Response_init_zero;
        response.op = rr_ble::rr_op_code_t::MSP_RAW_IMU;

        // failure condition. set error
        if (!(IMU.gyroscopeAvailable() && IMU.accelerationAvailable()))
        {
            status_ = org_ryderrobots_ros2_serial_Status::org_ryderrobots_ros2_serial_Status_NOT_AVAILABLE;
            org_ryderrobots_ros2_serial_BadRequest bad_request =
                org_ryderrobots_ros2_serial_BadRequest_init_zero;
            bad_request.etype = org_ryderrobots_ros2_serial_ErrorType::org_ryderrobots_ros2_serial_ErrorType_ET_SERVICE_UNAVAILABLE;
            response.data.bad_request = bad_request;
            response.which_data = org_ryderrobots_ros2_serial_Response_bad_request_tag;
            return;
        }

        // several actions may be supported by a specific sensor, so always
        // validate, and check which action is required.
        switch (req.which_data)
        {
        // send request back
        case org_ryderrobots_ros2_serial_Monitor_is_request_tag:
            return monitor(req, response);

        // error condition, unsupported request.
        default:
            org_ryderrobots_ros2_serial_BadRequest bad_request =
                org_ryderrobots_ros2_serial_BadRequest_init_zero;
            bad_request.etype = org_ryderrobots_ros2_serial_ErrorType::org_ryderrobots_ros2_serial_ErrorType_ET_UNKNOWN_OPERATION;
            response.data.bad_request = bad_request;
            response.which_data = org_ryderrobots_ros2_serial_Response_bad_request_tag;
            return;
        }
    }
}