
#include "cpp/cdr/FastCdr.h"
#include "cpp/exceptions/BadParamException.h"
#include <string.h>

using namespace eProsima::marshalling;
using namespace eProsima::storage;

const std::string FastCdr::BAD_PARAM_MESSAGE_DEFAULT("Bad parameter");
const std::string FastCdr::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT("Not enough memory in the buffer stream");

FastCdr::state::state(FastCdr &fastcdr) : m_currentPosition(fastcdr.m_currentPosition) {}

FastCdr::FastCdr(Storage &storage) : m_storage(storage), m_currentPosition(storage.begin()), m_lastPosition(storage.end())
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

/*char* FastCdr::getCurrentPosition()
{
    return &m_currentPosition;
}*/

size_t FastCdr::getSerializedDataLength() const
{
    Storage::iterator &tmp = m_storage.begin();
    size_t returnedSize =  m_currentPosition - tmp;
    delete &tmp;
    return returnedSize;
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
    delete &m_currentPosition;
    m_currentPosition = m_storage.begin();
}

bool FastCdr::resize(size_t minSizeInc)
{
    if(m_storage.resize(minSizeInc))
    {
        Storage::iterator &tmp = m_storage.begin();

        m_currentPosition << tmp;
        delete &m_lastPosition;
        m_lastPosition = m_storage.end();

        delete &tmp;
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
        m_storage.insert(m_currentPosition, value);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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
            m_storage.memcopy(m_currentPosition, string_t.c_str(), length);
        }
        else
        {
            setState(state);
            throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
        }
    }

    return *this;
}

FastCdr& FastCdr::serializeArray(const char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        m_storage.memcopy(m_currentPosition, char_t, totalSize);
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serializeArray(const int16_t *short_t, size_t numElements)
{
    size_t totalSize = sizeof(*short_t) * numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        m_storage.memcopy(m_currentPosition, short_t, totalSize);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serializeArray(const int32_t *long_t, size_t numElements)
{
    size_t totalSize = sizeof(*long_t) * numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        m_storage.memcopy(m_currentPosition, long_t, totalSize);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serializeArray(const int64_t *longlong_t, size_t numElements)
{
    size_t totalSize = sizeof(*longlong_t) * numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        m_storage.memcopy(m_currentPosition, longlong_t, totalSize);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serializeArray(const float *float_t, size_t numElements)
{
    size_t totalSize = sizeof(*float_t) * numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        m_storage.memcopy(m_currentPosition, float_t, totalSize);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::serializeArray(const double *double_t, size_t numElements)
{
    size_t totalSize = sizeof(*double_t) * numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        m_storage.memcopy(m_currentPosition, double_t, totalSize);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserialize(bool &bool_t)
{
    uint8_t value = 0;

    if((m_lastPosition - m_currentPosition) >= sizeof(uint8_t))
    {
        m_storage.get(m_currentPosition, value);

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

        throw BadParamException(BAD_PARAM_MESSAGE_DEFAULT);
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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
        //TODO
        //string_t = std::string(&m_currentPosition, length - ((&m_currentPosition)[length-1] == '\0' ? 1 : 0));
        m_currentPosition += length;
        return *this;
    }

    setState(state);
    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserializeArray(char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        m_storage.rmemcopy(m_currentPosition, char_t, totalSize);
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserializeArray(int16_t *short_t, size_t numElements)
{
    size_t totalSize = sizeof(*short_t) * numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        m_storage.rmemcopy(m_currentPosition, short_t, totalSize);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserializeArray(int32_t *long_t, size_t numElements)
{
    size_t totalSize = sizeof(*long_t) * numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        m_storage.rmemcopy(m_currentPosition, long_t, totalSize);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserializeArray(int64_t *longlong_t, size_t numElements)
{
    size_t totalSize = sizeof(*longlong_t) * numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        m_storage.rmemcopy(m_currentPosition, longlong_t, totalSize);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserializeArray(float *float_t, size_t numElements)
{
    size_t totalSize = sizeof(*float_t) * numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        m_storage.rmemcopy(m_currentPosition, float_t, totalSize);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

FastCdr& FastCdr::deserializeArray(double *double_t, size_t numElements)
{
    size_t totalSize = sizeof(*double_t) * numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        m_storage.rmemcopy(m_currentPosition, double_t, totalSize);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}
