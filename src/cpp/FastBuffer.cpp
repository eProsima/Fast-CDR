#include "cpp/FastBuffer.h"

#include <malloc.h>
#include <cstddef>

#define BUFFER_START_LENGTH 200

namespace eProsima
{
    FastBuffer::FastBuffer() : m_buffer(NULL),
        m_bufferSize(0), m_internalBuffer(true)
    {
    }

    FastBuffer::FastBuffer(char* const buffer, const size_t bufferSize) : m_buffer(buffer),
        m_bufferSize(bufferSize), m_internalBuffer(false)
    {
    }

    // Cambiar iteradores.
    bool FastBuffer::resize(size_t minSizeInc)
    {
        size_t incBufferSize = BUFFER_START_LENGTH;

        if(m_internalBuffer)
        {
            if(minSizeInc > BUFFER_START_LENGTH)
            {
                incBufferSize = minSizeInc;
            }

            if(m_buffer == NULL)
            {
                m_bufferSize = incBufferSize;

                m_buffer = (char*)malloc(m_bufferSize);

                if(m_buffer != NULL)
                {
                    //m_bufferRemainLength = incBufferSize;
                    return true;
                }
            }
            else
            {
                //ptrdiff_t currentPositionDiff = m_currentPosition - m_buffer;
                //ptrdiff_t alignPositionDiff = m_alignPosition - m_buffer;
                m_bufferSize += incBufferSize;

                m_buffer = (char*)realloc(m_buffer, m_bufferSize);

                if(m_buffer != NULL)
                {
                    //m_currentPosition = m_buffer + currentPositionDiff;
                    //m_alignPosition = m_buffer + alignPositionDiff;
                    //m_bufferRemainLength += incBufferSize;
                    return true;
                }
            }
        }

        return false;
    }
};
