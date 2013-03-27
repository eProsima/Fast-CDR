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
    */
    class eProsima_cpp_DllVariable CDRBuffer
    {
        friend class CDR;
    public:
        //! @brief This enumeration represents endianess.
        typedef enum
        {
            BIG_ENDIANESS = 0x0,
            LITTLE_ENDIANESS = 0x1
        } Endianess;

        static const Endianess DEFAULT_ENDIAN;

        class State
        {
            friend class CDR;
        private:

            State(CDRBuffer &cdrBuffer) : m_currentPosition(cdrBuffer.m_currentPosition) {}

            char *m_currentPosition;
        };

        /*!
        * @brief This constructor assigns the user's stream of bytes to the eProsima::CDRBuffer object.
        *
        * @param buffer The user's buffer that will be used. This buffer is not deallocated in the object's destruction. Cannot be NULL.
        * @param bufferLength The length of user's buffer.
        * @param endianess The initial endianess that will be used. By default is the endianess of the system.
        */
        CDRBuffer(char* const buffer, const size_t bufferLength, const Endianess endianess = DEFAULT_ENDIAN);

		//! @brief This function cheks the remaining space in the buffer.
        inline bool checkSpace(size_t dataSize){return m_bufferLength >= dataSize;}

        /*!
         * @brief This function returns the extra bytes regarding the allign.
         */
        inline size_t align(size_t dataSize){return dataSize > m_lastDataSize ? (dataSize - ((m_currentPosition - m_alignPosition) % dataSize)) & (dataSize-1) : 0;}

        /*!
         * @brief This function jumps the number of bytes of the align for the desired size.
         */
        inline void makeAlign(size_t align){m_currentPosition += align;}

		/*!
		 * @brief This function resets the align position for calculations to current position.
		 */
		inline void resetAlign(){m_alignPosition = m_currentPosition;}

        void reset();

    private:

        //! @brief Pointer to the stream of bytes that contains the CDR representation.
        char *m_buffer;

        const size_t m_bufferSize;

        //! @brief The remaining bytes in the stream.
        size_t m_bufferLength;

        //! @brief The current position in the serialization/deserialization process.
        char *m_currentPosition;

		//! @brief The position from the aligment is calculated.
		char *m_alignPosition;

        //! @brief The endianess that will be applied over the buffer.
        unsigned char m_endianess;

        //! @brief This attribute specified if it is needed to swap the bytes.
        bool m_swapBytes;

        //! Storage the last datasize serialized/deserialized.
        size_t m_lastDataSize;
    };
};

#endif // _CPP_CDRBUFFER_H_
