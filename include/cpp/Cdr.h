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
    /*!
     * @brief This class offers an interface to serialize/deserialize some basic types using a eProsima::CDRBuffer.
     * @ingroup CDRAPIREFERENCE
     */
    class eProsima_cpp_DllExport CDR
    {
    public:

        //! @brief This enumeration represents the two type of CDR serialication that it supported by eProsima::CDR.
        typedef enum
        {
            //! @brief Common CORBA CDR serialization.
            CORBA_CDR,
            //! @brief DDS CDR serialization.
            DDS_CDR
        } CdrType;

        //! @brief This enumeration represents the two posible values of the flag that resolves if the content is a parameter list (only in DDS CDR).
        typedef enum
        {
            //! @brief Specify that the content is not a parameter list.
            DDS_CDR_WITHOUT_PL = 0x0,
            //! @brief Specify that the content is a parameter list.
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
         *        If the CDR stream contains a encapsulation, then this function would call before starting to deserialize.
         * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
         */
         bool read_encapsulation();

        /*!
         * @brief This function returns the parameter list flag when the CDR type is eProsima::DDS_CDR.
         * @return The flag that specifies if the content is a parameter list.
         */
        DDSCdrPlFlag getDDSCdrPlFlag() const;

        /*!
         * @brief This function returns the option flags when the CDR type is eProsima::DDS_CDR.
         * @return The option flag.
         */
        uint16_t getDDSCdrOptions() const;

        /*!
         * @brief This function skips a number of bytes in the CDR stream buffer.
         * @numBytes The number of bytes that will be jumped.
         * @return True value is returned when the jump operation works succesfully. In other case false value is returned.
         */
        bool jump(uint32_t numBytes);

		/*!
		 * @brief This function resets the alignment to current position in the buffer.
		 */
		void resetAlignment();

        /*!
		 * @brief This function resets the current position in the buffer to the begining.
		 */
        void reset();

        /*!
         * @brief This function returns the current position in the CDR stream.
         * @return Pointer to the current position in the buffer.
         */
        char* getCurrentPosition();

        /*!
         * @brief This function returns the current state of the CDR stream.
         * @return The current state of the buffer.
         */
        CDRBuffer::State getState() const;

        /*!
         * @brief This function sets a previous state of the CDR stream;
         * @param state Previous state that will be set again.
         */
        void setState(CDRBuffer::State state);

        /*!
         * @brief This operator serializes an octet.
         * @param octet_t The value of the octet that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline bool operator<<(const uint8_t octet_t){return serialize(octet_t);}

        /*!
         * @brief This operator serializes a char.
         * @param char_t The value of the character that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline bool operator<<(const char char_t){return serialize(char_t);}

        /*!
        * @brief This operator serializes a unsigned short.
        * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
        * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator<<(const uint16_t ushort_t){return serialize(ushort_t);}

        /*!
        * @brief This operator serializes a short.
        * @param short_t The value of the short that will be serialized in the buffer.
        * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator<<(const int16_t short_t){return serialize(short_t);}

        /*!
        * @brief This operator serializes a unsigned long.
        * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
        * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator<<(const uint32_t ulong_t){return serialize(ulong_t);}

        /*!
        * @brief This operator serializes a long.
        * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
        * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator<<(const int32_t long_t){return serialize(long_t);}

        /*!
        * @brief This operator serializes a unsigned long long.
        * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
        * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator<<(const uint64_t ulonglong_t){return serialize(ulonglong_t);}

        /*!
        * @brief This operator serializes a long long.
        * @param longlong_t The value of the long long that will be serialized in the buffer.
        * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator<<(const int64_t longlong_t){return serialize(longlong_t);}

        /*!
        * @brief This operator serializes a float.
        * @param float_t The value of the float that will be serialized in the buffer.
        * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator<<(const float float_t){return serialize(float_t);}

        /*!
        * @brief This operator serializes a double.
        * @param double_t The value of the double that will be serialized in the buffer.
        * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator<<(const double double_t){return serialize(double_t);}

        /*!
          * @brief This operator serializes a boolean.
          * @param bool_t The value of the boolean that will be serialized in the buffer.
          * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
          */
        inline bool operator<<(const bool bool_t){return serialize(bool_t);}

        /*!
          * @brief This operator serializes a string.
          * @param string_t The string that will be serialized in the buffer.
          * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
          */
        inline bool operator<<(const std::string &string_t){return serialize(string_t);}

        /*!
          * @brief This operator template is used to serialize arrays.
          * @param array_t The array that will be serialized in the buffer.
          * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
          */
        template<class _T, size_t _Size>
        inline bool operator<<(const std::array<_T, _Size> &array_t){return serialize<_T, _Size>(array_t);}

        /*!
          * @brief This operator template is used to serialize sequences.
          * @param vector_t The sequence that will be serialized in the buffer.
          * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
          */
        template<class _T>
        inline bool operator<<(const std::vector<_T> &vector_t){return serialize<_T>(vector_t);}

        /*!
        * @brief This operator deserializes an octet.
        * @param octet_t The variable that will store the octet readed from the buffer.
        * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator>>(uint8_t &octet_t){return deserialize(octet_t);}

        /*!
        * @brief This operator deserializes a char.
        * @param char_t The variable that will store the character readed from the buffer.
        * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator>>(char &char_t){return deserialize(char_t);}

        /*!
        * @brief This operator deserializes a unsigned short.
        * @param ushort_t The variable that will store the unsigned short readed from the buffer.
        * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator>>(uint16_t &ushort_t){return deserialize(ushort_t);}

        /*!
        * @brief This operator deserializes a short.
        * @param short_t The variable that will store the short readed from the buffer.
        * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator>>(int16_t &short_t){return deserialize(short_t);}

        /*!
        * @brief This operator deserializes a unsigned long.
        * @param ulong_t The variable that will store the unsigned long readed from the buffer.
        * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator>>(uint32_t &ulong_t){return deserialize(ulong_t);}

        /*!
        * @brief This operator deserializes a long.
        * @param long_t The variable that will store the long readed from the buffer.
        * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator>>(int32_t &long_t){return deserialize(long_t);}

        /*!
        * @brief This operator deserializes a unsigned long long.
        * @param ulonglong_t The variable that will store the unsigned long long readed from the buffer.
        * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator>>(uint64_t &ulonglong_t){return deserialize(ulonglong_t);}

        /*!
        * @brief This operator deserializes a long long.
        * @param longlong_t The variable that will store the long long readed from the buffer.
        * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator>>(int64_t &longlong_t){return deserialize(longlong_t);}

        /*!
        * @brief This operator deserializes a float.
        * @param float_t The variable that will store the float readed from the buffer.
        * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator>>(float &float_t){return deserialize(float_t);}

        /*!
        * @brief This operator deserializes a double.
        * @param double_t The variable that will store the double readed from the buffer.
        * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
        */
        inline bool operator>>(double &double_t){return deserialize(double_t);}

        /*!
          * @brief This operator deserializes a boolean.
          * @param bool_t The variable that will store the boolean readed from the buffer.
          * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
          */
        inline bool operator>>(bool &bool_t){return deserialize(bool_t);}

        /*!
          * @brief This operator deserializes a string.
          * @param string_t The variable that will store the string readed from the buffer.
          * @return True value is returned when the deserialization operation works succesfully. In other case false value is returned.
          */
        inline bool operator>>(std::string &string_t){return deserialize(string_t);}

        /*!
          * @brief This operator template is used to deserialize arrays.
          * @param array_t The variable that will store the array readed from the buffer.
          * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
          */
        template<class _T, size_t _Size>
        inline bool operator>>(std::array<_T, _Size> &array_t){return deserialize<_T, _Size>(array_t);}

        /*!
          * @brief This operator template is used to deserialize sequences.
          * @param vector_t The variable that will store the sequence readed from the buffer.
          * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
          */
        template<class _T>
        inline bool operator>>(std::vector<_T> &vector_t){return deserialize<_T>(vector_t);}

        /*!
         * @brief This function serializes an octet.
         * @param octet_t The value of the octet that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool serialize(const uint8_t octet_t)
        {
            return serialize((char)octet_t);
        }

        /*!
         * @brief This function serializes an octet with a different endianess.
         * @param octet_t The value of the octet that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool serialize(const uint8_t octet_t, CDRBuffer::Endianess endianess)
        {
            return serialize((char)octet_t, endianess);
        }

        /*!
         * @brief This function serializes a char.
         * @param char_t The value of the character that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool serialize(const char char_t);

        /*!
         * @brief This function serializes a char with a different endianess.
         * @param char_t The value of the character that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool serialize(const char char_t, CDRBuffer::Endianess endianess)
        {
            return serialize(char_t);
        }

        /*!
         * @brief This function serializes a unsigned short.
         * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool serialize(const uint16_t ushort_t)
        {
            return serialize((int16_t)ushort_t);
        }

        /*!
         * @brief This function serializes a unsigned short with a different endianess.
         * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool serialize(const uint16_t ushort_t, CDRBuffer::Endianess endianess)
        {
            return serialize((int16_t)ushort_t, endianess);
        }

        /*!
         * @brief This function serializes a short.
         * @param short_t The value of the short that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool serialize(const int16_t short_t);

        /*!
         * @brief This function serializes a short with a different endianess.
         * @param short_t The value of the short that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool serialize(const int16_t short_t, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function serializes a unsigned long.
         * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool serialize(const uint32_t ulong_t)
        {
            return serialize((int32_t)ulong_t);
        }

        /*!
         * @brief This function serializes a unsigned long with a different endianess.
         * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool serialize(const uint32_t ulong_t, CDRBuffer::Endianess endianess)
        {
            return serialize((int32_t)ulong_t, endianess);
        }

        /*!
         * @brief This function serializes a long.
         * @param long_t The value of the long that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool serialize(const int32_t long_t);

        /*!
         * @brief This function serializes a long with a different endianess.
         * @param long_t The value of the long that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool serialize(const int32_t long_t, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function serializes a unsigned long long.
         * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool serialize(const uint64_t ulonglong_t)
        {
            return serialize((int64_t)ulonglong_t);
        }

        /*!
         * @brief This function serializes a unsigned long long with a different endianess.
         * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool serialize(const uint64_t ulonglong_t, CDRBuffer::Endianess endianess)
        {
            return serialize((int64_t)ulonglong_t, endianess);
        }

        /*!
         * @brief This function serializes a long long.
         * @param longlong_t The value of the long long that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool serialize(const int64_t longlong_t);

        /*!
         * @brief This function serializes a long long with a different endianess.
         * @param longlong_t The value of the long long that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool serialize(const int64_t longlong_t, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function serializes a float.
         * @param float_t The value of the float that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool serialize(const float float_t);

        /*!
         * @brief This function serializes a float with a different endianess.
         * @param float_t The value of the float that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool serialize(const float float_t, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function serializes a double.
         * @param double_t The value of the double that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool serialize(const double double_t);

        /*!
         * @brief This function serializes a double with a different endianess.
         * @param double_t The value of the double that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool serialize(const double double_t, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function serializes a boolean.
         * @param bool_t The value of the boolean that will be serialized in the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool serialize(const bool bool_t);

        /*!
         * @brief This function serializes a boolean with a different endianess.
         * @param bool_t The value of the boolean that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool serialize(const bool bool_t, CDRBuffer::Endianess endianess)
        {
            return serialize(bool_t);
        }

        /*!
         * @brief This function serializes a string.
         * @param bool_t The string that will be serialized in the buffer.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool serialize(const std::string &string_t);

        /*!
         * @brief This function serializes a string with a different endianess.
         * @param bool_t The string that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool serialize(const std::string &string_t, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function template serializes an array.
         * @param array_t The array that will be serialized in the buffer.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<class _T, size_t _Size>
        inline bool serialize(const std::array<_T, _Size> &array_t)
        { return serializeArray(array_t[0], array_t.size());}

        /*!
         * @brief This function template serializes an array with a different endianess.
         * @param array_t The array that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<class _T, size_t _Size>
        inline bool serialize(const std::array<_T, _Size> &array_t, CDRBuffer::Endianess endianess)
        { return serializeArray(array_t[0], array_t.size(), endianess);}

        /*!
         * @brief This function template serializes a sequence.
         * @param vector_t The sequence that will be serialized in the buffer.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
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

        /*!
         * @brief This function template serializes a sequence with a different endianess.
         * @param vector_t The sequence that will be serialized in the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<class _T>
        bool serialize(const std::vector<_T> &vector_t, CDRBuffer::Endianess endianess)
        {
            bool returnedValue = false;

            if(serialize((uint32_t)vector_t.size(), endianess))
            {
                returnedValue = serializeArray(vector_t.data(), vector_t.size(), endianess);
            }

            return returnedValue;
        }

        /*!
         * @brief This function serializes an array of octets.
         * @param octet_t The sequence of octets  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool serializeArray(const uint8_t *octet_t, size_t numElements)
        {
            return serializeArray((const char*)octet_t, numElements);
        }

        /*!
         * @brief This function serializes an array of octets with a different endianess.
         * @param octet_t The array of octets  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool serializeArray(const uint8_t *octet_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray((const char*)octet_t, numElements);
        }

        /*!
         * @brief This function serializes an array of characteres.
         * @param char_t The array of characteres  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool serializeArray(const char *char_t, size_t numElements);

        /*!
         * @brief This function serializes an array of characteres with a different endianess.
         * @param char_t The array of characteres  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool serializeArray(const char *char_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray(char_t, numElements);
        }

        /*!
         * @brief This function serializes an array of unsigned shorts.
         * @param ushort_t The array of unsigned shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool serializeArray(const uint16_t *ushort_t, size_t numElements)
        {
            return serializeArray((const int16_t*)ushort_t, numElements);
        }

        /*!
         * @brief This function serializes an array of unsigned shorts with a different endianess.
         * @param ushort_t The array of unsigned shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool serializeArray(const uint16_t *ushort_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray((const int16_t*)ushort_t, numElements, endianess);
        }

        /*!
         * @brief This function serializes an array of shorts.
         * @param short_t The array of shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool serializeArray(const int16_t *short_t, size_t numElements);

        /*!
         * @brief This function serializes an array of shorts with a different endianess.
         * @param short_t The array of shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool serializeArray(const int16_t *short_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function serializes an array of unsigned longs.
         * @param ulong_t The array of unsigned longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool serializeArray(const uint32_t *ulong_t, size_t numElements)
        {
            return serializeArray((const int32_t*)ulong_t, numElements);
        }

        /*!
         * @brief This function serializes an array of unsigned longs with a different endianess.
         * @param ulong_t The array of unsigned longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool serializeArray(const uint32_t *ulong_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray((const int32_t*)ulong_t, numElements, endianess);
        }

        /*!
         * @brief This function serializes an array of longs.
         * @param long_t The array of longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool serializeArray(const int32_t *long_t, size_t numElements);

        /*!
         * @brief This function serializes an array of longs with a different endianess.
         * @param long_t The array of longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool serializeArray(const int32_t *long_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function serializes an array of unsigned long longs.
         * @param ulonglong_t The array of unsigned long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool serializeArray(const uint64_t *ulonglong_t, size_t numElements)
        {
            return serializeArray((const int64_t*)ulonglong_t, numElements);
        }

        /*!
         * @brief This function serializes an array of unsigned long longs with a different endianess.
         * @param ulonglong_t The array of unsigned long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool serializeArray(const uint64_t *ulonglong_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray((const int64_t*)ulonglong_t, numElements, endianess);
        }

        /*!
         * @brief This function serializes an array of long longs.
         * @param longlong_t The array of  long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool serializeArray(const int64_t *longlong_t, size_t numElements);

        /*!
         * @brief This function serializes an array of long longs with a different endianess.
         * @param longlong_t The array of  long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool serializeArray(const int64_t *longlong_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function serializes an array of floats.
         * @param float_t The array of floats that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool serializeArray(const float *float_t, size_t numElements);

        /*!
         * @brief This function serializes an array of floats with a different endianess.
         * @param float_t The array of floats that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool serializeArray(const float *float_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function serializes an array of doubles.
         * @param double_t The array of doubles that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool serializeArray(const double *double_t, size_t numElements);

        /*!
         * @brief This function serializes an array of doubles with a different endianess.
         * @param double_t The array of doubles that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
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
         * @brief This function deserializes an octet.
         * @param octet_t The variable that will store the octet readed from the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserialize(uint8_t &octet_t)
        {
            return deserialize((char&)octet_t);
        }

        /*!
         * @brief This function deserializes an octet with a different endianess.
         * @param octet_t The variable that will store the octet readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserialize(uint8_t &octet_t, CDRBuffer::Endianess endianess)
        {
            return deserialize((char&)octet_t, endianess);
        }

        /*!
         * @brief This function deserializes a characteres.
         * @param char_t The variable that will store the character readed from the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(char &char_t);

        /*!
         * @brief This function deserializes a characteres with a different endianess.
         * @param char_t The variable that will store the character readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserialize(char &char_t, CDRBuffer::Endianess endianess)
        {
            return deserialize(char_t);
        }

        /*!
         * @brief This function deserializes a unsigned short.
         * @param ushort_t The variable that will store the unsigned short readed from the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserialize(uint16_t &ushort_t)
        {
            return deserialize((int16_t&)ushort_t);
        }

        /*!
         * @brief This function deserializes a unsigned short with a different endianess.
         * @param ushort_t The variable that will store the unsigned short readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserialize(uint16_t &ushort_t, CDRBuffer::Endianess endianess)
        {
            return deserialize((int16_t&)ushort_t, endianess);
        }

        /*!
         * @brief This function deserializes a short.
         * @param short_t The variable that will store the short readed from the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(int16_t &short_t);

        /*!
         * @brief This function deserializes a short with a different endianess.
         * @param short_t The variable that will store the short readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(int16_t &short_t, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function deserializes a unsigned long.
         * @param ulong_t The variable that will store the unsigned long readed from the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserialize(uint32_t &ulong_t)
        {
            return deserialize((int32_t&)ulong_t);
        }

        /*!
         * @brief This function deserializes a unsigned long with a different endianess.
         * @param ulong_t The variable that will store the unsigned long readed from the buffer..
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserialize(uint32_t &ulong_t, CDRBuffer::Endianess endianess)
        {
            return deserialize((int32_t&)ulong_t, endianess);
        }

        /*!
         * @brief This function deserializes a long.
         * @param long_t The variable that will store the long readed from the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(int32_t &long_t);

        /*!
         * @brief This function deserializes a long with a different endianess.
         * @param long_t The variable that will store the long readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(int32_t &long_t, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function deserializes a unsigned long long.
         * @param ulonglong_t The variable that will store the unsigned long long readed from the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserialize(uint64_t &ulonglong_t)
        {
            return deserialize((int64_t&)ulonglong_t);
        }

        /*!
         * @brief This function deserializes a unsigned long long with a different endianess.
         * @param ulonglong_t The variable that will store the unsigned long long readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserialize(uint64_t &ulonglong_t, CDRBuffer::Endianess endianess)
        {
            return deserialize((int64_t&)ulonglong_t, endianess);
        }

        /*!
         * @brief This function deserializes a long long.
         * @param longlong_t The variable that will store the long long readed from the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(int64_t &longlong_t);

        /*!
         * @brief This function deserializes a long long with a different endianess.
         * @param longlong_t The variable that will store the long long readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(int64_t &longlong_t, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function deserializes a float.
         * @param float_t The variable that will store the float readed from the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(float &float_t);

        /*!
         * @brief This function deserializes a float with a different endianess.
         * @param float_t The variable that will store the float readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(float &float_t, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function deserializes a double.
         * @param double_t The variable that will store the double readed from the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(double &double_t);

        /*!
         * @brief This function deserializes a double with a different endianess.
         * @param double_t The variable that will store the double readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(double &double_t, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function deserializes a boolean.
         * @param bool_t The variable that will store the boolean readed from the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(bool &bool_t);

        /*!
         * @brief This function deserializes a boolean with a different endianess.
         * @param bool_t The variable that will store the boolean readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserialize(bool &bool_t, CDRBuffer::Endianess endianess)
        {
            return deserialize(bool_t);
        };

        /*!
         * @brief This function deserializes a string.
         * @param string_t The variable that will store the string readed from the buffer.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(std::string &string_t);

        /*!
         * @brief This function deserializes a string with a different endianess.
         * @param string_t The variable that will store the string readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization operation works succesfully. In other case false value is returned.
         */
        bool deserialize(std::string &string_t, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function template deserializes an array.
         * @param array_t The variable that will store the array readed from the buffer.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<class _T, size_t _Size>
        inline bool deserialize(std::array<_T, _Size> &array_t)
        { return deserializeArray(array_t[0], array_t.size());}

        /*!
         * @brief This function template deserializes an array with a different endianess.
         * @param array_t The variable that will store the array readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<class _T, size_t _Size>
        inline bool deserialize(std::array<_T, _Size> &array_t, CDRBuffer::Endianess endianess)
        { return deserializeArray(array_t[0], array_t.size(), endianess);}

        /*!
         * @brief This function template deserializes a sequence.
         * @param vector_t The variable that will store the sequence readed from the buffer.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
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

        /*!
         * @brief This function template deserializes a sequence with a different endianess.
         * @param vector_t The variable that will store the sequence readed from the buffer.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<class _T>
        bool deserialize(std::vector<_T> &vector_t, CDRBuffer::Endianess endianess)
        {
            bool returnedValue = false;
            uint32_t seqLength = 0;

            if(deserialize(seqLength, endianess))
            {
                vector_t.resize(seqLength);
                returnedValue = deserializeArray(vector_t.data(), vector_t.size(), endianess);
            }

            return returnedValue;
        }

        /*!
         * @brief This function deserializes an array of octets.
         * @param octet_t The variable that will store the array of octets readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserializeArray(uint8_t *octet_t, size_t numElements)
        {
            return deserializeArray((char*)octet_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of octets with a different endianess.
         * @param octet_t The variable that will store the array of octets readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserializeArray(uint8_t *octet_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray((char*)octet_t, numElements, endianess);
        }

        /*!
         * @brief This function deserializes an array of characteres.
         * @param char_t The variable that will store the array of characteres readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool deserializeArray(char *char_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of characteres with a different endianess.
         * @param char_t The variable that will store the array of characteres readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserializeArray(char *char_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray(char_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of unsigned short.
         * @param ushort_t The variable that will store the array of unsigned short readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserializeArray(uint16_t *ushort_t, size_t numElements)
        {
            return deserializeArray((int16_t*)ushort_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of unsigned short with a different endianess.
         * @param ushort_t The variable that will store the array of unsigned short readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserializeArray(uint16_t *ushort_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray((int16_t*)ushort_t, numElements, endianess);
        }

        /*!
         * @brief This function deserializes an array of short.
         * @param short_t The variable that will store the array of short readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool deserializeArray(int16_t *short_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of short with a different endianess.
         * @param short_t The variable that will store the array of short readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool deserializeArray(int16_t *short_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function deserializes an array of unsigned long.
         * @param ulong_t The variable that will store the array of unsigned long readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserializeArray(uint32_t *ulong_t, size_t numElements)
        {
            return deserializeArray((int32_t*)ulong_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of unsigned long with a different endianess.
         * @param ulong_t The variable that will store the array of unsigned long readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserializeArray(uint32_t *ulong_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray((int32_t*)ulong_t, numElements, endianess);
        }

        /*!
         * @brief This function deserializes an array of long.
         * @param long_t The variable that will store the array of long readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool deserializeArray(int32_t *long_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of long with a different endianess.
         * @param long_t The variable that will store the array of long readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool deserializeArray(int32_t *long_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function deserializes an array of unsigned long long.
         * @param ulonglong_t The variable that will store the array of unsigned long long readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserializeArray(uint64_t *ulonglong_t, size_t numElements)
        {
            return deserializeArray((int64_t*)ulonglong_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of unsigned long long with a different endianess.
         * @param ulonglong_t The variable that will store the array of unsigned long long readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        inline
        bool deserializeArray(uint64_t *ulonglong_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray((int64_t*)ulonglong_t, numElements, endianess);
        }

        /*!
         * @brief This function deserializes an array of long long.
         * @param longlong_t The variable that will store the array of long long readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool deserializeArray(int64_t *longlong_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of long long with a different endianess.
         * @param longlong_t The variable that will store the array of long long readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool deserializeArray(int64_t *longlong_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function deserializes an array of float.
         * @param float_t The variable that will store the array of float readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool deserializeArray(float *float_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of float with a different endianess.
         * @param float_t The variable that will store the array of float readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool deserializeArray(float *float_t, size_t numElements, CDRBuffer::Endianess endianess);

        /*!
         * @brief This function deserializes an array of double.
         * @param double_t The variable that will store the array of double readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        bool deserializeArray(double *double_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of double with a different endianess.
         * @param double_t The variable that will store the array of double readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
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

        /*!
         * @brief This function template detects the content type of an array and serializes the array.
         * @param array_t The array that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<class _T, size_t _Size>
        bool serializeArray(const std::array<_T, _Size> &array_t, size_t numElements)
        {
            return serializeArray(array_t[0], numElements * array_t.size());
        }

        /*!
         * @brief This function template detects the content type of an array and serializes the array with a different endianess.
         * @param array_t The array that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<class _T, size_t _Size>
        bool serializeArray(const std::array<_T, _Size> &array_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray(array_t[0], numElements * array_t.size(), endianess);
        }

        /*!
         * @brief This function template detects is used to detect the content type of an array and serializes the array.
         * @param t Reference to the first element of the array.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<typename _T>
        bool serializeArray(const _T &t, size_t numElements)
        {
            return serializeArray(&t, numElements);
        }

        /*!
         * @brief This function template detects is used to detect the content type of an array and serializes the array with a different endianess.
         * @param t Reference to the first element of the array.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<typename _T>
        bool serializeArray(const _T &t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return serializeArray(&t, numElements, endianess);
        }

        /*!
         * @brief This function template detects the content type of an array and deserializes the array.
         * @param array_t The variable that will store the array readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<class _T, size_t _Size>
        bool deserializeArray(std::array<_T, _Size> &array_t, size_t numElements)
        {
            return deserializeArray(array_t[0], numElements * array_t.size());
        }

        /*!
         * @brief This function template detects the content type of an array and deserializes the array with a different endianess.
         * @param array_t The variable that will store the array readed from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<class _T, size_t _Size>
        bool deserializeArray(std::array<_T, _Size> &array_t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray(array_t[0], numElements * array_t.size(), endianess);
        }


        /*!
         * @brief This function template is used to detect the content type of an array and deserializes the array.
         * @param t Reference to the first element of the array.
         * @param numElements Number of the elements in the array.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<typename _T>
        bool deserializeArray(_T &t, size_t numElements)
        {
            return deserializeArray(&t, numElements);
        }

        /*!
         * @brief This function template is used to detect the content type of an array and deserializes the array with a different endianess.
         * @param t Reference to the first element of the array.
         * @param numElements Number of the elements in the array.
         * @param endianess Endianess that will be used in the serialization of this value.
         * @return True value is returned when the serialization  operation works succesfully. In other case false value is returned.
         */
        template<typename _T>
        bool deserializeArray(_T &t, size_t numElements, CDRBuffer::Endianess endianess)
        {
            return deserializeArray(&t, numElements, endianess);
        }

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
