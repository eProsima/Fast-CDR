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

        void reset();

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
         * @brief This function serialize an octet.
         */
        inline bool operator<<(const uint8_t octet_t){return serialize(octet_t);}

        /*!
         * @brief This function serialize an char.
         */
        inline bool operator<<(const char char_t){return serialize(char_t);}

        /*!
        * @brief This function serialize a unsigned short.
        */
        inline bool operator<<(const uint16_t ushort_t){return serialize(ushort_t);}

        /*!
        * @brief This function serialize a short.
        */
        inline bool operator<<(const int16_t short_t){return serialize(short_t);}

        /*!
        * @brief This function serialize a unsigned long.
        */
        inline bool operator<<(const uint32_t ulong_t){return serialize(ulong_t);}

        /*!
        * @brief This function serialize a long.
        */
        inline bool operator<<(const int32_t long_t){return serialize(long_t);}

        /*!
        * @brief This function serialize a unsigned long long.
        */
        inline bool operator<<(const uint64_t ulonglong_t){return serialize(ulonglong_t);}

        /*!
        * @brief This function serialize a long long.
        */
        inline bool operator<<(const int64_t longlong_t){return serialize(longlong_t);}

        /*!
        * @brief This function serialize a float.
        */
        inline bool operator<<(const float float_t){return serialize(float_t);}

        /*!
        * @brief This function serialize a double.
        */
        inline bool operator<<(const double double_t){return serialize(double_t);}

        /*!
          * @brief This function serialize a boolean.
          */
        inline bool operator<<(const bool bool_t){return serialize(bool_t);}

        /*!
          * @brief This function serialize a string.
          */
        inline bool operator<<(const std::string &string_t){return serialize(string_t);}

        template<class _T, size_t _Size>
        inline bool operator<<(const std::array<_T, _Size> &array_t){return serialize<_T, _Size>(array_t);}

        template<class _T>
        inline bool operator<<(const std::vector<_T> &vector_t){return serialize<_T>(vector_t);}

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

        /*!
          * @brief This function deserialize a string.
          */
        inline bool operator>>(std::string &string_t){return deserialize(string_t);}

        template<class _T, size_t _Size>
        inline bool operator>>(std::array<_T, _Size> &array_t){return deserialize<_T, _Size>(array_t);}

        template<class _T>
        inline bool operator>>(std::vector<_T> &vector_t){return deserialize<_T>(vector_t);}

        /*!
          * @brief This function deserialize an octet.
          */
        inline
        bool serialize(const uint8_t octet_t)
        {
            return serialize((char)octet_t);
        }

        inline
        bool serialize(const uint8_t octet_t, CDRBuffer::Endianess endianess)
        {
            return serialize((char)octet_t, endianess);
        }

        /*!
          * @brief This function deserialize an char.
          */
        bool serialize(const char char_t);
        inline
        bool serialize(const char char_t, CDRBuffer::Endianess endianess)
        {
            return serialize(char_t);
        }

        /*!
          * @brief This function serialize a unsigned short.
          */
        inline
        bool serialize(const uint16_t ushort_t)
        {
            return serialize((int16_t)ushort_t);
        }
        inline
        bool serialize(const uint16_t ushort_t, CDRBuffer::Endianess endianess)
        {
            return serialize((int16_t)ushort_t, endianess);
        }

        /*!
          * @brief This function serialize a short.
          */
        bool serialize(const int16_t short_t);
        bool serialize(const int16_t short_t, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function serialize a unsigned long.
          */
        inline
        bool serialize(const uint32_t ulong_t)
        {
            return serialize((int32_t)ulong_t);
        }
        inline
        bool serialize(const uint32_t ulong_t, CDRBuffer::Endianess endianess)
        {
            return serialize((int32_t)ulong_t, endianess);
        }

        /*!
          * @brief This function serialize a long.
          */
        bool serialize(const int32_t long_t);
        bool serialize(const int32_t long_t, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function serialize a unsigned long long.
          */
        inline
        bool serialize(const uint64_t ulonglong_t)
        {
            return serialize((int64_t)ulonglong_t);
        }
        inline
        bool serialize(const uint64_t ulonglong_t, CDRBuffer::Endianess endianess)
        {
            return serialize((int64_t)ulonglong_t, endianess);
        }

        /*!
          * @brief This function serialize a long long.
          */
        bool serialize(const int64_t longlong_t);
        bool serialize(const int64_t longlong_t, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function serialize a float.
          */
        bool serialize(const float float_t);
        bool serialize(const float float_t, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function serialize a double.
          */
        bool serialize(const double double_t);
        bool serialize(const double double_t, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function serialize a bool.
          */
        bool serialize(const bool bool_t);
        inline
        bool serialize(const bool bool_t, CDRBuffer::Endianess endianess)
        {
            return serialize(bool_t);
        }

        /*!
          * @brief This function serialize a string.
          */
        bool serialize(const std::string &string_t);
        bool serialize(const std::string &string_t, CDRBuffer::Endianess endianess);

        template<class _T, size_t _Size>
        inline bool serialize(const std::array<_T, _Size> &array_t)
        { return serializeArray(array_t[0], array_t.size());}
        template<class _T, size_t _Size>
        inline bool serialize(const std::array<_T, _Size> &array_t, CDRBuffer::Endianess endianess)
        { return serializeArray(array_t[0], array_t.size(), endianess);}

        template<class _T>
        bool serialize(const std::vector<_T> &vector_t)
        {
            bool returnedValue = false;

            if(*this << (uint32_t)vector_t.size())
            {
                returnedValue = serializeArray(vector_t.data(), vector_t.size());
            }

            return returnedValue;
        }
        template<class _T>
        bool serialize(const std::vector<_T> &vector_t, CDRBuffer::Endianess endianess)
        {
            bool returnedValue = false;

            if(*this << (uint32_t)vector_t.size())
            {
                returnedValue = serializeArray(vector_t.data(), vector_t.size(), endianess);
            }

            return returnedValue;
        }

        /*!
          * @brief This function serialize an array of octets.
          */
        inline
        bool serializeArray(const uint8_t *octet_t, size_t numElements)
        {
            return serializeArray((const char*)octet_t, numElements);
        }
        inline
        bool serializeArray(const uint8_t *octet_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray((const char*)octet_t, numElements);
        }

        /*!
          * @brief This function serialize an array of chars.
          */
        bool serializeArray(const char *char_t, size_t numElements);
        inline
        bool serializeArray(const char *char_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray(char_t, numElements);
        }

        /*!
          * @brief This function serialize an array of unsigned short.
          */
        inline
        bool serializeArray(const uint16_t *ushort_t, size_t numElements)
        {
            return serializeArray((const int16_t*)ushort_t, numElements);
        }
        inline
        bool serializeArray(const uint16_t *ushort_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray((const int16_t*)ushort_t, numElements, endianess);
        }

        /*!
          * @brief This function serialize an array of shorts.
          */
        bool serializeArray(const int16_t *short_t, size_t numElements);
        bool serializeArray(const int16_t *short_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function serialize an array of unsigned longs.
          */
        inline
        bool serializeArray(const uint32_t *ulong_t, size_t numElements)
        {
            return serializeArray((const int32_t*)ulong_t, numElements);
        }
        inline
        bool serializeArray(const uint32_t *ulong_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray((const int32_t*)ulong_t, numElements, endianess);
        }

        /*!
          * @brief This function serialize an array of longs.
          */
        bool serializeArray(const int32_t *long_t, size_t numElements);
        bool serializeArray(const int32_t *long_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function serialize an array of unsigned long longs.
          */
        inline
        bool serializeArray(const uint64_t *ulonglong_t, size_t numElements)
        {
            return serializeArray((const int64_t*)ulonglong_t, numElements);
        }
        inline
        bool serializeArray(const uint64_t *ulonglong_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray((const int64_t*)ulonglong_t, numElements, endianess);
        }

        /*!
          * @brief This function serialize an array of long longs.
          */
        bool serializeArray(const int64_t *longlong_t, size_t numElements);
        bool serializeArray(const int64_t *longlong_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function serialize an array of chars.
          */
        bool serializeArray(const float *float_t, size_t numElements);
        bool serializeArray(const float *float_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function serialize an array of chars.
          */
        bool serializeArray(const double *double_t, size_t numElements);
        bool serializeArray(const double *double_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function deserialize a sequence of basic types.
          */
        template<typename _T>
        bool serializeSequence(const _T *t, size_t numElements)
        {
            bool returnedValue = false;

            if(*this << numElements)
            {
                returnedValue = serializeArray(t, numElements);
            }

            return returnedValue;
        }
        template<typename _T>
        bool serializeSequence(const _T *t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            bool returnedValue = false;

            //TODO Error
            if(*this << numElements)
            {
                returnedValue = serializeArray(t, numElements, endianess);
            }

            return returnedValue;
        }

        /*!
          * @brief This function deserialize an octet.
          */
        inline
        bool deserialize(uint8_t &octet_t)
        {
            return deserialize((char&)octet_t);
        }
        inline
        bool deserialize(uint8_t &octet_t, CDRBuffer::Endianess endianess)
        {
            return deserialize((char&)octet_t, endianess);
        }

        /*!
          * @brief This function deserialize an char.
          */
        bool deserialize(char &char_t);
        inline
        bool deserialize(char &char_t, CDRBuffer::Endianess endianess)
        {
            return deserialize(char_t);
        }

        /*!
          * @brief This function deserialize a unsigned short.
          */
        inline
        bool deserialize(uint16_t &ushort_t)
        {
            return deserialize((int16_t&)ushort_t);
        }
        inline
        bool deserialize(uint16_t &ushort_t, CDRBuffer::Endianess endianess)
        {
            return deserialize((int16_t&)ushort_t, endianess);
        }

        /*!
          * @brief This function deserialize a short.
          */
        bool deserialize(int16_t &short_t);
        bool deserialize(int16_t &short_t, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function deserialize a unsigned long.
          */
        inline
        bool deserialize(uint32_t &ulong_t)
        {
            return deserialize((int32_t&)ulong_t);
        }
        inline
        bool deserialize(uint32_t &ulong_t, CDRBuffer::Endianess endianess)
        {
            return deserialize((int32_t&)ulong_t, endianess);
        }

        /*!
          * @brief This function deserialize a long.
          */
        bool deserialize(int32_t &long_t);
        bool deserialize(int32_t &long_t, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function deserialize a unsigned long long.
          */
        inline
        bool deserialize(uint64_t &ulonglong_t)
        {
            return deserialize((int64_t&)ulonglong_t);
        }
        inline
        bool deserialize(uint64_t &ulonglong_t, CDRBuffer::Endianess endianess)
        {
            return deserialize((int64_t&)ulonglong_t, endianess);
        }

        /*!
          * @brief This function deserialize a long long.
          */
        bool deserialize(int64_t &longlong_t);
        bool deserialize(int64_t &longlong_t, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function deserialize a float.
          */
        bool deserialize(float &float_t);
        bool deserialize(float &float_t, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function deserialize a double.
          */
        bool deserialize(double &double_t);
        bool deserialize(double &double_t, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function deserialize a bool.
          */
        bool deserialize(bool &bool_t);
        inline
        bool deserialize(bool &bool_t, CDRBuffer::Endianess endianess)
        {
            return deserialize(bool_t);
        };

        /*!
          * @brief This function deserialize a string.
          */
        bool deserialize(std::string &string_t);
        bool deserialize(std::string &string_t, CDRBuffer::Endianess endianess);

        template<class _T, size_t _Size>
        inline bool deserialize(std::array<_T, _Size> &array_t)
        { return deserializeArray(array_t[0], array_t.size());}
        template<class _T, size_t _Size>
        inline bool deserialize(std::array<_T, _Size> &array_t, CDRBuffer::Endianess endianess)
        { return deserializeArray(array_t[0], array_t.size(), endianess);}

        template<class _T>
        bool deserialize(std::vector<_T> &vector_t)
        {
            bool returnedValue = false;
            uint32_t seqLength = 0;

            if(*this >> seqLength)
            {
                vector_t.resize(seqLength);
                returnedValue = deserializeArray(vector_t.data(), vector_t.size());
            }

            return returnedValue;
        }
        template<class _T>
        bool deserialize(std::vector<_T> &vector_t, CDRBuffer::Endianess endianess)
        {
            bool returnedValue = false;
            uint32_t seqLength = 0;

            //TODO ERrror
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
        inline
        bool deserializeArray(uint8_t *octet_t, size_t numElements)
        {
            return deserializeArray((char*)octet_t, numElements);
        }
        inline
        bool deserializeArray(uint8_t *octet_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray((char*)octet_t, numElements, endianess);
        }

        /*!
          * @brief This function deserialize an array of chars.
          */
        bool deserializeArray(char *char_t, size_t numElements);
        inline
        bool deserializeArray(char *char_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray(char_t, numElements);
        }

        /*!
          * @brief This function deserialize an array of unsigned short.
          */
        inline
        bool deserializeArray(uint16_t *ushort_t, size_t numElements)
        {
            return deserializeArray((int16_t*)ushort_t, numElements);
        }
        inline
        bool deserializeArray(uint16_t *ushort_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray((int16_t*)ushort_t, numElements, endianess);
        }

        /*!
          * @brief This function deserialize an array of shorts.
          */
        bool deserializeArray(int16_t *short_t, size_t numElements);
        bool deserializeArray(int16_t *short_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function deserialize an array of unsigned longs.
          */
        inline
        bool deserializeArray(uint32_t *ulong_t, size_t numElements)
        {
            return deserializeArray((int32_t*)ulong_t, numElements);
        }
        inline
        bool deserializeArray(uint32_t *ulong_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray((int32_t*)ulong_t, numElements, endianess);
        }

        /*!
          * @brief This function deserialize an array of longs.
          */
        bool deserializeArray(int32_t *long_t, size_t numElements);
        bool deserializeArray(int32_t *long_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function deserialize an array of unsigned long longs.
          */
        inline
        bool deserializeArray(uint64_t *ulonglong_t, size_t numElements)
        {
            return deserializeArray((int64_t*)ulonglong_t, numElements);
        }
        inline
        bool deserializeArray(uint64_t *ulonglong_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray((int64_t*)ulonglong_t, numElements, endianess);
        }

        /*!
          * @brief This function deserialize an array of long longs.
          */
        bool deserializeArray(int64_t *longlong_t, size_t numElements);
        bool deserializeArray(int64_t *longlong_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function deserialize an array of chars.
          */
        bool deserializeArray(float *float_t, size_t numElements);
        bool deserializeArray(float *float_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function deserialize an array of chars.
          */
        bool deserializeArray(double *double_t, size_t numElements);
        bool deserializeArray(double *double_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
          * @brief This function deserialize a sequence of basic types.
          */
        template<typename _T>
        bool deserializeSequence(_T *t, size_t maxNumElements, size_t &numElements)
        {
            bool returnedValue = false;

            if(*this >> numElements && numElements <= maxNumElements)
            {
                returnedValue = deserializeArray(t, numElements);
            }

            return returnedValue;
        }
        template<typename _T>
        bool deserializeSequence(_T *t, size_t maxNumElements, size_t &numElements, CDRBuffer::Endianess endianess)
        {
            bool returnedValue = false;

            // TODO Error
            if(*this >> numElements && numElements <= maxNumElements)
            {
                returnedValue = deserializeArray(t, numElements, endianess);
            }

            return returnedValue;
        }

    private:

        template<class _T, size_t _Size>
        bool serializeArray(const std::array<_T, _Size> &array_t, size_t numElements)
        {
            return serializeArray(array_t[0], numElements * array_t.size());
        }
        template<class _T, size_t _Size>
        bool serializeArray(const std::array<_T, _Size> &array_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray(array_t[0], numElements * array_t.size(), endianess);
        }

        template<typename _T>
        bool serializeArray(const _T &t, size_t numElements)
        {
            return serializeArray(&t, numElements);
        }
        template<typename _T>
        bool serializeArray(const _T &t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray(&t, numElements, endianess);
        }

        template<class _T, size_t _Size>
        bool deserializeArray(std::array<_T, _Size> &array_t, size_t numElements)
        {
            return deserializeArray(array_t[0], numElements * array_t.size());
        }
        template<class _T, size_t _Size>
        bool deserializeArray(std::array<_T, _Size> &array_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray(array_t[0], numElements * array_t.size(), endianess);
        }

        template<typename _T>
        bool deserializeArray(_T &t, size_t numElements)
        {
            return deserializeArray(&t, numElements);
        }
        template<typename _T>
        bool deserializeArray(_T &t, size_t numElements, CDRBuffer::Endianess endianess)
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
