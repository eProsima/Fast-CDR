/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This copy of FastCdr is licensed to you under the terms described in the
 * FASTCDR_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

#include "fastcdr/FastCdr.h"
#include "fastcdr/exceptions/BadParamException.h"
#include <string.h>

using namespace eprosima::fastcdr;
using namespace ::exception;

FastCdr::state::state(FastCdr &fastcdr) : m_currentPosition(fastcdr.m_currentPosition) {}

FastCdr::FastCdr(FastBuffer &cdrBuffer) : m_cdrBuffer(cdrBuffer), m_currentPosition(cdrBuffer.begin()), m_lastPosition(cdrBuffer.end())
{
}

bool FastCdr::jump(size_t numBytes)
{
    bool returnedValue = false;

    if(((m_lastPosition - m_currentPosition) >= numBytes) || resize(numBytes))
    {
        m_currentPosition += numBytes;
        returnedValue = true;
    }

    return returnedValue;
}

char* FastCdr::getCurrentPosition()
{
    return &m_currentPosition;
}

FastCdr::state FastCdr::getState()
{
    return FastCdr::state(*this);
}

void FastCdr::setState(FastCdr::state &state)
{
    m_currentPosition >> state.m_currentPosition;
}

void FastCdr::reset()
{
    m_currentPosition = m_cdrBuffer.begin();
}

bool FastCdr::resize(size_t minSizeInc)
{
    if(m_cdrBuffer.resize(minSizeInc))
    {
        m_currentPosition << m_cdrBuffer.begin();
        m_lastPosition = m_cdrBuffer.end();
        return true;
    }
    
    return false;
}

FastCdr& FastCdr::serialize(const bool bool_t)
{
    uint8_t value = 0;

    if(((m_lastPosition - m_currentPosition) >= sizeof(uint8_t)) || resize(sizeof(uint8_t)))
    {
        if(bool_t)
            value = 1;
        m_currentPosition++ << value;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serialize(const char *string_t)
{
    uint32_t length = (uint32_t)strlen(string_t);
    FastCdr::state state(*this);

    if(length > 0)
    {
		serialize(++length);

        if(((m_lastPosition - m_currentPosition) >= length) || resize(length))
        {
            m_currentPosition.memcopy(string_t, length);
            m_currentPosition += length;
        }
        else
        {
            setState(state);
            throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
        }
    }
	else
		serialize(length);

    return *this;
}

FastCdr& FastCdr::serializeArray(const char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        m_currentPosition.memcopy(char_t, totalSize);
        m_currentPosition += totalSize;
        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serializeArray(const int16_t *short_t, size_t numElements)
{
    size_t totalSize = sizeof(*short_t) * numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        m_currentPosition.memcopy(short_t, totalSize);
        m_currentPosition += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serializeArray(const int32_t *long_t, size_t numElements)
{
    size_t totalSize = sizeof(*long_t) * numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        m_currentPosition.memcopy(long_t, totalSize);
        m_currentPosition += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serializeArray(const wchar_t *wchar, size_t numElements)
{
    for(size_t count = 0; count < numElements; ++count)
        serialize(wchar[count]);
    return *this;
}

FastCdr& FastCdr::serializeArray(const int64_t *longlong_t, size_t numElements)
{
    size_t totalSize = sizeof(*longlong_t) * numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        m_currentPosition.memcopy(longlong_t, totalSize);
        m_currentPosition += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serializeArray(const float *float_t, size_t numElements)
{
    size_t totalSize = sizeof(*float_t) * numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        m_currentPosition.memcopy(float_t, totalSize);
        m_currentPosition += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serializeArray(const double *double_t, size_t numElements)
{
    size_t totalSize = sizeof(*double_t) * numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        m_currentPosition.memcopy(double_t, totalSize);
        m_currentPosition += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize(bool &bool_t)
{
    uint8_t value = 0;

    if((m_lastPosition - m_currentPosition) >= sizeof(uint8_t))
    {
        m_currentPosition++ >> value;

        if(value == 1)
        {
            bool_t = true;
            return *this;
        }
        else if(value == 0)
        {
            bool_t = false;
            return *this;
        }

        throw BadParamException(BadParamException::BAD_PARAM_MESSAGE_DEFAULT);
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize(char *&string_t)
{
    uint32_t length = 0;
    FastCdr::state state(*this);

    deserialize(length);

    if(length == 0)
    {
        string_t = NULL;
        return *this;
    }
    else if((m_lastPosition - m_currentPosition) >= length)
    {
        // Allocate memory.
        string_t = (char*)calloc(length + ((&m_currentPosition)[length-1] == '\0' ? 0 : 1), sizeof(char));
        memcpy(string_t, &m_currentPosition, length);
        m_currentPosition += length;
        return *this;
    }

    setState(state);
    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

const char* FastCdr::readString(uint32_t &length)
{
	const char* returnedValue = "";
	state state(*this);

	*this >> length;

	if(length == 0)
	{
		return returnedValue;
	}
	else if((m_lastPosition - m_currentPosition) >= length)
	{
		returnedValue = &m_currentPosition;
		m_currentPosition += length;
		if(returnedValue[length-1] == '\0') --length;
		return returnedValue;
	}

	setState(state);
	throw eprosima::fastcdr::exception::NotEnoughMemoryException(eprosima::fastcdr::exception::NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserializeArray(char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        m_currentPosition.rmemcopy(char_t, totalSize);
        m_currentPosition += totalSize;
        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserializeArray(int16_t *short_t, size_t numElements)
{
    size_t totalSize = sizeof(*short_t) * numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        m_currentPosition.rmemcopy(short_t, totalSize);
        m_currentPosition += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserializeArray(int32_t *long_t, size_t numElements)
{
    size_t totalSize = sizeof(*long_t) * numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        m_currentPosition.rmemcopy(long_t, totalSize);
        m_currentPosition += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserializeArray(wchar_t *wchar, size_t numElements)
{
    uint32_t value;
    for(size_t count = 0; count < numElements; ++count)
    {
        deserialize(value);
        wchar[count] = (wchar_t)value;
    }
    return *this;
}

FastCdr& FastCdr::deserializeArray(int64_t *longlong_t, size_t numElements)
{
    size_t totalSize = sizeof(*longlong_t) * numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        m_currentPosition.rmemcopy(longlong_t, totalSize);
        m_currentPosition += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserializeArray(float *float_t, size_t numElements)
{
    size_t totalSize = sizeof(*float_t) * numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        m_currentPosition.rmemcopy(float_t, totalSize);
        m_currentPosition += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserializeArray(double *double_t, size_t numElements)
{
    size_t totalSize = sizeof(*double_t) * numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        m_currentPosition.rmemcopy(double_t, totalSize);
        m_currentPosition += totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}