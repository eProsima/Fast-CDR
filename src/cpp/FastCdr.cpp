// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fastcdr/FastCdr.h>
#include <fastcdr/exceptions/BadParamException.h>
#include <string.h>

using namespace eprosima::fastcdr;
using namespace ::exception;

FastCdr::state::state(
        const FastCdr& fastcdr)
    : current_position_(fastcdr.current_position_)
{
}

FastCdr::state::state(
        const state& current_state)
    : current_position_(current_state.current_position_)
{
}

FastCdr::FastCdr(
        FastBuffer& cdrBuffer)
    : cdr_buffer_(cdrBuffer)
    , current_position_(cdrBuffer.begin())
    , last_position_(cdrBuffer.end())
{
}

bool FastCdr::jump(
        size_t numBytes)
{
    bool returnedValue = false;

    if (((last_position_ - current_position_) >= numBytes) || resize(numBytes))
    {
        current_position_ += numBytes;
        returnedValue = true;
    }

    return returnedValue;
}

char* FastCdr::get_current_position()
{
    return &current_position_;
}

FastCdr::state FastCdr::get_state()
{
    return FastCdr::state(*this);
}

void FastCdr::set_state(
        FastCdr::state& current_state)
{
    current_position_ >> current_state.current_position_;
}

void FastCdr::reset()
{
    current_position_ = cdr_buffer_.begin();
}

bool FastCdr::resize(
        size_t minSizeInc)
{
    if (cdr_buffer_.resize(minSizeInc))
    {
        current_position_ << cdr_buffer_.begin();
        last_position_ = cdr_buffer_.end();
        return true;
    }

    return false;
}

FastCdr& FastCdr::serialize(
        const bool bool_t)
{
    uint8_t value = 0;

    if (((last_position_ - current_position_) >= sizeof(uint8_t)) || resize(sizeof(uint8_t)))
    {
        if (bool_t)
        {
            value = 1;
        }
        current_position_++ << value;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serialize(
        const char* string_t)
{
    uint32_t length = 0;

    if (string_t != nullptr)
    {
        length = size_to_uint32(strlen(string_t)) + 1;
    }

    if (length > 0)
    {
        FastCdr::state state_before_error(*this);
        serialize(length);

        if (((last_position_ - current_position_) >= length) || resize(length))
        {
            current_position_.memcopy(string_t, length);
            current_position_ += length;
        }
        else
        {
            set_state(state_before_error);
            throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
        }
    }
    else
    {
        serialize(length);
    }

    return *this;
}

FastCdr& FastCdr::serialize(
        const wchar_t* string_t)
{
    uint32_t bytesLength = 0;
    size_t wstrlen = 0;

    if (string_t != nullptr)
    {
        wstrlen = wcslen(string_t);
        bytesLength = size_to_uint32(wstrlen * 4);
    }

    if (bytesLength > 0)
    {
        FastCdr::state state_(*this);
        serialize(size_to_uint32(wstrlen));

        if (((last_position_ - current_position_) >= bytesLength) || resize(bytesLength))
        {
#if defined(_WIN32)
            serialize_array(string_t, wstrlen);
#else
            current_position_.memcopy(string_t, bytesLength);
            current_position_ += bytesLength; // size on bytes
#endif // if defined(_WIN32)
        }
        else
        {
            set_state(state_);
            throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
        }
    }
    else
    {
        serialize(bytesLength);
    }

    return *this;
}

FastCdr& FastCdr::serialize_array(
        const bool* bool_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*bool_t) * num_elements;

    if (((last_position_ - current_position_) >= totalSize) || resize(totalSize))
    {
        for (size_t count = 0; count < num_elements; ++count)
        {
            uint8_t value = 0;

            if (bool_t[count])
            {
                value = 1;
            }
            current_position_++ << value;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serialize_array(
        const char* char_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*char_t) * num_elements;

    if (((last_position_ - current_position_) >= totalSize) || resize(totalSize))
    {
        current_position_.memcopy(char_t, totalSize);
        current_position_ += totalSize;
        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serialize_array(
        const int16_t* short_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*short_t) * num_elements;

    if (((last_position_ - current_position_) >= totalSize) || resize(totalSize))
    {
        current_position_.memcopy(short_t, totalSize);
        current_position_ += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serialize_array(
        const int32_t* long_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*long_t) * num_elements;

    if (((last_position_ - current_position_) >= totalSize) || resize(totalSize))
    {
        current_position_.memcopy(long_t, totalSize);
        current_position_ += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serialize_array(
        const wchar_t* wchar,
        size_t num_elements)
{
    for (size_t count = 0; count < num_elements; ++count)
    {
        serialize(wchar[count]);
    }
    return *this;
}

FastCdr& FastCdr::serialize_array(
        const int64_t* longlong_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*longlong_t) * num_elements;

    if (((last_position_ - current_position_) >= totalSize) || resize(totalSize))
    {
        current_position_.memcopy(longlong_t, totalSize);
        current_position_ += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serialize_array(
        const float* float_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*float_t) * num_elements;

    if (((last_position_ - current_position_) >= totalSize) || resize(totalSize))
    {
        current_position_.memcopy(float_t, totalSize);
        current_position_ += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serialize_array(
        const double* double_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*double_t) * num_elements;

    if (((last_position_ - current_position_) >= totalSize) || resize(totalSize))
    {
        current_position_.memcopy(double_t, totalSize);
        current_position_ += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serialize_array(
        const long double* ldouble_t,
        size_t num_elements)
{
    size_t totalSize = 16 * num_elements;

    if (((last_position_ - current_position_) >= totalSize) || resize(totalSize))
    {
#if FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
        for (size_t idx = 0; idx < num_elements; ++idx)
        {
            __float128 tmp = ldouble_t[idx];
            current_position_ << tmp;
            current_position_ += 16;
        }
#else
#if FASTCDR_SIZEOF_LONG_DOUBLE == 16
        current_position_.memcopy(ldouble_t, totalSize);
        current_position_ += totalSize;
#else
#if FASTCDR_SIZEOF_LONG_DOUBLE == 8
        for (size_t idx = 0; idx < num_elements; ++idx)
        {
            current_position_ << static_cast<long double>(0);
            current_position_ += 8;
            current_position_ << ldouble_t[idx];
            current_position_ += 8;
        }
#else
#error unsupported long double type and no __float128 available
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 8
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 16
#endif // FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize(
        bool& bool_t)
{
    uint8_t value = 0;

    if ((last_position_ - current_position_) >= sizeof(uint8_t))
    {
        current_position_++ >> value;

        if (value == 1)
        {
            bool_t = true;
            return *this;
        }
        else if (value == 0)
        {
            bool_t = false;
            return *this;
        }

        throw BadParamException("Got unexpected byte value in deserialize for bool (expected 0 or 1)");
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize(
        char*& string_t)
{
    uint32_t length = 0;
    FastCdr::state state_before_error(*this);

    deserialize(length);

    if (length == 0)
    {
        string_t = NULL;
        return *this;
    }
    else if ((last_position_ - current_position_) >= length)
    {
        // Allocate memory.
        string_t =
                reinterpret_cast<char*>(calloc(length + ((&current_position_)[length - 1] == '\0' ? 0 : 1),
                sizeof(char)));
        memcpy(string_t, &current_position_, length);
        current_position_ += length;
        return *this;
    }

    set_state(state_before_error);
    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize(
        wchar_t*& string_t)
{
    uint32_t length = 0;
    FastCdr::state state_before_error(*this);

    deserialize(length);

    if (length == 0)
    {
        string_t = NULL;
        return *this;
    }
    else if ((last_position_ - current_position_) >= length)
    {
        // Allocate memory.
        string_t = reinterpret_cast<wchar_t*>(calloc(length + 1, sizeof(wchar_t))); // WStrings never serialize terminating zero

#if defined(_WIN32)
        for (size_t idx = 0; idx < length; ++idx)
        {
            uint32_t temp;
            current_position_ >> temp;
            string_t[idx] = static_cast<wchar_t>(temp);
            current_position_ += 4;
        }
#else
        memcpy(string_t, &current_position_, length * sizeof(wchar_t));
        current_position_ += length * sizeof(wchar_t);
#endif // if defined(_WIN32)

        return *this;
    }

    set_state(state_before_error);
    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

const char* FastCdr::read_string(
        uint32_t& length)
{
    const char* returnedValue = "";
    state state_before_error(*this);

    *this >> length;

    if (length == 0)
    {
        return returnedValue;
    }
    else if ((last_position_ - current_position_) >= length)
    {
        returnedValue = &current_position_;
        current_position_ += length;
        if (returnedValue[length - 1] == '\0')
        {
            --length;
        }
        return returnedValue;
    }

    set_state(state_before_error);
    throw eprosima::fastcdr::exception::NotEnoughMemoryException(
              eprosima::fastcdr::exception::NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

std::wstring FastCdr::read_wstring(
        uint32_t& length)
{
    std::wstring returnedValue = L"";
    state state_(*this);

    *this >> length;
    uint32_t bytesLength = length * 4;

    if (bytesLength == 0)
    {
        return returnedValue;
    }
    else if ((last_position_ - current_position_) >= bytesLength)
    {

        returnedValue.resize(length);
        deserialize_array(const_cast<wchar_t*>(returnedValue.c_str()), length);
        if (returnedValue[length - 1] == L'\0')
        {
            --length;
            returnedValue.erase(length);
        }
        return returnedValue;
    }

    set_state(state_);
    throw eprosima::fastcdr::exception::NotEnoughMemoryException(
              eprosima::fastcdr::exception::NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize_array(
        bool* bool_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*bool_t) * num_elements;

    if ((last_position_ - current_position_) >= totalSize)
    {
        for (size_t count = 0; count < num_elements; ++count)
        {
            uint8_t value = 0;
            current_position_++ >> value;

            if (value == 1)
            {
                bool_t[count] = true;
            }
            else if (value == 0)
            {
                bool_t[count] = false;
            }
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize_array(
        char* char_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*char_t) * num_elements;

    if ((last_position_ - current_position_) >= totalSize)
    {
        current_position_.rmemcopy(char_t, totalSize);
        current_position_ += totalSize;
        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize_array(
        int16_t* short_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*short_t) * num_elements;

    if ((last_position_ - current_position_) >= totalSize)
    {
        current_position_.rmemcopy(short_t, totalSize);
        current_position_ += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize_array(
        int32_t* long_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*long_t) * num_elements;

    if ((last_position_ - current_position_) >= totalSize)
    {
        current_position_.rmemcopy(long_t, totalSize);
        current_position_ += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize_array(
        wchar_t* wchar,
        size_t num_elements)
{
    uint32_t value = 0;
    for (size_t count = 0; count < num_elements; ++count)
    {
        deserialize(value);
        wchar[count] = static_cast<wchar_t>(value);
    }
    return *this;
}

FastCdr& FastCdr::deserialize_array(
        int64_t* longlong_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*longlong_t) * num_elements;

    if ((last_position_ - current_position_) >= totalSize)
    {
        current_position_.rmemcopy(longlong_t, totalSize);
        current_position_ += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize_array(
        float* float_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*float_t) * num_elements;

    if ((last_position_ - current_position_) >= totalSize)
    {
        current_position_.rmemcopy(float_t, totalSize);
        current_position_ += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize_array(
        double* double_t,
        size_t num_elements)
{
    size_t totalSize = sizeof(*double_t) * num_elements;

    if ((last_position_ - current_position_) >= totalSize)
    {
        current_position_.rmemcopy(double_t, totalSize);
        current_position_ += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize_array(
        long double* ldouble_t,
        size_t num_elements)
{
    size_t totalSize = 16 * num_elements;

    if ((last_position_ - current_position_) >= totalSize)
    {
#if FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
        for (size_t idx = 0; idx < num_elements; ++idx)
        {
            __float128 tmp;
            current_position_ >> tmp;
            current_position_ += 16;
            ldouble_t[idx] = static_cast<long double>(tmp);
        }
#else
#if FASTCDR_SIZEOF_LONG_DOUBLE == 16
        current_position_.rmemcopy(ldouble_t, totalSize);
        current_position_ += totalSize;
#else
#if FASTCDR_SIZEOF_LONG_DOUBLE == 8
        for (size_t idx = 0; idx < num_elements; ++idx)
        {
            current_position_ += 8;
            current_position_ >> ldouble_t[idx];
            current_position_ += 8;
        }
#else
#error unsupported long double type and no __float128 available
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 8
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 16
#endif // FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serialize_bool_sequence(
        const std::vector<bool>& vector_t)
{
    state state_before_error(*this);

    *this << static_cast<int32_t>(vector_t.size());

    size_t totalSize = vector_t.size() * sizeof(bool);

    if (((last_position_ - current_position_) >= totalSize) || resize(totalSize))
    {
        for (size_t count = 0; count < vector_t.size(); ++count)
        {
            uint8_t value = 0;
            std::vector<bool>::const_reference ref = vector_t[count];

            if (ref)
            {
                value = 1;
            }
            current_position_++ << value;
        }
    }
    else
    {
        set_state(state_before_error);
        throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
    }

    return *this;
}

FastCdr& FastCdr::deserialize_bool_sequence(
        std::vector<bool>& vector_t)
{
    uint32_t sequence_length = 0;
    state state_before_error(*this);

    *this >> sequence_length;

    size_t totalSize = sequence_length * sizeof(bool);

    if ((last_position_ - current_position_) >= totalSize)
    {
        vector_t.resize(sequence_length);
        for (uint32_t count = 0; count < sequence_length; ++count)
        {
            uint8_t value = 0;
            current_position_++ >> value;

            if (value == 1)
            {
                vector_t[count] = true;
            }
            else if (value == 0)
            {
                vector_t[count] = false;
            }
        }
    }
    else
    {
        set_state(state_before_error);
        throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
    }

    return *this;
}

FastCdr& FastCdr::deserialize_string_sequence(
        std::string*& sequence_t,
        size_t& num_elements)
{
    uint32_t sequence_length = 0;
    state state_before_error(*this);

    deserialize(sequence_length);

    try
    {
        sequence_t = new std::string[sequence_length];
        deserialize_array(sequence_t, sequence_length);
    }
    catch (eprosima::fastcdr::exception::Exception& ex)
    {
        delete [] sequence_t;
        sequence_t = NULL;
        set_state(state_before_error);
        ex.raise();
    }

    num_elements = sequence_length;
    return *this;
}

FastCdr& FastCdr::deserialize_wstring_sequence(
        std::wstring*& sequence_t,
        size_t& num_elements)
{
    uint32_t sequence_length = 0;
    state state_before_error(*this);

    deserialize(sequence_length);

    try
    {
        sequence_t = new std::wstring[sequence_length];
        deserialize_array(sequence_t, sequence_length);
    }
    catch (eprosima::fastcdr::exception::Exception& ex)
    {
        delete [] sequence_t;
        sequence_t = NULL;
        set_state(state_before_error);
        ex.raise();
    }

    num_elements = sequence_length;
    return *this;
}
