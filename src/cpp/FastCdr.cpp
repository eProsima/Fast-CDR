
#include "cpp/FastCdr.h"
#include "cpp/exceptions/NotEnoughMemoryException.h"
#include "cpp/exceptions/BadParamException.h"
#include <string.h>

using namespace eProsima;

const std::string NOT_ENOUGH_MEMORY_MESSAGE("Not enough memory in the buffer stream");
const std::string BAD_PARAM_MESSAGE("Bad parameter");

FastCdr::FastCdr(FastBuffer &cdrBuffer) : m_cdrBuffer(cdrBuffer), m_currentPosition(cdrBuffer.begin())
{
}

bool FastCdr::jump(uint32_t numBytes)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(numBytes)))
    {
        m_cdrBuffer.m_currentPosition += numBytes;
        returnedValue = true;
    }

    return returnedValue;
}

char* FastCdr::getCurrentPosition()
{
    return m_cdrBuffer.m_currentPosition;
}

FastBuffer::State FastCdr::getState() const
{
    return FastBuffer::State(m_cdrBuffer);
}

void FastCdr::setState(FastBuffer::State &state)
{
    m_cdrBuffer.m_currentPosition = state.m_currentPosition;
    m_cdrBuffer.m_bufferRemainLength = state.m_bufferRemainLength;
    m_cdrBuffer.m_alignPosition = state.m_alignPosition;
}

FastCdr& FastCdr::serialize(const char char_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(char_t)) || m_cdrBuffer.resize(sizeof(char_t)))
    {
        *m_cdrBuffer.m_currentPosition++ = char_t;
        m_cdrBuffer.m_bufferRemainLength -= sizeof(char_t);
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const int16_t short_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(short_t)) || m_cdrBuffer.resize(sizeof(short_t)))
    {
        const char *dst = reinterpret_cast<const char*>(&short_t);
        memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(short_t));
        m_cdrBuffer.m_currentPosition += sizeof(short_t);
        m_cdrBuffer.m_bufferRemainLength -= sizeof(short_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const int32_t long_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(long_t)) || m_cdrBuffer.resize(sizeof(long_t)))
    {
        const char *dst = reinterpret_cast<const char*>(&long_t);
        memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(long_t));
        m_cdrBuffer.m_currentPosition += sizeof(long_t);
        m_cdrBuffer.m_bufferRemainLength -= sizeof(long_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const int64_t longlong_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(longlong_t)) || m_cdrBuffer.resize(sizeof(longlong_t)))
    {
        const char *dst = reinterpret_cast<const char*>(&longlong_t);
        memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(longlong_t));
        m_cdrBuffer.m_currentPosition += sizeof(longlong_t);
        m_cdrBuffer.m_bufferRemainLength -= sizeof(longlong_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const float float_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(float_t)) || m_cdrBuffer.resize(sizeof(float_t)))
    {
        const char *dst = reinterpret_cast<const char*>(&float_t);
        memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(float_t));
        m_cdrBuffer.m_currentPosition += sizeof(float_t);
        m_cdrBuffer.m_bufferRemainLength -= sizeof(float_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const double double_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(double_t)) || m_cdrBuffer.resize(sizeof(double_t)))
    {
        const char *dst = reinterpret_cast<const char*>(&double_t);
        memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(double_t));
        m_cdrBuffer.m_currentPosition += sizeof(double_t);
        m_cdrBuffer.m_bufferRemainLength -= sizeof(double_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const bool bool_t)
{
    uint8_t value = 0;

    if(m_cdrBuffer.checkSpace(sizeof(uint8_t)) || m_cdrBuffer.resize(sizeof(uint8_t)))
    {
        if(bool_t)
            value = 1;
        *m_cdrBuffer.m_currentPosition++ = value;
        m_cdrBuffer.m_bufferRemainLength -= sizeof(uint8_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serialize(const std::string &string_t)
{
    uint32_t length = (uint32_t)string_t.length();
    FastBuffer::State state(m_cdrBuffer);

    serialize(length);

    if(length > 0)
    {
        if(m_cdrBuffer.checkSpace(length) || m_cdrBuffer.resize(length))
        {
            memcpy(m_cdrBuffer.m_currentPosition, string_t.c_str(), length);
            m_cdrBuffer.m_currentPosition += length;
            m_cdrBuffer.m_bufferRemainLength -= length;
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

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        memcpy(m_cdrBuffer.m_currentPosition, char_t, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serializeArray(const int16_t *short_t, size_t numElements)
{
    size_t totalSize = sizeof(*short_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        memcpy(m_cdrBuffer.m_currentPosition, short_t, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serializeArray(const int32_t *long_t, size_t numElements)
{
    size_t totalSize = sizeof(*long_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        memcpy(m_cdrBuffer.m_currentPosition, long_t, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serializeArray(const int64_t *longlong_t, size_t numElements)
{
    size_t totalSize = sizeof(*longlong_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        memcpy(m_cdrBuffer.m_currentPosition, longlong_t, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serializeArray(const float *float_t, size_t numElements)
{
    size_t totalSize = sizeof(*float_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        memcpy(m_cdrBuffer.m_currentPosition, float_t, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::serializeArray(const double *double_t, size_t numElements)
{
    size_t totalSize = sizeof(*double_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        memcpy(m_cdrBuffer.m_currentPosition, double_t, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(char &char_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(char_t)))
    {
        char_t = *m_cdrBuffer.m_currentPosition++;
        m_cdrBuffer.m_bufferRemainLength -= sizeof(char_t);
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(int16_t &short_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(short_t)))
    {
        char *dst = reinterpret_cast<char*>(&short_t);
        memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(short_t));
        m_cdrBuffer.m_currentPosition += sizeof(short_t);
        m_cdrBuffer.m_bufferRemainLength -= sizeof(short_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(int32_t &long_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(long_t)))
    {
        char *dst = reinterpret_cast<char*>(&long_t);
        memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(long_t));
        m_cdrBuffer.m_currentPosition += sizeof(long_t);
        m_cdrBuffer.m_bufferRemainLength -= sizeof(long_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(int64_t &longlong_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(longlong_t)))
    {
        char *dst = reinterpret_cast<char*>(&longlong_t);
        memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(longlong_t));
        m_cdrBuffer.m_currentPosition += sizeof(longlong_t);
        m_cdrBuffer.m_bufferRemainLength -= sizeof(longlong_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(float &float_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(float_t)))
    {
        char *dst = reinterpret_cast<char*>(&float_t);
        memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(float_t));
        m_cdrBuffer.m_currentPosition += sizeof(float_t);
        m_cdrBuffer.m_bufferRemainLength -= sizeof(float_t);

        return *this;
    }

   throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(double &double_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(double_t)))
    {
        char *dst = reinterpret_cast<char*>(&double_t);
        memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(double_t));
        m_cdrBuffer.m_currentPosition += sizeof(double_t);
        m_cdrBuffer.m_bufferRemainLength -= sizeof(double_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserialize(bool &bool_t)
{
    uint8_t value = 0;

    if(m_cdrBuffer.checkSpace(sizeof(uint8_t)))
    {
        value = *m_cdrBuffer.m_currentPosition++;
        m_cdrBuffer.m_bufferRemainLength -= sizeof(uint8_t);

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
    FastBuffer::State state(m_cdrBuffer);

    deserialize(length);

    if(length == 0)
    {
        string_t = "";
        return *this;
    }
    else if(m_cdrBuffer.checkSpace(length))
    {
        string_t = std::string(m_cdrBuffer.m_currentPosition, length - (m_cdrBuffer.m_currentPosition[length-1] == '\0' ? 1 : 0));
        m_cdrBuffer.m_currentPosition += length;
        m_cdrBuffer.m_bufferRemainLength -= length;
        return *this;
    }

    setState(state);
    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserializeArray(char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        memcpy(char_t, m_cdrBuffer.m_currentPosition, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserializeArray(int16_t *short_t, size_t numElements)
{
    size_t totalSize = sizeof(*short_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        memcpy(short_t, m_cdrBuffer.m_currentPosition, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserializeArray(int32_t *long_t, size_t numElements)
{
    size_t totalSize = sizeof(*long_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        memcpy(long_t, m_cdrBuffer.m_currentPosition, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserializeArray(int64_t *longlong_t, size_t numElements)
{
    size_t totalSize = sizeof(*longlong_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        memcpy(longlong_t, m_cdrBuffer.m_currentPosition, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserializeArray(float *float_t, size_t numElements)
{
    size_t totalSize = sizeof(*float_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        memcpy(float_t, m_cdrBuffer.m_currentPosition, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

FastCdr& FastCdr::deserializeArray(double *double_t, size_t numElements)
{
    size_t totalSize = sizeof(*double_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        memcpy(double_t, m_cdrBuffer.m_currentPosition, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}