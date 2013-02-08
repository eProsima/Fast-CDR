#ifndef _CPP_CDR_H_
#define _CPP_CDR_H_

#include "eProsima_cpp/eProsima_cpp_dll.h"
#include "CdrBuffer.h"
#include <stdint.h>
#include <string>

namespace eProsima
{
    class eProsima_cpp_DllExport CDR
    {
    public:

        typedef enum
        {
            CORBA_CDR,
            DDS_CDR
        } CdrType;

        typedef enum
        {
            DDS_CDR_WITHOUT_PL = 0x0,
            DDS_CDR_WITH_PL = 0x2
        } DDSCdrPlFlag;

        /*!
         * @brief This constructor creates a eProsima::CDR object that could serialize/deserialize
         * the assigned buffer.
         *
         * @param cdrBuffer A reference to the buffer that contains or will contain the CDR representation.
         * @param cdrType Represents the type of CDR that will be use in serialization/deserialization. By default is CORBA CDR.
         */
        CDR(CDRBuffer &cdrBuffer, const CdrType cdrType = CORBA_CDR);

        /*!
         * @brief This function reads the encapsulation of the CDR stream.
         *         If the CDR stream contains a encapsulation, then this function would call before starting to deserialize.
         */
         bool read_encapsulation();

        /*!
         * @brief This function return the parameter list flag when the CDR type is DDS_CDR.
         */
        DDSCdrPlFlag getDDSCdrPlFlag() const;

        /*!
         * @brief This function returns the option flags when the CDR type is DDS_CDR.
         */
        uint16_t getDDSCdrOptions() const;

        /*!
         * @brief This function skip a number of bytes in the CDR stream.
         */
        bool jump(uint32_t numBytes);

        /*!
         * @brief This function returns the current position in the CDR stream;
         */
        char* getCurrentPosition();

        /*!
         * @brief This function returns the current state of the CDR stream.
         */
        CDRBuffer::State getState() const;

        /*!
         * @brief This function set a previous state of the CDR stream;
         */
        void setState(CDRBuffer::State state);

         /*!
          * @brief This function deserialize an octet.
          */
         bool operator>>(uint8_t &octet);

         /*!
          * @brief This function deserialize a unsigned short.
          */
         bool operator>>(uint16_t &ushort);

        /*!
          * @brief This function deserialize a unsigned long.
          */
         bool operator>>(uint32_t &ulong);

        /*!
          * @brief This function deserialize a unsigned long.
          */
        bool deserialize(uint32_t &ulong, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIAN);

        bool operator>>(std::string &string);
    private:

        //! @brief This function cheks the remaining space in the buffer.
        bool checkSpace(size_t dataSize);

        //! @brief Reference to the buffer that will be serialized/deserialized.
        CDRBuffer &m_cdrBuffer;

        //! @brief The type of CDR that will be use in serialization/deserialization.
        CdrType m_cdrType;

        //! @brief Using DDS_CDR type, this attribute stores if the stream buffer contains a parameter list or not.
        DDSCdrPlFlag m_plFlag;

        //! @brief This attribute stores the option flags when the CDR type is DDS_CDR;
        uint16_t m_options;
    };
};

#endif // _CPP_CDR_H_
