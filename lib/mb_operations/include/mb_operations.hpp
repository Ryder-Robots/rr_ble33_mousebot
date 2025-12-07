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

#ifndef MB_OPERATIONS_HPP
#define MB_OPERATIONS_HPP

#include "pb_encode.h"
#include "pb_decode.h"
#include "rr_serial.pb.h"

namespace mb_operations {
    /**
     * @class MbOperationHandler
     * @brief interface for operation handlers
     * 
     * Provides interface for operation handlers. An operation is a specific operation that
     * protobuf provies. Operation handlers are expected to interpret a specific operation,
     * perform some sort of action, and return the result.
     * 
     * All operations shall be objects managed within a factory object, which will instantiate the operation handlers during
     * its init() method.  This MUST be done during the setup() phase of the main loop, in order to ensure memory
     * for every operation handler has been allocated. 
     * 
     * Operation handlers MAY at most be in charge of one sensor, or a range of a sensors of the same LINK_ID, for 
     * instance a range_sensor operation handler MAY handle several range ultrasonic range sensors,  but CANNOT 
     * handle ultrasonic and LIDAR sensors.
     * 
     * During main loop execution, the factory object will return an appropriate operation handler pointer instance,
     * based upon the req.op value.
     * 
     * The main loop when then call perform_op(req), and receive the a response, which it will return back
     * to Serial device.  Objects SHALL be owned by factory object.
     * 
     * CAVEAT: Note that because Arduino's main loop operates in one thread, and performs one action at a time. 
     * This method does not pay attention to thread safety.
     */
    class MbOperationHandler {
        public:

        /**
         * @fn init
         * @brief performs inilization of operation handler.
         * 
         * As part of factory objects initilization procedure, init() for each component will be called, this
         * will perform any initalization operations.
         */
        virtual void init() = 0;

        /**
         * @fn perform_op
         * @brief handles specific operation action. Response is the result of the action including any error 
         * state.
         */
        virtual const org_ryderrobots_ros2_serial_Response & perform_op(const org_ryderrobots_ros2_serial_Request & req) = 0;

        /**
         * @fn status
         * @brief reports back sensor status.
         * 
         * For sensors that are supported by the micro-controller, statuses will either be ACTIVE or FAILURE.
         * For FAILUREs it is up to the calling system to intrepret how to handle this.
         */
        virtual const org_ryderrobots_ros2_serial_Status status() = 0;
    };
}

#endif // MB_OPERATIONS_HPP