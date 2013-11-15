/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This copy of Fast Buffers is licensed to you under the terms described in the
 * FAST_BUFFERS_LICENSE file included in this distribution.
 *
 *************************************************************************/

#ifndef _CPP_CDRBUFFER_H_
#define _CPP_CDRBUFFER_H_

#include "Cdr_dll.h"
#include <stdint.h>
#include <cstdio>
#include <string.h>
#include <cstddef>

namespace eprosima
{
    /*!
     * @brief This class implements the iterator used to go through a FastBuffer.
     */
    class _FastBuffer_iterator
    {
    public:

        /*!
         * @brief Default constructor.
         * The iterator points any position.
         */
        _FastBuffer_iterator() : m_buffer(NULL), m_currentPosition(NULL) {}

        /*!
         * @brief Constructor.
         * The iterator points to the indicated position.
         * @param buffer Pointer to the raw buffer.
         * @param index Position of the raw buffer where the iterator will point.
         */
        explicit _FastBuffer_iterator(char *buffer, size_t index) : m_buffer(buffer), m_currentPosition(&m_buffer[index]){}

        /*!
         * @brief This operator changes the iterator's raw buffer.
         * This operator makes the iterator points to the same position but in other raw buffer.
         * The new raw buffer is the same than the source iterator's.
         * @param iterator The source iterator. The iterator will use the source iterator's raw buffer after this operation.
         */
        inline
        void operator<<(const _FastBuffer_iterator &iterator)
        {
            ptrdiff_t diff = m_currentPosition - m_buffer;
            m_buffer = iterator.m_buffer;
            m_currentPosition = m_buffer + diff;
        }

        /*!
         * @brief This operator changes the position where the iterator points.
         * This operator takes the index of the source iterator, but the iterator continue using its raw buffer.
         * @param The source iterator. The iterator will use the source's iterator index to point to its own raw buffer.
         */
        inline
        void operator>>(const _FastBuffer_iterator &iterator)
        {
            ptrdiff_t diff = iterator.m_currentPosition - iterator.m_buffer;
            m_currentPosition = m_buffer + diff;
        }

        /*!
         * @brief This operator copies a data in the raw buffer.
         * The copy uses the size of the data's type.
         * @param data Data to be copied. Cannot be NULL.
         */
        template<typename _T>
        inline
        void operator<<(const _T &data)
        {
            *((_T*)m_currentPosition) = data;
        }

        /*!
         * @brief This operator copies data from the raw buffer to a variable.
         * The copy uses the size of the data's type.
         * @param data Data to be filled.
         */
        template<typename _T>
        inline
        void operator>>(_T &data)
        {
            data = *((_T*)m_currentPosition);
        }

        /*!
         * @brief This function copies a buffer into the raw buffer.
         * @param src The source buffer.
         * @param size The size of bytes to be copied.
         */
        inline
        void memcopy(const void* src, const size_t size)
        {
            memcpy(m_currentPosition, src, size);
        }

        /*!
         * @brief This function copies from the raw buffer to a external buffer.
         * @param dst The destination buffer.
         * @param size The size of bytes to be copies.
         */
        inline
        void rmemcopy(void* dst, const size_t size)
        {
            memcpy(dst, m_currentPosition, size);
        }

        /*!
         * @brief This function increment the position where the iterator points.
         * @param numBytes Number of bytes the iterator move forward the position.
         */
        inline
        void operator+=(size_t numBytes)
        {
            m_currentPosition += numBytes;
        }

        /*!
         * @brief This operator returns the subtraction of the current interator's position and the source iterator's position.
         * @iterator Source iterator whose position is subtracted to the current iterator's position.
         * @return The result of subtract the current iterator's position and the source iterator's position.
         */
        inline
        size_t operator-(const _FastBuffer_iterator &it) const
        {
            return m_currentPosition - it.m_currentPosition;
        }

        /*!
         * @brief This function increments in one the position of the iterator.
         * @return The current iterator.
         */
        inline
        _FastBuffer_iterator operator++()
        {
            ++m_currentPosition;
            return *this;
        }

        /*!
         * @brief This function increments in one the position of the iterator.
         * @return The current iterator.
         */
        inline
        _FastBuffer_iterator operator++(int)
        {
            _FastBuffer_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        /*!
         * @brief This function returns the currento position in the raw buffer.
         * @return The current position in the raw buffer.
         */
        inline 
        char* operator&()
        {
            return m_currentPosition;
        }

    private:

        //! Pointer to the raw buffer.
        char *m_buffer;

        //! Current position in the raw buffer.
        char *m_currentPosition;
    };

    /*!
    * @brief This class represents a stream of bytes that contains or will contain
    * serialized data. This class is used by the serializers to serialize
    * or deserialize using their representation.
    * @ingroup CDRAPIREFERENCE
    */
    class Cdr_DllAPI FastBuffer
    {
    public:
        typedef _FastBuffer_iterator iterator;

        /*!
         * @brief This constructor creates an internal stream and assigns it to the eprosima::FastBuffer object.
         * The user can obtain this internal stream using the function eprosima::FastBuffer::getBuffer(). Be careful because this internal stream
         * is deleted in the destruction of the eprosima::FastBuffer object.
         */
        FastBuffer();

        /*!
         * @brief This constructor assigns the user's stream of bytes to the eprosima::FastBuffer object.
         * The user's stream will be used to serialize.
         *
         * @param buffer The user's buffer that will be used. This buffer is not deallocated in the object's destruction. Cannot be NULL.
         * @param bufferSize The length of user's buffer.
         */
        FastBuffer(char* const buffer, const size_t bufferSize);

        /*!
         * @brief This function returns the stream that the eprosima::FastBuffer uses to serialize data.
         * @return The stream used by eprosima::FastBuffer to serialize data.
         */
        inline char* getBuffer() const { return m_buffer;}

        /*!
         * @brief This function returns the size of the allocated memory of the stream that the eprosima::FastBuffer uses to serialize data.
         * @return The size of the allocated memory of the stream used by the eprosima::FastBuffer to serialize data.
         */
        inline size_t getBufferSize() const { return m_bufferSize;}

        /*!
         * @brief This function returns a iterator that points to the begining of the stream.
         * @return The new iterator.
         */
        inline
        iterator begin()
        {
            return (iterator(m_buffer, 0));
        }

        /*!
         * @brief This function returns a iterator that points to the end of the stream.
         * @return The new iterator.
         */
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

        //! @brief Pointer to the stream of bytes that contains the serialized data.
        char *m_buffer;

        //! @brief The total size of the user's buffer.
        size_t m_bufferSize;

        //! @brief This variable indicates if the managed buffer is internal or is from the user.
        bool m_internalBuffer;
    };
}; // namespace eprosima

#endif // _CPP_CDRBUFFER_H_
