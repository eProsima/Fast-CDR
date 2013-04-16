#include "cpp/CdrBuffer.h"

namespace eProsima
{
#if defined(__LITTLE_ENDIAN__)
    const CDRBuffer::Endianness CDRBuffer::DEFAULT_ENDIAN = LITTLE_ENDIANNESS;
#elif defined (__BIG_ENDIAN__)
    const CDRBuffer::Endianness CDRBuffer::DEFAULT_ENDIAN = BIG_ENDIANNESS;
#endif

    CDRBuffer::CDRBuffer(char* const buffer, const size_t bufferSize, const Endianness endianness) : m_buffer(buffer),
        m_bufferSize(bufferSize), m_bufferLength(bufferSize), m_currentPosition(buffer), m_alignPosition(buffer), m_endianness(endianness),
        m_swapBytes(endianness == DEFAULT_ENDIAN ? false : true), m_lastDataSize(0), m_funcAllocator(NULL)
    {
    }

    CDRBuffer::CDRBuffer(char* const buffer, const size_t bufferSize, CDRBufferFuncAllocator funcAllocator, const Endianness endianness): m_buffer(buffer),
        m_bufferSize(bufferSize), m_bufferLength(bufferSize), m_currentPosition(buffer), m_alignPosition(buffer), m_endianness(endianness),
        m_swapBytes(endianness == DEFAULT_ENDIAN ? false : true), m_lastDataSize(0), m_funcAllocator(funcAllocator)
    {
    }

    void CDRBuffer::reset()
    {
        m_bufferLength = m_bufferSize;
        m_currentPosition = m_buffer;
        m_alignPosition = m_buffer;
        m_swapBytes = m_endianness == DEFAULT_ENDIAN ? false : true;
        m_lastDataSize = 0;
    }

    bool CDRBuffer::resize(size_t minSizeInc)
    {
        if(m_funcAllocator != NULL)
        {
            ptrdiff_t currentPositionDiff = m_currentPosition - m_buffer;
            ptrdiff_t alignPositionDiff = m_alignPosition - m_buffer;
            size_t oldBufferSize = m_bufferSize;

            if(m_funcAllocator(&m_buffer, &m_bufferSize, minSizeInc))
            {
                m_currentPosition = m_buffer + currentPositionDiff;
                m_alignPosition = m_buffer + alignPositionDiff;
                m_bufferLength += m_bufferSize - oldBufferSize;

                return true;
            }
        }

        return false;
    }
};