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

#ifndef MB_OP_FACTORY_HPP
#define MB_OP_FACTORY_HPP

#include <mb_operations.hpp>
#include <rr_imu.hpp>

namespace mb_operations
{

    /**
     * @class MBOperationsFactory
     * @brief
     */
    class MBOperationsFactory
    {
    public:
        /**
         * @fn init
         * @brief performs initlization, including op_handlers
         */
        void init();

        MBOperationsFactory() = default;

        /**
         * handle construction and deconstruction in code, to protect against memory errors.
         */
        ~MBOperationsFactory() = default;

        /**
         * @fn get_op_handler
         * @brief if supported, return MbOperationHandler.
         *
         * Attempts to return correct operation handler. and sets status.
         * Statuses can be one of the following:
         *
         * UNKNOWN - operation code is unknown to factory. (null pointer is returned)
         * READY -  operation handler is ready and returned
         * NOT_AVAILABLE - operation handler may be still initilizing, or currently processing another request.
         * FAILURE - operation handler exists, but has experienced some sort of failure.
         *
         * @return pointer to operation handler if available, otherwise null pointer.
         * @param req deserialized request
         * @param status one of the following statuses UNKNOWN, READY, NOT_AVAILABLE, or FAILURE
         */
        MbOperationHandler *get_op_handler(const org_ryderrobots_ros2_serial_Request &req, org_ryderrobots_ros2_serial_Status &status);

        private:
            RRImuOpHandler imu_op_hdl_; 
    };
}

#endif // MB_OP_FACTORY_HPP