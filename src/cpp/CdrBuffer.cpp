#include "CdrBuffer.h"

namespace eProsima
{
#if defined(__LITTLE_ENDIAN__)
    const CDRBuffer::Endianess CDRBuffer::DEFAULT_ENDIAN = LITTLE_ENDIANESS;
#elif defined (__BIG_ENDIAN__)
    const CDRBuffer::Endianess CDRBuffer::DEFAULT_ENDIAN = BIG_ENDIANESS;
#endif

    CDRBuffer::CDRBuffer(char* const buffer, const size_t bufferSize, const Endianess endianess) : m_buffer(buffer),
        m_bufferSize(bufferSize), m_bufferLength(bufferSize), m_currentPosition(buffer), m_alignPosition(buffer), m_endianess(endianess),
        m_swapBytes(endianess == DEFAULT_ENDIAN ? false : true), m_lastDataSize(0)
    {
    }

    void CDRBuffer::reset()
    {
        m_bufferLength = m_bufferSize;
        m_currentPosition = m_buffer;
        m_alignPosition = m_buffer;
        m_swapBytes = m_endianess == DEFAULT_ENDIAN ? false : true;
        m_lastDataSize = 0;
    }
};