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

#include <wdt.hpp>

namespace wdt
{
    /**
     * These functions are pretty spectic chipsets that support them, such as Arduino Nano BLE 33.
     */
    void Wdt::init()
    {
        // 5 second timeout
        NRF_WDT->CRV = 32768 * timeout_secs_; // 32768 * 5
        NRF_WDT->RREN = WDT_RREN_RR0_Msk;     // Enable RR[0]
        NRF_WDT->CONFIG = ((WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos) | (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos));
        NRF_WDT->TASKS_START = 1;
    }

    void Wdt::reset()
    {
        NRF_WDT->RR[0] = WDT_RR_RR_Reload;
    }

    Wdt &Wdt::get_instance()
    {
        static Wdt instance;
        return instance;
    }
}