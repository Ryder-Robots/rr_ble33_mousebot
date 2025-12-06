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

#ifndef WDT_HPP
#define WDT_HPP

#include <nrf.h>
#include <Arduino.h>

namespace wdt
{

    /**
     * @class wdt
     * @brief watchdog functionality
     *
     * Connects to BLE internal watchdog, which is nRF52840.
     */
    class Wdt
    {

    private:
        size_t timeout_secs_ = 5;

        Wdt()
        {
            init();
        }

    public:
        Wdt(const Wdt &) = delete;

        Wdt &operator=(const Wdt &) = delete;

        static Wdt & get_instance();

        /**
         * @fn init
         * @brief initlize RR
         */
        void init();

        /**
         * @fn reset
         * @brief Feed RR[0]
         */
        void reset();
    };
}

#endif // WDT_HPP