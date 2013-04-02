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
         * @brief This enumeration represents endianess types.
         */
        typedef enum
        {
            //! @brief Big endianess.
            BIG_ENDIANESS = 0x0,
            //! @brief Little endianess.
            LITTLE_ENDIANESS = 0x1
        } Endianess;

        //! @brief Default endiness in the system.
        static const Endianess DEFAULT_ENDIAN;

        /*!
         * brief This class stores a state of a eProsima::CDRBuffer.
         */
        class State
        {
            friend class CDR;
        private:

            /*!
             * @brief Default constructor.
             * @param cdrBuffer The buffer that will be used to create the new state.
             */
            State(CDRBuffer &cdrBuffer) : m_currentPosition(cdrBuffer.m_currentPosition) {}

            //! @brief The position in the buffer when the state was created.
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

    private:

        /*!
         * @brief This function checks the remaining space in the buffer.
         * @param dataSize The spected size to be available.
         * @return True value if the space is available. In other case false value is returned.
         */
        inline bool checkSpace(size_t dataSize){return m_bufferLength >= dataSize;}

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
		 * @brief This function resets the current position in the buffer to the begining.
		 */
        void reset();

        //! @brief Pointer to the stream of bytes that contains the CDR representation.
        char *m_buffer;

        //! @brief The total size of the user's buffer.
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

        //! Stores the last datasize serialized/deserialized. It's used to optimize.
        size_t m_lastDataSize;
    };
};

#endif // _CPP_CDRBUFFER_H_
