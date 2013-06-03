#ifndef _CPP_CDRBUFFER_H_
#define _CPP_CDRBUFFER_H_

#include "eProsima_cpp/eProsima_cpp_dll.h"
#include <stdint.h>
#include <cstdio>
#include <string.h>

namespace eProsima
{
    /*!
     * @brief This class implements the iterator used to go through a FastBuffer.
     * @ingroup CDRAPIREFERENCE
     */
    class _FastBuffer_iterator
    {
    public:

        _FastBuffer_iterator() : m_buffer(NULL), m_currentPosition(NULL) {}

        explicit _FastBuffer_iterator(char *buffer, size_t index) : m_buffer(buffer), m_currentPosition(&m_buffer[index]){}

        inline
        void operator<<(const _FastBuffer_iterator &iterator)
        {
            ptrdiff_t diff = m_currentPosition - m_buffer;
            m_buffer = iterator.m_buffer;
            m_currentPosition = m_buffer + diff;
        }

        inline
        void operator>>(const _FastBuffer_iterator &iterator)
        {
            ptrdiff_t diff = iterator.m_currentPosition - iterator.m_buffer;
            m_currentPosition = m_buffer + diff;
        }

        template<typename _T>
        inline
        void operator<<(const _T &data)
        {
            *((_T*)m_currentPosition) = data;
        }

        template<typename _T>
        inline
        void operator>>(_T &data)
        {
            data = *((_T*)m_currentPosition);
        }

        inline
        void memcopy(const void* src, const size_t size)
        {
            memcpy(m_currentPosition, src, size);
        }

        inline
        void rmemcopy(void* dst, const size_t size)
        {
            memcpy(dst, m_currentPosition, size);
        }

        inline
        void operator+=(size_t numBytes)
        {
            m_currentPosition += numBytes;
        }

        inline
        size_t operator-(const _FastBuffer_iterator &it) const
        {
            return m_currentPosition - it.m_currentPosition;
        }

        inline
        _FastBuffer_iterator operator++()
        {
            ++m_currentPosition;
            return *this;
        }

        inline
        _FastBuffer_iterator operator++(int)
        {
            _FastBuffer_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        inline 
        char* operator&()
        {
            return m_currentPosition;
        }

    private:

        char *m_buffer;

        char *m_currentPosition;
    };

    /*!
    * @brief This class represents a stream of bytes that contains or will contain
    * a CDR representation. This class is used by an object eProsima::CDR to serialize
    * or deserialize a CDR representation.
    * @ingroup CDRAPIREFERENCE
    */
    class eProsima_cpp_DllVariable FastBuffer
    {
    public:
        typedef _FastBuffer_iterator iterator;

        /*!
         * @brief This constructor creates an internal stream and assigns it to the eProsima::FastBuffer object.
         * The user can obtain this internal stream using the function eProsima::FastBuffer::getBuffer(). Be careful because this internal stream
         * is deleted in the destruction of the eProsima::FastBuffer object.
         */
        FastBuffer();

        /*!
         * @brief This constructor assigns the user's stream of bytes to the eProsima::FastBuffer object.
         * The user's stream will be used to serialize.
         *
         * @param buffer The user's buffer that will be used. This buffer is not deallocated in the object's destruction. Cannot be NULL.
         * @param bufferSize The length of user's buffer.
         */
        FastBuffer(char* const buffer, const size_t bufferSize);

        /*!
         * @brief This function returns the stream that the eProsima::FastBuffer uses to serialize data.
         * @return The stream used by eProsima::FastBuffer to serialize data.
         */
        inline char* getBuffer() const { return m_buffer;}

        /*!
         * @brief This function returns the size of the allocated memory of the stream that the eProsima::FastBuffer uses to serialize data.
         * @return The size of the allocated memory of the stream used by the eProsima::FastBuffer to serialize data.
         */
        inline size_t getBufferSize() const { return m_bufferSize;}

        inline
        iterator begin()
        {
            return (iterator(m_buffer, 0));
        }

        inline
        iterator end()
        {
            return (iterator(m_buffer, m_bufferSize));
        }

        /*!
         * @brief This function resizes the raw buffer. It will call the user's defined function to make this job.
         * @param minSizeInc The minimun growth expected of the current raw buffer.
         * @return True value has to be returned if the operation works successful. In other case false value has to be returned.
         */
        bool resize(size_t minSizeInc);

    private:

        //! @brief Pointer to the stream of bytes that contains the CDR representation.
        char *m_buffer;

        //! @brief The total size of the user's buffer.
        size_t m_bufferSize;

        //! @brief This variable indicates if the managed buffer is internal or is from the user.
        bool m_internalBuffer;
    };
};

#endif // _CPP_CDRBUFFER_H_
