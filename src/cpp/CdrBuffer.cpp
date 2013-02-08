#include "CdrBuffer.h"

namespace eProsima
{
    CDRBuffer::CDRBuffer(char* const buffer, const size_t bufferLength, const Endianess endianess) : m_buffer(buffer),
        m_bufferLength(bufferLength), m_currentPosition(buffer), m_endianess(endianess),
        m_swapBytes(endianess == DEFAULT_ENDIAN ? false : true)
    {
    }

    bool CDRBuffer::operator==(CDRBuffer::Endianess &endianess)
    {
        return (endianess == CDRBuffer::NO_ENDIAN) || (m_endianess == endianess);
    }

    bool CDRBuffer::operator!=(CDRBuffer::Endianess &endianess)
    {
        return !(endianess == CDRBuffer::NO_ENDIAN) && !(m_endianess == endianess);
    }
};