/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This copy of FastCdr is licensed to you under the terms described in the
 * FASTCDR_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

#include "fastcdr/FastBuffer.h"

#include <malloc.h>

#define BUFFER_START_LENGTH 200

using namespace eprosima::fastcdr;

FastBuffer::FastBuffer() : m_buffer(NULL),
    m_bufferSize(0), m_internalBuffer(true)
{
}

FastBuffer::FastBuffer(char* const buffer, const size_t bufferSize) : m_buffer(buffer),
    m_bufferSize(bufferSize), m_internalBuffer(false)
{
}

FastBuffer::~FastBuffer()
{
    if(m_internalBuffer && m_buffer != NULL)
    {
        free(m_buffer);
    }
}

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
                return true;
            }
        }
        else
        {
            m_bufferSize += incBufferSize;

            m_buffer = (char*)realloc(m_buffer, m_bufferSize);

            if(m_buffer != NULL)
            {
                return true;
            }
        }
    }

    return false;
}
