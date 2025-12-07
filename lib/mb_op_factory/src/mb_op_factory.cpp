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

#include <mb_op_factory.hpp>

namespace mb_operations
{
    void MBOperationsFactory::init()
    {
        imu_op_hdl_.init();
    }

    MbOperationHandler *MBOperationsFactory::get_op_handler(const org_ryderrobots_ros2_serial_Request &req, org_ryderrobots_ros2_serial_Status &status)
    {
        RRImuOpHandler *hdl = nullptr;
        status = org_ryderrobots_ros2_serial_Status::org_ryderrobots_ros2_serial_Status_UNKNOWN;
        switch (req.op)
        {
        case rr_ble::MSP_RAW_IMU:
            hdl = &imu_op_hdl_;
            break;

        default:
            status = org_ryderrobots_ros2_serial_Status::org_ryderrobots_ros2_serial_Status_UNKNOWN;
            return nullptr;
        }

        status = hdl->status();
        if (status != org_ryderrobots_ros2_serial_Status_READY)
        {
            return nullptr;
        }
        status = org_ryderrobots_ros2_serial_Status_READY;
        return hdl;
    }

}