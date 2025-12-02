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
 * Base header file, that applies to all libraries
 */
#ifndef RR_BLE_MOUSEBOT_HPP
#define RR_BLE_MOUSEBOT_HPP

#include <Arduino.h>


#include "pb_encode.h"
#include "pb_decode.h"
#include "rr_serial.pb.h"

// #define TERM_CHAR 0x1E

namespace rr_ble {

enum {
    // monitoring sits in 1xx range
    MSP_RAW_IMU = 102,
    MSP_MOTOR = 104,
    MSP_RAW_SENSORS = 105,

    // Commands sit in the 2xx range.
    MSP_SET_RAW_RC = 200,

    // Errors included under here
    BAD_REQUEST = 400,
};

}

// // Operations codes are defined beneath
// #define  200
// #define  102
// #define  104
// #define  105

#endif // RR_BLE_MOUSEBOT_HPP