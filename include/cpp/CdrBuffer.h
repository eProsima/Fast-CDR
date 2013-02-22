#ifndef _CPP_CDRBUFFER_H_
#define _CPP_CDRBUFFER_H_

#include "eProsima_cpp/eProsima_cpp_dll.h"
#include <stdint.h>

namespace eProsima
{
    /*!
    * @brief This class represents a stream of bytes that contains or will contain
    * a CDR representation. This class is used by an object eProsima::CDR to serialize
    * or deserialize a CDR representation.
    */
    class eProsima_cpp_DllExport CDRBuffer
    {
        friend class CDR;
    public:
        //! @brief This enumeration represents endianess.
        typedef enum
        {
            BIG_ENDIAN = 0x0,
            LITTLE_ENDIAN = 0x1,
            NO_ENDIAN = 0xF
        } Endianess;

#if defined(__LITTLE_ENDIAN__)
        static const Endianess DEFAULT_ENDIAN = LITTLE_ENDIAN;
#elif defined (__BIG_ENDIAN__)
        static const Endianess DEFAULT_ENDIAN = BIG_ENDIAN;
#endif

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
        inline bool checkSpace(size_t dataSize, uint32_t numElements = 1){return (m_bufferLength - (m_currentPosition - m_buffer) >= (dataSize * numElements) + align(dataSize));}

        /*!
         * @brief This function returns the extra bytes regarding the allign.
         */
        inline uint32_t align(size_t dataSize){return (dataSize - ((m_currentPosition - m_alignPosition) % dataSize)) & (dataSize-1);}

        /*!
         * @brief This function jumps the number of bytes of the align for the desired size.
         */
        inline void makeAlign(size_t dataSize){m_currentPosition += align(dataSize);}

		/*!
		 * @brief This function resets the align position for calculations to current position.
		 */
		inline void resetAlign(){m_alignPosition = m_currentPosition;}

		inline bool operator==(Endianess &endianess){return (endianess == CDRBuffer::NO_ENDIAN) || (m_endianess == endianess);}

		inline bool operator!=(Endianess &endianess){return !(endianess == CDRBuffer::NO_ENDIAN) && !(m_endianess == endianess);}

    private:

        //! @brief Pointer to the stream of bytes that contains the CDR representation.
        char *m_buffer;

        //! @brief The length of the stream of bytes.
        const size_t m_bufferLength;

        //! @brief The current position in the serialization/deserialization process.
        char *m_currentPosition;

		//! @brief The position from the aligment is calculated.
		char *m_alignPosition;

        //! @brief The endianess that will be applied over the buffer.
        unsigned char m_endianess;

        //! @brief This attribute specified if it is needed to swap the bytes.
        bool m_swapBytes;
    };
};

#endif // _CPP_CDRBUFFER_H_
