#ifndef _CPP_CDRBUFFER_H_
#define _CPP_CDRBUFFER_H_

#include "eProsima_cpp/eProsima_cpp_dll.h"
#include <stdint.h>
#include <cstdio>

namespace eProsima
{
    /*!
    * @brief This class represents a stream of bytes that contains or will contain
    * a CDR representation. This class is used by an object eProsima::CDR to serialize
    * or deserialize a CDR representation.
    * @ingroup CDRAPIREFERENCE
    */
    class eProsima_cpp_DllVariable CDRBuffer
    {
        friend class CDR;
    public:
        /*!
         * @brief This enumeration represents endianness types.
         */
        typedef enum
        {
            //! @brief Big endianness.
            BIG_ENDIANNESS = 0x0,
            //! @brief Little endianness.
            LITTLE_ENDIANNESS = 0x1
        } Endianness;

        //! @brief Default endiness in the system.
        static const Endianness DEFAULT_ENDIAN;

        /*!
         * @brief This class stores a state of a eProsima::CDRBuffer. Its usage is dangerours when the eProsima::CDRBuffer can use
         * a user's function to allocate dynamically memory.
         * @ingroup CDRAPIREFERENCE
         */
        class State
        {
            friend class CDR;
        private:

            /*!
             * @brief Default constructor.
             * @param cdrBuffer The buffer that will be used to create the new state.
             */
            State(CDRBuffer &cdrBuffer);

             //! @brief The remaining bytes in the stream when the state was created.
            size_t m_bufferRemainLength;

            //! @brief The position in the buffer when the state was created.
            char *m_currentPosition;

		    //! @brief The position from the aligment is calculated,  when the state was created..
		    char *m_alignPosition;

            //! @brief This attribute specified if it is needed to swap the bytes when the state was created..
            bool m_swapBytes;

            //! @brief Stores the last datasize serialized/deserialized when the state was created.
            size_t m_lastDataSize;
        };

        /*!
         * @brief This constructor creates an internal stream and assigns it to the eProsima::CDRBuffer object.
         * The user can obtain this internal stream using the function eProsima::CDRBuffer::getBuffer(). Be careful because this internal stream
         * is deleted in the destruction of the eProsima::CDRBuffer object.
         *
         * @param endianness The initial endianness that will be used. By default is the endianness of the system.
         */
        CDRBuffer(const Endianness endianness = DEFAULT_ENDIAN);

        /*!
         * @brief This constructor assigns the user's stream of bytes to the eProsima::CDRBuffer object.
         * The user's stream will be used to serialize.
         *
         * @param buffer The user's buffer that will be used. This buffer is not deallocated in the object's destruction. Cannot be NULL.
         * @param bufferSize The length of user's buffer.
         * @param endianness The initial endianness that will be used. By default is the endianness of the system.
         */
        CDRBuffer(char* const buffer, const size_t bufferSize, const Endianness endianness = DEFAULT_ENDIAN);

        /*!
         * @brief This function returns the stream that the eProsima::CDRBuffer uses to serialize data.
         * @return The stream used by eProsima::CDRBuffer to serialize data.
         */
        inline char* getBuffer() const { return m_buffer;}

        /*!
         * @brief This function returns the size of the allocated memory of the stream that the eProsima::CDRBuffer uses to serialize data.
         * @return The size of the allocated memory of the stream used by the eProsima::CDRBuffer to serialize data.
         */
        inline size_t getBufferSize() const { return m_bufferSize;}

        /*!
         * @brief This function returns the length of the serialized data inside the stream.
         * @return The length of the serialized data.
         */
        inline size_t getSerializedDataLength() const { return m_currentPosition - m_buffer;}

        /*!
		 * @brief This function resets the eProsima::CDRBuffer object. The serialization process starts at the beginning.
		 */
        void reset();

    private:

        /*!
         * @brief This function checks the remaining space in the buffer.
         * @param dataSize The spected size to be available.
         * @return True value if the space is available. In other case false value is returned.
         */
        inline bool checkSpace(size_t dataSize){return m_bufferRemainLength >= dataSize;}

        /*!
         * @brief This function returns the extra bytes regarding the allign.
         * @param dataSize The size of the data that will be serialized.
         * @return The size needed for the aligment.
         */
        inline size_t align(size_t dataSize){return dataSize > m_lastDataSize ? (dataSize - ((m_currentPosition - m_alignPosition) % dataSize)) & (dataSize-1) : 0;}

        /*!
         * @brief This function jumps the number of bytes of the alignment. These bytes should be calculated with the function eProsima::CDRBuffer::align.
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

        //! @brief The endianness that will be applied over the buffer.
        unsigned char m_endianness;

        //! @brief This attribute specified if it is needed to swap the bytes.
        bool m_swapBytes;

        //! @brief Stores the last datasize serialized/deserialized. It's used to optimize.
        size_t m_lastDataSize;

        //! @brief This variable indicates if the managed buffer is internal or is from the user.
        bool m_internalBuffer;
    };
};

#endif // _CPP_CDRBUFFER_H_
