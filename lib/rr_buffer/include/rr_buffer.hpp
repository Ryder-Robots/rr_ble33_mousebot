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

#ifndef RR_BUFFER_HPP
#define RR_BUFFER_HPP

// JSF Rule 40 [web:79], use cstring
#include <cstring>
#include <rr_ble.hpp>

namespace rr_buffer
{

    /**
     * @class RRBuffer
     * @brief memory allocator.
     *
     * Memory does not dynamically allocate memory. It is designed this way to protect agains inconsistent
     * memory allocation which can occur on a small processor.
     */
    class RRBuffer
    {
    private:
        std::uint8_t buf_[BUFSIZ];

        /**
         * Internal constructor to ensure that this remaines a singlton.
         */
        RRBuffer()
        {
            clear();
        }

    public:
        RRBuffer(const RRBuffer &) = delete;
        RRBuffer &operator=(const RRBuffer &) = delete;

        ~RRBuffer() = default;

        /**
         * @fn clear
         * @brief frees memory allocaiton, and adds '\n' to position 0
         */
        void clear();

        /**
         * @fn buffer
         * @brief returns buffer that can be used for Serial functions.
         */
        std::uint8_t *buffer();


        /**
         * @fn get_instance
         * @brief creates instance of buffer object.
         */
        static RRBuffer& get_instance();
    };
}

#endif