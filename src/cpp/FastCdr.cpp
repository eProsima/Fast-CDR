
#include "cpp/FastCdr.h"
#include "cpp/exceptions/NotEnoughMemoryException.h"
#include "cpp/exceptions/BadParamException.h"
#include <string.h>

using namespace eProsima;

const std::string NOT_ENOUGH_MEMORY_MESSAGE("Not enough memory in the buffer stream");
const std::string BAD_PARAM_MESSAGE("Bad parameter");

FastCdr::state::state(FastCdr &fastcdr) : m_currentPosition(fastcdr.m_currentPosition) {}

FastCdr::FastCdr(FastBuffer &cdrBuffer) : m_cdrBuffer(cdrBuffer), m_currentPosition(cdrBuffer.begin()), m_lastPosition(cdrBuffer.end())
{
}

bool FastCdr::jump(uint32_t numBytes)
{
    bool returnedValue = false;

    if(((m_lastPosition - m_currentPosition) >= sizeof(numBytes)))
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

FastCdr& FastCdr::serialize(const char char_t)
{
    if(((m_lastPosition - m_currentPosition) >= sizeof(char_t)) || resize(sizeof(char_t)))
    {
        m_currentPosition++ << char_t;
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const int16_t short_t)
{
    if(((m_lastPosition - m_currentPosition) >= sizeof(short_t)) || resize(sizeof(short_t)))
    {
        m_currentPosition << short_t;
        m_currentPosition += sizeof(short_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const int32_t long_t)
{
    if(((m_lastPosition - m_currentPosition) >= sizeof(long_t)) || resize(sizeof(long_t)))
    {
        m_currentPosition << long_t;
        m_currentPosition += sizeof(long_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const int64_t longlong_t)
{
    if(((m_lastPosition - m_currentPosition) >= sizeof(longlong_t)) || resize(sizeof(longlong_t)))
    {
        m_currentPosition << longlong_t;
        m_currentPosition += sizeof(longlong_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const float float_t)
{
    if(((m_lastPosition - m_currentPosition) >= sizeof(float_t)) || resize(sizeof(float_t)))
    {
        m_currentPosition << float_t;
        m_currentPosition += sizeof(float_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const double double_t)
{
    if(((m_lastPosition - m_currentPosition) >= sizeof(double_t)) || resize(sizeof(double_t)))
    {
        m_currentPosition << double_t;
        m_currentPosition += sizeof(double_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const std::string &string_t)
{
    uint32_t length = (uint32_t)string_t.length();
    FastCdr::state state(*this);

    serialize(length);

    if(length > 0)
    {
        if(((m_lastPosition - m_currentPosition) >= length) || resize(length))
        {
            m_currentPosition.memcopy(string_t.c_str(), length);
            m_currentPosition += length;
        }
        else
        {
            setState(state);
            throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
        }
    }

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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(char &char_t)
{
    if((m_lastPosition - m_currentPosition) >= sizeof(char_t))
    {
        m_currentPosition++ >> char_t;
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(int16_t &short_t)
{
    if((m_lastPosition - m_currentPosition) >= sizeof(short_t))
    {
        m_currentPosition >> short_t;
        m_currentPosition += sizeof(short_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(int32_t &long_t)
{
    if((m_lastPosition - m_currentPosition) >= sizeof(long_t))
    {
        m_currentPosition >> long_t;
        m_currentPosition += sizeof(long_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(int64_t &longlong_t)
{
    if((m_lastPosition - m_currentPosition) >= sizeof(longlong_t))
    {
        m_currentPosition >> longlong_t;
        m_currentPosition += sizeof(longlong_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(float &float_t)
{
    if((m_lastPosition - m_currentPosition) >= sizeof(float_t))
    {
        m_currentPosition >> float_t;
        m_currentPosition += sizeof(float_t);

        return *this;
    }

   throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(double &double_t)
{
    if((m_lastPosition - m_currentPosition) >= sizeof(double_t))
    {
        m_currentPosition >> double_t;
        m_currentPosition += sizeof(double_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

        throw BadParamException(BAD_PARAM_MESSAGE);
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(std::string &string_t)
{
    uint32_t length = 0;
    FastCdr::state state(*this);

    deserialize(length);

    if(length == 0)
    {
        string_t = "";
        return *this;
    }
    else if((m_lastPosition - m_currentPosition) >= length)
    {
        string_t = std::string(&m_currentPosition, length - ((&m_currentPosition)[length-1] == '\0' ? 1 : 0));
        m_currentPosition += length;
        return *this;
    }

    setState(state);
    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
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

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}
