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

#ifndef MBERROR_HPP
#define MBERROR_HPP

#include <rr_ble.hpp>

/**
 * MBERRORs are defined beneath, update this header file to add more errors.
 */
namespace mberror {
    /**
     * @class RRBlError
     * Base class for RR BLE errors.
     */
    class RRBlError {
        public:

        explicit RRBlError(rr_ble::rr_op_code_t op_code, pb_ostream_t ostream):
        op_code_(op_code),
        ostream_(ostream)
        {}

        ~RRBlError() = default;

        /**
         * @fn serialize
         * @brief populates buffer *buf with bad request data. Status will be the status
         * code set by calling routine.
         * 
         * @param buf buffer to populate
         * @param status status to describe error.
         * @return on success return zero, otherwise return non-zero value.
         */
        virtual int serialize(org_ryderrobots_ros2_serial_ErrorType status) = 0;
        
        
        protected:
        rr_ble::rr_op_code_t op_code_;
        pb_ostream_t ostream_;
    };

    class RRBadRequest : public RRBlError {
        public:
        explicit RRBadRequest(pb_ostream_t ostream): RRBlError(rr_ble::rr_op_code_t::BAD_REQUEST, ostream) {}

        int serialize(org_ryderrobots_ros2_serial_ErrorType status) override;
    };
}

#endif // MBERROR_HPP