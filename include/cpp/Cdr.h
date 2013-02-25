#ifndef _CPP_CDR_H_
#define _CPP_CDR_H_

#include "eProsima_cpp/eProsima_cpp_dll.h"
#include "CdrBuffer.h"
#include <stdint.h>
#include <string>
#include <array>
#include <vector>

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
		 * @brief This function resets the alignment to current position.
		 */
		void resetAlignment();

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
        inline bool operator>>(uint8_t &octet_t){return deserialize(octet_t);}

        /*!
        * @brief This function deserialize an char.
        */
        inline bool operator>>(char &char_t){return deserialize(char_t);}

        /*!
        * @brief This function deserialize a unsigned short.
        */
        inline bool operator>>(uint16_t &ushort_t){return deserialize(ushort_t);}

        /*!
        * @brief This function deserialize a short.
        */
        inline bool operator>>(int16_t &short_t){return deserialize(short_t);}

        /*!
        * @brief This function deserialize a unsigned long.
        */
        inline bool operator>>(uint32_t &ulong_t){return deserialize(ulong_t);}

        /*!
        * @brief This function deserialize a long.
        */
        inline bool operator>>(int32_t &long_t){return deserialize(long_t);}

        /*!
        * @brief This function deserialize a unsigned long long.
        */
        inline bool operator>>(uint64_t &ulonglong_t){return deserialize(ulonglong_t);}

        /*!
        * @brief This function deserialize a long long.
        */
        inline bool operator>>(int64_t &longlong_t){return deserialize(longlong_t);}

        /*!
        * @brief This function deserialize a float.
        */
        inline bool operator>>(float &float_t){return deserialize(float_t);}

        /*!
        * @brief This function deserialize a double.
        */
        inline bool operator>>(double &double_t){return deserialize(double_t);}

        /*!
          * @brief This function deserialize a boolean.
          */
        inline bool operator>>(bool &bool_t){return deserialize(bool_t);}

         
        inline bool operator>>(std::string &string_t){return deserialize(string_t);}

        template<class _T, size_t _Size>
        inline bool operator>>(std::array<_T, _Size> &array_t){return deserialize<_T, _Size>(array_t);}

        template<class _T>
        inline bool operator>>(std::vector<_T> &vector_t){return deserialize<_T>(vector_t);}

         /*!
          * @brief This function deserialize an octet.
          */
        bool deserialize(uint8_t &octet_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize an char.
          */
        bool deserialize(char &char_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize a unsigned short.
          */
        bool deserialize(uint16_t &ushort_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize a short.
          */
        bool deserialize(int16_t &short_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize a unsigned long.
          */
        bool deserialize(uint32_t &ulong_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize a long.
          */
        bool deserialize(int32_t &long_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize a unsigned long long.
          */
        bool deserialize(uint64_t &ulonglong_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize a long long.
          */
        bool deserialize(int64_t &longlong_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize a float.
          */
        bool deserialize(float &float_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize a double.
          */
        bool deserialize(double &double_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize a bool.
          */
        bool deserialize(bool &bool_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize a string.
          */
        bool deserialize(std::string &string_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        template<class _T, size_t _Size>
        inline bool deserialize(std::array<_T, _Size> &array_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS)
        { return deserializeArray(array_t[0], array_t.size(), endianess);}

        template<class _T>
        bool deserialize(std::vector<_T> &vector_t, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS)
        {
            bool returnedValue = false;
            uint32_t seqLength = 0;

            if(*this >> seqLength)
            {
                vector_t.resize(seqLength);
                returnedValue = deserializeArray(vector_t.data(), vector_t.size(), endianess);
            }

            return returnedValue;
        }

        /*!
          * @brief This function deserialize an array of octets.
          */
        bool deserializeArray(uint8_t *octet_t, size_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize an array of unsigned short.
          */
        bool deserializeArray(uint16_t *ushort_t, size_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize an array of shorts.
          */
        bool deserializeArray(int16_t *short_t, size_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize an array of unsigned longs.
          */
        bool deserializeArray(uint32_t *ulong_t, size_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize an array of longs.
          */
        bool deserializeArray(int32_t *long_t, size_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize an array of unsigned long longs.
          */
        bool deserializeArray(uint64_t *ulonglong_t, size_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize an array of long longs.
          */
        bool deserializeArray(int64_t *longlong_t, size_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize an array of chars.
          */
        bool deserializeArray(char *char_t, size_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize an array of chars.
          */
        bool deserializeArray(float *float_t, size_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize an array of chars.
          */
        bool deserializeArray(double *double_t, size_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);

        /*!
          * @brief This function deserialize a sequence of basic types.
          */
        template<typename _T>
        bool deserializeSequence(_T *t, size_t maxNumElements, size_t &numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS)
        {
            bool returnedValue = false;

            if(*this >> numElements && numElements <= maxNumElements)
            {
                returnedValue = deserializeArray(t, numElements, endianess);
            }

            return returnedValue;
        }

    private:

        template<class _T, size_t _Size>
        bool deserializeArray(std::array<_T, _Size> &array_t, size_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS)
        {
            return deserializeArray(array_t[0], numElements * array_t.size(), endianess);
        }

        template<typename _T>
        bool deserializeArray(_T &t, size_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS)
        {
            return deserializeArray(&t, numElements, endianess);
        }

        /*template<class T>
        bool deserialize(T *array_pointer, uint32_t numElements, CDRBuffer::Endianess endianess = CDRBuffer::NO_ENDIANESS);*/

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
