#ifndef _CPP_CDRBUFFER_H_
#define _CPP_CDRBUFFER_H_

#include "eProsima_cpp/eProsima_cpp_dll.h"
#include <stdint.h>
#include <cstdio>

namespace eProsima
{
    /*!
     * @brief This class implements the iterator used to go through a FastBuffer.
     * @ingroup CDRAPIREFERENCE
     */
    class _FastBuffer_iterator
    {
    public:

        _FastBuffer_iterator() : m_index(0) {}

        explicit _FastBuffer_iterator(size_t index) : m_index(index) {}

    private:

        size_t m_index;
    };

    /*!
    * @brief This class represents a stream of bytes that contains or will contain
    * a CDR representation. This class is used by an object eProsima::CDR to serialize
    * or deserialize a CDR representation.
    * @ingroup CDRAPIREFERENCE
    */
    class eProsima_cpp_DllVariable FastBuffer
    {
        friend class Cdr;
        friend class FastCdr;
        typedef _FastBuffer_iterator iterator;
    public:

        /*!
         * @brief This class stores a state of a eProsima::FastBuffer. Its usage is dangerours when the eProsima::FastBuffer can use
         * a user's function to allocate dynamically memory.
         * @ingroup CDRAPIREFERENCE
         */
        class eProsima_cpp_DllVariable State
        {
            friend class Cdr;
            friend class FastCdr;
        private:

            /*!
             * @brief Default constructor.
             * @param FastBuffer The buffer that will be used to create the new state.
             */
            State(FastBuffer &FastBuffer);

             //! @brief The remaining bytes in the stream when the state was created.
            size_t m_bufferRemainLength;

            //! @brief The position in the buffer when the state was created.
            char *m_currentPosition;

		    //! @brief The position from the aligment is calculated,  when the state was created..
		    char *m_alignPosition;
        };

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

        /*!
         * @brief This function returns the length of the serialized data inside the stream.
         * @return The length of the serialized data.
         */
        inline size_t getSerializedDataLength() const { return m_currentPosition - m_buffer;}

        /*!
		 * @brief This function resets the eProsima::FastBuffer object. The serialization process starts at the beginning.
		 */
        void reset();

        inline
        iterator begin()
        {
            return (iterator(0));
        }

    private:

        /*!
         * @brief This function checks the remaining space in the buffer.
         * @param dataSize The spected size to be available.
         * @return True value if the space is available. In other case false value is returned.
         */
        inline bool checkSpace(size_t dataSize){return m_bufferRemainLength >= dataSize;}

        /*!
         * @brief This function jumps the number of bytes of the alignment. These bytes should be calculated with the function eProsima::FastBuffer::align.
         * @param align The number of bytes to be skipped.
         */
        inline void makeAlign(size_t align){m_currentPosition += align;}

		/*!
		 * @brief This function resets the align position for calculations to current position.
		 */
		inline void resetAlign(){m_alignPosition = m_currentPosition;}

        /*!
         * @brief This function resizes the raw buffer. It will call the user's defined function to make this job.
         * @param minSizeInc The minimun growth expected of the current raw buffer.
         * @return True value has to be returned if the operation works successful. In other case false value has to be returned.
         */
        bool resize(size_t minSizeInc);

        //! @brief Pointer to the stream of bytes that contains the CDR representation.
        char *m_buffer;

        //! @brief The total size of the user's buffer.
        size_t m_bufferSize;

        //! @brief The remaining bytes in the stream.
        size_t m_bufferRemainLength;

        //! @brief The current position in the serialization/deserialization process.
        char *m_currentPosition;

		//! @brief The position from the aligment is calculated.
		char *m_alignPosition;

        //! @brief This variable indicates if the managed buffer is internal or is from the user.
        bool m_internalBuffer;
    };
};

#endif // _CPP_CDRBUFFER_H_
