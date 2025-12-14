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
 * CAVEAT: Covariance is not yet implemented.
 */

#ifndef RR_IMU_HPP
#define RR_IMU_HPP

#include "Arduino.h"
#include <Arduino_BMI270_BMM150.h>
#include "pb_encode.h"
#include "pb_decode.h"
#include "rr_serial.pb.h"
#include "math.h"
#include <mb_operations.hpp>
#include <MadgwickAHRS.h>
#include <rr_ble.hpp>

namespace mb_operations
{

    /**
     * @class RRImu
     * @brief responds to IMU op codes.
     */
    class RRImuOpHandler : public mb_operations::MbOperationHandler
    {

    private:
        Madgwick filter_;

        org_ryderrobots_ros2_serial_Status status_ = org_ryderrobots_ros2_serial_Status::org_ryderrobots_ros2_serial_Status_NOT_AVAILABLE;

        unsigned long last_update_ms_ = 0;
        static constexpr unsigned long UPDATE_INTERVAL_MS = 10; // 100Hz to match Madgwick filter

        // private methods
        void euler_to_quaternion(float roll, float pitch, float yaw,
                             float *q_w, float *q_x, float *q_y, float *q_z);

        void  monitor(const org_ryderrobots_ros2_serial_Request &req, org_ryderrobots_ros2_serial_Response & res);

    public:
        RRImuOpHandler() = default;
        ~RRImuOpHandler() = default;

        /**
         * @fn status
         * @brief reported status to be used for feature list.
         */
        org_ryderrobots_ros2_serial_Status status() override;

        /**
         * @fn init
         * @brief performs inilization of IMU, and Accelometer.
         */
        void init() override;

        /**
         * @fn perform_op
         * @brief set IMU data in accordance to org_ryderrobots_ros2_serial_MspRawImu
         *
         * org_ryderrobots_ros2_serial_Quaternion.x, y, and z correspond to IMU.readGyroscope(x, y, z)
         *
         *
         * Note this assumes that the mousebot is orientated facing 1,0,0,0 of the maze or left most corner, where
         * IMU is oritenated with 'USB plug' facing at bottom of robot, and Y is travesing is length of mousebot travesing
         * from top if of chip in a forward direction.
         *
         * Accelometer, and gyroscope data is used to compute x,y,z, and w with Arduinos MadgwickAHRS.h filter objects.
         *
         *      y
         *   +-----+
         * x |     | x
         *   | USB |
         *   +-----+
         *      y
         * For z value this should always be assumed to be '0' as mousebot will not perform any incline raises while
         * solving a standard maze.
         */
        void perform_op(const org_ryderrobots_ros2_serial_Request &req, org_ryderrobots_ros2_serial_Response &response) override;
    };
}

#endif