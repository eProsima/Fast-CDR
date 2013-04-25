#ifndef _CPP_CDR_H_
#define _CPP_CDR_H_

#include "eProsima_cpp/eProsima_cpp_dll.h"
#include "cpp/CdrBuffer.h"
#include "cpp/exceptions/Exception.h"
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
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
         */
         CDR& read_encapsulation();

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
         * @return True value is returned when the jump operation works successfully. In other case false value is returned.
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
        void setState(CDRBuffer::State &state);

        /*!
         * @brief This operator serializes an octet.
         * @param octet_t The value of the octet that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline CDR& operator<<(const uint8_t octet_t){return serialize(octet_t);}

        /*!
         * @brief This operator serializes a char.
         * @param char_t The value of the character that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline CDR& operator<<(const char char_t){return serialize(char_t);}

        /*!
        * @brief This operator serializes a unsigned short.
        * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline CDR& operator<<(const uint16_t ushort_t){return serialize(ushort_t);}

        /*!
        * @brief This operator serializes a short.
        * @param short_t The value of the short that will be serialized in the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline CDR& operator<<(const int16_t short_t){return serialize(short_t);}

        /*!
        * @brief This operator serializes a unsigned long.
        * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline CDR& operator<<(const uint32_t ulong_t){return serialize(ulong_t);}

        /*!
        * @brief This operator serializes a long.
        * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline CDR& operator<<(const int32_t long_t){return serialize(long_t);}

        /*!
        * @brief This operator serializes a unsigned long long.
        * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline CDR& operator<<(const uint64_t ulonglong_t){return serialize(ulonglong_t);}

        /*!
        * @brief This operator serializes a long long.
        * @param longlong_t The value of the long long that will be serialized in the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline CDR& operator<<(const int64_t longlong_t){return serialize(longlong_t);}

        /*!
        * @brief This operator serializes a float.
        * @param float_t The value of the float that will be serialized in the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline CDR& operator<<(const float float_t){return serialize(float_t);}

        /*!
        * @brief This operator serializes a double.
        * @param double_t The value of the double that will be serialized in the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline CDR& operator<<(const double double_t){return serialize(double_t);}

        /*!
          * @brief This operator serializes a boolean.
          * @param bool_t The value of the boolean that will be serialized in the buffer.
          * @return Reference to the eProsima::CDR object.
          * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
          */
        inline CDR& operator<<(const bool bool_t){return serialize(bool_t);}

        /*!
          * @brief This operator serializes a string.
          * @param string_t The string that will be serialized in the buffer.
          * @return Reference to the eProsima::CDR object.
          * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
          */
        inline CDR& operator<<(const std::string &string_t){return serialize(string_t);}

        /*!
          * @brief This operator template is used to serialize arrays.
          * @param array_t The array that will be serialized in the buffer.
          * @return Reference to the eProsima::CDR object.
          * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
          */
        template<class _T, size_t _Size>
        inline CDR& operator<<(const std::array<_T, _Size> &array_t){return serialize<_T, _Size>(array_t);}

        /*!
          * @brief This operator template is used to serialize sequences.
          * @param vector_t The sequence that will be serialized in the buffer.
          * @return Reference to the eProsima::CDR object.
          * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
          */
        template<class _T>
        inline CDR& operator<<(const std::vector<_T> &vector_t){return serialize<_T>(vector_t);}

        /*!
        * @brief This operator deserializes an octet.
        * @param octet_t The variable that will store the octet read from the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline CDR& operator>>(uint8_t &octet_t){return deserialize(octet_t);}

        /*!
        * @brief This operator deserializes a char.
        * @param char_t The variable that will store the character read from the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline CDR& operator>>(char &char_t){return deserialize(char_t);}

        /*!
        * @brief This operator deserializes a unsigned short.
        * @param ushort_t The variable that will store the unsigned short read from the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline CDR& operator>>(uint16_t &ushort_t){return deserialize(ushort_t);}

        /*!
        * @brief This operator deserializes a short.
        * @param short_t The variable that will store the short read from the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline CDR& operator>>(int16_t &short_t){return deserialize(short_t);}

        /*!
        * @brief This operator deserializes a unsigned long.
        * @param ulong_t The variable that will store the unsigned long read from the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline CDR& operator>>(uint32_t &ulong_t){return deserialize(ulong_t);}

        /*!
        * @brief This operator deserializes a long.
        * @param long_t The variable that will store the long read from the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline CDR& operator>>(int32_t &long_t){return deserialize(long_t);}

        /*!
        * @brief This operator deserializes a unsigned long long.
        * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline CDR& operator>>(uint64_t &ulonglong_t){return deserialize(ulonglong_t);}

        /*!
        * @brief This operator deserializes a long long.
        * @param longlong_t The variable that will store the long long read from the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline CDR& operator>>(int64_t &longlong_t){return deserialize(longlong_t);}

        /*!
        * @brief This operator deserializes a float.
        * @param float_t The variable that will store the float read from the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline CDR& operator>>(float &float_t){return deserialize(float_t);}

        /*!
        * @brief This operator deserializes a double.
        * @param double_t The variable that will store the double read from the buffer.
        * @return Reference to the eProsima::CDR object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline CDR& operator>>(double &double_t){return deserialize(double_t);}

        /*!
          * @brief This operator deserializes a boolean.
          * @param bool_t The variable that will store the boolean read from the buffer.
          * @return Reference to the eProsima::CDR object.
          * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
          * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
          */
        inline CDR& operator>>(bool &bool_t){return deserialize(bool_t);}

        /*!
          * @brief This operator deserializes a string.
          * @param string_t The variable that will store the string read from the buffer.
          * @return Reference to the eProsima::CDR object.
          * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
          */
        inline CDR& operator>>(std::string &string_t){return deserialize(string_t);}

        /*!
          * @brief This operator template is used to deserialize arrays.
          * @param array_t The variable that will store the array read from the buffer.
          * @return Reference to the eProsima::CDR object.
          * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
          */
        template<class _T, size_t _Size>
        inline CDR& operator>>(std::array<_T, _Size> &array_t){return deserialize<_T, _Size>(array_t);}

        /*!
          * @brief This operator template is used to deserialize sequences.
          * @param vector_t The variable that will store the sequence read from the buffer.
          * @return Reference to the eProsima::CDR object.
          * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
          */
        template<class _T>
        inline CDR& operator>>(std::vector<_T> &vector_t){return deserialize<_T>(vector_t);}

        /*!
         * @brief This function serializes an octet.
         * @param octet_t The value of the octet that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serialize(const uint8_t octet_t)
        {
            return serialize((char)octet_t);
        }

        /*!
         * @brief This function serializes an octet with a different endianness.
         * @param octet_t The value of the octet that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serialize(const uint8_t octet_t, CDRBuffer::Endianness endianness)
        {
            return serialize((char)octet_t, endianness);
        }

        /*!
         * @brief This function serializes an octet without alignment.
         * @param octet_t The value of the octet that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeFast(const uint8_t octet_t)
        {
            return serialize((char)octet_t);
        }

        /*!
         * @brief This function serializes an octet with a different endianness and without alignment.
         * @param octet_t The value of the octet that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeFast(const uint8_t octet_t, CDRBuffer::Endianness endianness)
        {
            return serialize((char)octet_t, endianness);
        }

        /*!
         * @brief This function serializes a char.
         * @param char_t The value of the character that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const char char_t);

        /*!
         * @brief This function serializes a char with a different endianness.
         * @param char_t The value of the character that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serialize(const char char_t, CDRBuffer::Endianness endianness)
        {
            return serialize(char_t);
        }

        /*!
         * @brief This function serializes a char without alignment.
         * @param char_t The value of the character that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeFast(const char char_t)
        {
            return serialize(char_t);
        }

        /*!
         * @brief This function serializes a char with a different endianness and without alignment.
         * @param char_t The value of the character that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeFast(const char char_t, CDRBuffer::Endianness endianness)
        {
            return serialize(char_t);
        }

        /*!
         * @brief This function serializes a unsigned short.
         * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serialize(const uint16_t ushort_t)
        {
            return serialize((int16_t)ushort_t);
        }

        /*!
         * @brief This function serializes a unsigned short with a different endianness.
         * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serialize(const uint16_t ushort_t, CDRBuffer::Endianness endianness)
        {
            return serialize((int16_t)ushort_t, endianness);
        }

        /*!
         * @brief This function serializes a unsigned short without alignment.
         * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeFast(const uint16_t ushort_t)
        {
            return serializeFast((int16_t)ushort_t);
        }

        /*!
         * @brief This function serializes a unsigned short with a different endianness and without alignment.
         * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeFast(const uint16_t ushort_t, CDRBuffer::Endianness endianness)
        {
            return serializeFast((int16_t)ushort_t, endianness);
        }

        /*!
         * @brief This function serializes a short.
         * @param short_t The value of the short that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const int16_t short_t);

        /*!
         * @brief This function serializes a short with a different endianness.
         * @param short_t The value of the short that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const int16_t short_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes a short without alignment.
         * @param short_t The value of the short that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeFast(const int16_t short_t);

        /*!
         * @brief This function serializes a short with a different endianness and without alignment.
         * @param short_t The value of the short that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeFast(const int16_t short_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes a unsigned long.
         * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serialize(const uint32_t ulong_t)
        {
            return serialize((int32_t)ulong_t);
        }

        /*!
         * @brief This function serializes a unsigned long with a different endianness.
         * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serialize(const uint32_t ulong_t, CDRBuffer::Endianness endianness)
        {
            return serialize((int32_t)ulong_t, endianness);
        }

        /*!
         * @brief This function serializes a unsigned long without alignment.
         * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeFast(const uint32_t ulong_t)
        {
            return serializeFast((int32_t)ulong_t);
        }

        /*!
         * @brief This function serializes a unsigned long with a different endianness and without alignment.
         * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeFast(const uint32_t ulong_t, CDRBuffer::Endianness endianness)
        {
            return serializeFast((int32_t)ulong_t, endianness);
        }

        /*!
         * @brief This function serializes a long.
         * @param long_t The value of the long that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const int32_t long_t);

        /*!
         * @brief This function serializes a long with a different endianness.
         * @param long_t The value of the long that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const int32_t long_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes a long without alignment.
         * @param long_t The value of the long that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeFast(const int32_t long_t);

        /*!
         * @brief This function serializes a long with a different endianness and without alignment.
         * @param long_t The value of the long that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeFast(const int32_t long_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes a unsigned long long.
         * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serialize(const uint64_t ulonglong_t)
        {
            return serialize((int64_t)ulonglong_t);
        }

        /*!
         * @brief This function serializes a unsigned long long with a different endianness.
         * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serialize(const uint64_t ulonglong_t, CDRBuffer::Endianness endianness)
        {
            return serialize((int64_t)ulonglong_t, endianness);
        }

        /*!
         * @brief This function serializes a unsigned long long without alignment.
         * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeFast(const uint64_t ulonglong_t)
        {
            return serializeFast((int64_t)ulonglong_t);
        }

        /*!
         * @brief This function serializes a unsigned long long with a different endianness and without alignment.
         * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeFast(const uint64_t ulonglong_t, CDRBuffer::Endianness endianness)
        {
            return serializeFast((int64_t)ulonglong_t, endianness);
        }

        /*!
         * @brief This function serializes a long long.
         * @param longlong_t The value of the long long that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const int64_t longlong_t);

        /*!
         * @brief This function serializes a long long with a different endianness.
         * @param longlong_t The value of the long long that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const int64_t longlong_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes a long long without alignment.
         * @param longlong_t The value of the long long that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeFast(const int64_t longlong_t);

        /*!
         * @brief This function serializes a long long with a different endianness and without alignment.
         * @param longlong_t The value of the long long that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeFast(const int64_t longlong_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes a float.
         * @param float_t The value of the float that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const float float_t);

        /*!
         * @brief This function serializes a float with a different endianness.
         * @param float_t The value of the float that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const float float_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes a float without alignment.
         * @param float_t The value of the float that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeFast(const float float_t);

        /*!
         * @brief This function serializes a float with a different endianness and without alignment.
         * @param float_t The value of the float that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeFast(const float float_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes a double.
         * @param double_t The value of the double that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const double double_t);

        /*!
         * @brief This function serializes a double with a different endianness.
         * @param double_t The value of the double that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const double double_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes a double without alignment.
         * @param double_t The value of the double that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeFast(const double double_t);

        /*!
         * @brief This function serializes a double with a different endianness and without alignment.
         * @param double_t The value of the double that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeFast(const double double_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes a boolean.
         * @param bool_t The value of the boolean that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const bool bool_t);

        /*!
         * @brief This function serializes a boolean with a different endianness.
         * @param bool_t The value of the boolean that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serialize(const bool bool_t, CDRBuffer::Endianness endianness)
        {
            return serialize(bool_t);
        }

        /*!
         * @brief This function serializes a boolean without alignment.
         * @param bool_t The value of the boolean that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeFast(const bool bool_t)
        {
            return serialize(bool_t);
        }

        /*!
         * @brief This function serializes a boolean with a different endianness and without alignment.
         * @param bool_t The value of the boolean that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeFast(const bool bool_t, CDRBuffer::Endianness endianness)
        {
            return serialize(bool_t);
        }

        /*!
         * @brief This function serializes a string.
         * @param bool_t The string that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const std::string &string_t);

        /*!
         * @brief This function serializes a string with a different endianness.
         * @param bool_t The string that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serialize(const std::string &string_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes a string without alignment.
         * @param bool_t The string that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeFast(const std::string &string_t);

        /*!
         * @brief This function serializes a string with a different endianness and without alignment..
         * @param bool_t The string that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeFast(const std::string &string_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function template serializes an array.
         * @param array_t The array that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        inline CDR& serialize(const std::array<_T, _Size> &array_t)
        { return serializeArray(array_t.data(), array_t.size());}

        /*!
         * @brief This function template serializes an array with a different endianness.
         * @param array_t The array that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        inline CDR& serialize(const std::array<_T, _Size> &array_t, CDRBuffer::Endianness endianness)
        { return serializeArray(array_t.data(), array_t.size(), endianness);}

        /*!
         * @brief This function template serializes an array without alignment.
         * @param array_t The array that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        inline CDR& serializeFast(const std::array<_T, _Size> &array_t)
        { return serializeArrayFast(array_t.data(), array_t.size());}

        /*!
         * @brief This function template serializes an array with a different endianness and without alignment.
         * @param array_t The array that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        inline CDR& serializeFast(const std::array<_T, _Size> &array_t, CDRBuffer::Endianness endianness)
        { return serializeArrayFast(array_t.data(), array_t.size(), endianness);}

        /*!
         * @brief This function template serializes a sequence.
         * @param vector_t The sequence that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        template<class _T>
        CDR& serialize(const std::vector<_T> &vector_t)
        {
            CDRBuffer::State state(m_cdrBuffer);

            *this << (int32_t)vector_t.size();

            try
            {
                return serializeArray(vector_t.data(), vector_t.size());
            }
            catch(Exception &ex)
            {
                setState(state);
                ex.raise();
            }

            return *this;
        }

        /*!
         * @brief This function template serializes a sequence with a different endianness.
         * @param vector_t The sequence that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        template<class _T>
        CDR& serialize(const std::vector<_T> &vector_t, CDRBuffer::Endianness endianness)
        {
            bool auxSwap = m_cdrBuffer.m_swapBytes;
            m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

            try
            {
                serialize(vector_t);
                m_cdrBuffer.m_swapBytes = auxSwap;
            }
            catch(Exception &ex)
            {
                m_cdrBuffer.m_swapBytes = auxSwap;
                ex.raise();
            }

            return *this;
        }

        /*!
         * @brief This function template serializes a sequence without alignment.
         * @param vector_t The sequence that will be serialized in the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        template<class _T>
        CDR& serializeFast(const std::vector<_T> &vector_t)
        {
            CDRBuffer::State state(m_cdrBuffer);

            serializeFast((int32_t)vector_t.size());

            try
            {
                return serializeArrayFast(vector_t.data(), vector_t.size());
            }
            catch(Exception &ex)
            {
                setState(state);
                ex.raise();
            }

            return *this;
        }

        /*!
         * @brief This function template serializes a sequence with a different endianness and without alignment.
         * @param vector_t The sequence that will be serialized in the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        template<class _T>
        CDR& serializeFast(const std::vector<_T> &vector_t, CDRBuffer::Endianness endianness)
        {
            bool auxSwap = m_cdrBuffer.m_swapBytes;
            m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

            try
            {
                serializeFast(vector_t);
                m_cdrBuffer.m_swapBytes = auxSwap;
            }
            catch(Exception &ex)
            {
                m_cdrBuffer.m_swapBytes = auxSwap;
                ex.raise();
            }

            return *this;
        }

        /*!
         * @brief This function serializes an array of octets.
         * @param octet_t The sequence of octets  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArray(const uint8_t *octet_t, size_t numElements)
        {
            return serializeArray((const char*)octet_t, numElements);
        }

        /*!
         * @brief This function serializes an array of octets with a different endianness.
         * @param octet_t The array of octets  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArray(const uint8_t *octet_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return serializeArray((const char*)octet_t, numElements);
        }

        /*!
         * @brief This function serializes an array of octets without alignment.
         * @param octet_t The sequence of octets  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArrayFast(const uint8_t *octet_t, size_t numElements)
        {
            return serializeArray((const char*)octet_t, numElements);
        }

        /*!
         * @brief This function serializes an array of octets with a different endianness and without alignment.
         * @param octet_t The array of octets  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArrayFast(const uint8_t *octet_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return serializeArray((const char*)octet_t, numElements);
        }

        /*!
         * @brief This function serializes an array of characteres.
         * @param char_t The array of characteres  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArray(const char *char_t, size_t numElements);

        /*!
         * @brief This function serializes an array of characteres with a different endianness.
         * @param char_t The array of characteres  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArray(const char *char_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return serializeArray(char_t, numElements);
        }

        /*!
         * @brief This function serializes an array of characteres without alignment.
         * @param char_t The array of characteres  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArrayFast(const char *char_t, size_t numElements)
        {
            return serializeArray(char_t, numElements);
        }

        /*!
         * @brief This function serializes an array of characteres with a different endianness and without alignment.
         * @param char_t The array of characteres  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArrayFast(const char *char_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return serializeArray(char_t, numElements);
        }

        /*!
         * @brief This function serializes an array of unsigned shorts.
         * @param ushort_t The array of unsigned shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArray(const uint16_t *ushort_t, size_t numElements)
        {
            return serializeArray((const int16_t*)ushort_t, numElements);
        }

        /*!
         * @brief This function serializes an array of unsigned shorts with a different endianness.
         * @param ushort_t The array of unsigned shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArray(const uint16_t *ushort_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return serializeArray((const int16_t*)ushort_t, numElements, endianness);
        }

        /*!
         * @brief This function serializes an array of unsigned shorts without alignment.
         * @param ushort_t The array of unsigned shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArrayFast(const uint16_t *ushort_t, size_t numElements)
        {
            return serializeArrayFast((const int16_t*)ushort_t, numElements);
        }

        /*!
         * @brief This function serializes an array of unsigned shorts with a different endianness and without alignment.
         * @param ushort_t The array of unsigned shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArrayFast(const uint16_t *ushort_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return serializeArrayFast((const int16_t*)ushort_t, numElements, endianness);
        }

        /*!
         * @brief This function serializes an array of shorts.
         * @param short_t The array of shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArray(const int16_t *short_t, size_t numElements);

        /*!
         * @brief This function serializes an array of shorts with a different endianness.
         * @param short_t The array of shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArray(const int16_t *short_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes an array of shorts without alignment.
         * @param short_t The array of shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArrayFast(const int16_t *short_t, size_t numElements);

        /*!
         * @brief This function serializes an array of shorts with a different endianness and without alignment.
         * @param short_t The array of shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArrayFast(const int16_t *short_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes an array of unsigned longs.
         * @param ulong_t The array of unsigned longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArray(const uint32_t *ulong_t, size_t numElements)
        {
            return serializeArray((const int32_t*)ulong_t, numElements);
        }

        /*!
         * @brief This function serializes an array of unsigned longs with a different endianness.
         * @param ulong_t The array of unsigned longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArray(const uint32_t *ulong_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return serializeArray((const int32_t*)ulong_t, numElements, endianness);
        }

        /*!
         * @brief This function serializes an array of unsigned longs without alignment.
         * @param ulong_t The array of unsigned longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArrayFast(const uint32_t *ulong_t, size_t numElements)
        {
            return serializeArrayFast((const int32_t*)ulong_t, numElements);
        }

        /*!
         * @brief This function serializes an array of unsigned longs with a different endianness and without alignment.
         * @param ulong_t The array of unsigned longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArrayFast(const uint32_t *ulong_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return serializeArrayFast((const int32_t*)ulong_t, numElements, endianness);
        }

        /*!
         * @brief This function serializes an array of longs.
         * @param long_t The array of longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArray(const int32_t *long_t, size_t numElements);

        /*!
         * @brief This function serializes an array of longs with a different endianness.
         * @param long_t The array of longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArray(const int32_t *long_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes an array of longs without alignment.
         * @param long_t The array of longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArrayFast(const int32_t *long_t, size_t numElements);

        /*!
         * @brief This function serializes an array of longs with a different endianness and without alignment.
         * @param long_t The array of longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArrayFast(const int32_t *long_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes an array of unsigned long longs.
         * @param ulonglong_t The array of unsigned long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArray(const uint64_t *ulonglong_t, size_t numElements)
        {
            return serializeArray((const int64_t*)ulonglong_t, numElements);
        }

        /*!
         * @brief This function serializes an array of unsigned long longs with a different endianness.
         * @param ulonglong_t The array of unsigned long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArray(const uint64_t *ulonglong_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return serializeArray((const int64_t*)ulonglong_t, numElements, endianness);
        }

        /*!
         * @brief This function serializes an array of unsigned long longs without alignment.
         * @param ulonglong_t The array of unsigned long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArrayFast(const uint64_t *ulonglong_t, size_t numElements)
        {
            return serializeArrayFast((const int64_t*)ulonglong_t, numElements);
        }

        /*!
         * @brief This function serializes an array of unsigned long longs with a different endianness and without alignment.
         * @param ulonglong_t The array of unsigned long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        CDR& serializeArrayFast(const uint64_t *ulonglong_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return serializeArrayFast((const int64_t*)ulonglong_t, numElements, endianness);
        }

        /*!
         * @brief This function serializes an array of long longs.
         * @param longlong_t The array of  long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArray(const int64_t *longlong_t, size_t numElements);

        /*!
         * @brief This function serializes an array of long longs with a different endianness.
         * @param longlong_t The array of  long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArray(const int64_t *longlong_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes an array of long longs without alignment.
         * @param longlong_t The array of  long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArrayFast(const int64_t *longlong_t, size_t numElements);

        /*!
         * @brief This function serializes an array of long longs with a different endianness and without alignment.
         * @param longlong_t The array of  long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArrayFast(const int64_t *longlong_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes an array of floats.
         * @param float_t The array of floats that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArray(const float *float_t, size_t numElements);

        /*!
         * @brief This function serializes an array of floats with a different endianness.
         * @param float_t The array of floats that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArray(const float *float_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes an array of floats without alignment.
         * @param float_t The array of floats that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArrayFast(const float *float_t, size_t numElements);

        /*!
         * @brief This function serializes an array of floats with a different endianness and without alignment.
         * @param float_t The array of floats that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArrayFast(const float *float_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes an array of doubles.
         * @param double_t The array of doubles that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArray(const double *double_t, size_t numElements);

        /*!
         * @brief This function serializes an array of doubles with a different endianness.
         * @param double_t The array of doubles that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArray(const double *double_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function serializes an array of doubles without aligment.
         * @param double_t The array of doubles that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArrayFast(const double *double_t, size_t numElements);

        /*!
         * @brief This function serializes an array of doubles with a different endianness and without alignment.
         * @param double_t The array of doubles that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        CDR& serializeArrayFast(const double *double_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes an octet.
         * @param octet_t The variable that will store the octet read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserialize(uint8_t &octet_t)
        {
            return deserialize((char&)octet_t);
        }

        /*!
         * @brief This function deserializes an octet with a different endianness.
         * @param octet_t The variable that will store the octet read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserialize(uint8_t &octet_t, CDRBuffer::Endianness endianness)
        {
            return deserialize((char&)octet_t, endianness);
        }

        /*!
         * @brief This function deserializes an octet without alignment.
         * @param octet_t The variable that will store the octet read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeFast(uint8_t &octet_t)
        {
            return deserialize((char&)octet_t);
        }

        /*!
         * @brief This function deserializes an octet with a different endianness and without alignment.
         * @param octet_t The variable that will store the octet read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeFast(uint8_t &octet_t, CDRBuffer::Endianness endianness)
        {
            return deserialize((char&)octet_t, endianness);
        }

        /*!
         * @brief This function deserializes a characteres.
         * @param char_t The variable that will store the character read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(char &char_t);

        /*!
         * @brief This function deserializes a characteres with a different endianness.
         * @param char_t The variable that will store the character read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserialize(char &char_t, CDRBuffer::Endianness endianness)
        {
            return deserialize(char_t);
        }

        /*!
         * @brief This function deserializes a characteres without alignment.
         * @param char_t The variable that will store the character read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeFast(char &char_t)
        {
            return deserialize(char_t);
        }

        /*!
         * @brief This function deserializes a characteres with a different endianness and without alignment.
         * @param char_t The variable that will store the character read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeFast(char &char_t, CDRBuffer::Endianness endianness)
        {
            return deserialize(char_t);
        }

        /*!
         * @brief This function deserializes a unsigned short.
         * @param ushort_t The variable that will store the unsigned short read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserialize(uint16_t &ushort_t)
        {
            return deserialize((int16_t&)ushort_t);
        }

        /*!
         * @brief This function deserializes a unsigned short with a different endianness.
         * @param ushort_t The variable that will store the unsigned short read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserialize(uint16_t &ushort_t, CDRBuffer::Endianness endianness)
        {
            return deserialize((int16_t&)ushort_t, endianness);
        }

        /*!
         * @brief This function deserializes a unsigned short without alignment.
         * @param ushort_t The variable that will store the unsigned short read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeFast(uint16_t &ushort_t)
        {
            return deserializeFast((int16_t&)ushort_t);
        }

        /*!
         * @brief This function deserializes a unsigned short with a different endianness and without alignment.
         * @param ushort_t The variable that will store the unsigned short read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeFast(uint16_t &ushort_t, CDRBuffer::Endianness endianness)
        {
            return deserializeFast((int16_t&)ushort_t, endianness);
        }

        /*!
         * @brief This function deserializes a short.
         * @param short_t The variable that will store the short read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(int16_t &short_t);

        /*!
         * @brief This function deserializes a short with a different endianness.
         * @param short_t The variable that will store the short read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(int16_t &short_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes a short without alignment.
         * @param short_t The variable that will store the short read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeFast(int16_t &short_t);

        /*!
         * @brief This function deserializes a short with a different endianness and without alignment.
         * @param short_t The variable that will store the short read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeFast(int16_t &short_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes a unsigned long.
         * @param ulong_t The variable that will store the unsigned long read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserialize(uint32_t &ulong_t)
        {
            return deserialize((int32_t&)ulong_t);
        }

        /*!
         * @brief This function deserializes a unsigned long with a different endianness.
         * @param ulong_t The variable that will store the unsigned long read from the buffer..
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserialize(uint32_t &ulong_t, CDRBuffer::Endianness endianness)
        {
            return deserialize((int32_t&)ulong_t, endianness);
        }

        /*!
         * @brief This function deserializes a unsigned long without alignment.
         * @param ulong_t The variable that will store the unsigned long read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeFast(uint32_t &ulong_t)
        {
            return deserializeFast((int32_t&)ulong_t);
        }

        /*!
         * @brief This function deserializes a unsigned long with a different endianness and without alignment.
         * @param ulong_t The variable that will store the unsigned long read from the buffer..
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeFast(uint32_t &ulong_t, CDRBuffer::Endianness endianness)
        {
            return deserializeFast((int32_t&)ulong_t, endianness);
        }

        /*!
         * @brief This function deserializes a long.
         * @param long_t The variable that will store the long read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(int32_t &long_t);

        /*!
         * @brief This function deserializes a long with a different endianness.
         * @param long_t The variable that will store the long read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(int32_t &long_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes a long without alignment.
         * @param long_t The variable that will store the long read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeFast(int32_t &long_t);

        /*!
         * @brief This function deserializes a long with a different endianness and without alignment.
         * @param long_t The variable that will store the long read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeFast(int32_t &long_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes a unsigned long long.
         * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserialize(uint64_t &ulonglong_t)
        {
            return deserialize((int64_t&)ulonglong_t);
        }

        /*!
         * @brief This function deserializes a unsigned long long with a different endianness.
         * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserialize(uint64_t &ulonglong_t, CDRBuffer::Endianness endianness)
        {
            return deserialize((int64_t&)ulonglong_t, endianness);
        }

        /*!
         * @brief This function deserializes a unsigned long long without aligment.
         * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeFast(uint64_t &ulonglong_t)
        {
            return deserializeFast((int64_t&)ulonglong_t);
        }

        /*!
         * @brief This function deserializes a unsigned long long with a different endianness and without alignment.
         * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeFast(uint64_t &ulonglong_t, CDRBuffer::Endianness endianness)
        {
            return deserializeFast((int64_t&)ulonglong_t, endianness);
        }

        /*!
         * @brief This function deserializes a long long.
         * @param longlong_t The variable that will store the long long read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(int64_t &longlong_t);

        /*!
         * @brief This function deserializes a long long with a different endianness.
         * @param longlong_t The variable that will store the long long read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(int64_t &longlong_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes a long long without alignment.
         * @param longlong_t The variable that will store the long long read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeFast(int64_t &longlong_t);

        /*!
         * @brief This function deserializes a long long with a different endianness and without alignment.
         * @param longlong_t The variable that will store the long long read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeFast(int64_t &longlong_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes a float.
         * @param float_t The variable that will store the float read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(float &float_t);

        /*!
         * @brief This function deserializes a float with a different endianness.
         * @param float_t The variable that will store the float read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(float &float_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes a float without alignment.
         * @param float_t The variable that will store the float read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeFast(float &float_t);

        /*!
         * @brief This function deserializes a float with a different endianness and without alignment.
         * @param float_t The variable that will store the float read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeFast(float &float_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes a double.
         * @param double_t The variable that will store the double read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(double &double_t);

        /*!
         * @brief This function deserializes a double with a different endianness.
         * @param double_t The variable that will store the double read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(double &double_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes a double without alignment.
         * @param double_t The variable that will store the double read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeFast(double &double_t);

        /*!
         * @brief This function deserializes a double with a different endianness and without alignment.
         * @param double_t The variable that will store the double read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeFast(double &double_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes a boolean.
         * @param bool_t The variable that will store the boolean read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
         */
        CDR& deserialize(bool &bool_t);

        /*!
         * @brief This function deserializes a boolean with a different endianness.
         * @param bool_t The variable that will store the boolean read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
         */
        inline
        CDR& deserialize(bool &bool_t, CDRBuffer::Endianness endianness)
        {
            return deserialize(bool_t);
        };

        /*!
         * @brief This function deserializes a boolean without alignment.
         * @param bool_t The variable that will store the boolean read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
         */
        inline
        CDR& deserializeFast(bool &bool_t)
        {
            return deserialize(bool_t);
        }

        /*!
         * @brief This function deserializes a boolean with a different endianness and without alignment.
         * @param bool_t The variable that will store the boolean read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
         */
        inline
        CDR& deserializeFast(bool &bool_t, CDRBuffer::Endianness endianness)
        {
            return deserialize(bool_t);
        };

        /*!
         * @brief This function deserializes a string.
         * @param string_t The variable that will store the string read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(std::string &string_t);

        /*!
         * @brief This function deserializes a string with a different endianness.
         * @param string_t The variable that will store the string read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserialize(std::string &string_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes a string without alignment.
         * @param string_t The variable that will store the string read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeFast(std::string &string_t);

        /*!
         * @brief This function deserializes a string with a different endianness and without alignment.
         * @param string_t The variable that will store the string read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeFast(std::string &string_t, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function template deserializes an array.
         * @param array_t The variable that will store the array read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        inline CDR& deserialize(std::array<_T, _Size> &array_t)
        { return deserializeArray(array_t.data(), array_t.size());}

        /*!
         * @brief This function template deserializes an array with a different endianness.
         * @param array_t The variable that will store the array read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        inline CDR& deserialize(std::array<_T, _Size> &array_t, CDRBuffer::Endianness endianness)
        { return deserializeArray(array_t.data(), array_t.size(), endianness);}

        /*!
         * @brief This function template deserializes an array without alignment.
         * @param array_t The variable that will store the array read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        inline CDR& deserializeFast(std::array<_T, _Size> &array_t)
        { return deserializeArrayFast(array_t.data(), array_t.size());}

        /*!
         * @brief This function template deserializes an array with a different endianness and without alignment.
         * @param array_t The variable that will store the array read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        inline CDR& deserializeFast(std::array<_T, _Size> &array_t, CDRBuffer::Endianness endianness)
        { return deserializeArrayFast(array_t.data(), array_t.size(), endianness);}

        /*!
         * @brief This function template deserializes a sequence.
         * @param vector_t The variable that will store the sequence read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T>
        CDR& deserialize(std::vector<_T> &vector_t)
        {
            uint32_t seqLength = 0;
            CDRBuffer::State state(m_cdrBuffer);

            *this >> seqLength;

            try
            {
                vector_t.resize(seqLength);
                return deserializeArray(vector_t.data(), vector_t.size());
            }
            catch(Exception &ex)
            {
                setState(state);
                ex.raise();
            }

            return *this;
        }

        /*!
         * @brief This function template deserializes a sequence with a different endianness.
         * @param vector_t The variable that will store the sequence read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T>
        CDR& deserialize(std::vector<_T> &vector_t, CDRBuffer::Endianness endianness)
        {
            bool auxSwap = m_cdrBuffer.m_swapBytes;
            m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

            try
            {
                deserialize(vector_t);
                m_cdrBuffer.m_swapBytes = auxSwap;
            }
            catch(Exception &ex)
            {
                m_cdrBuffer.m_swapBytes = auxSwap;
                ex.raise();
            }

            return *this;
        }

        /*!
         * @brief This function template deserializes a sequence without alignment.
         * @param vector_t The variable that will store the sequence read from the buffer.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T>
        CDR& deserializeFast(std::vector<_T> &vector_t)
        {
            uint32_t seqLength = 0;
            CDRBuffer::State state(m_cdrBuffer);

            serializeFast(seqLength);

            try
            {
                vector_t.resize(seqLength);
                return deserializeArrayFast(vector_t.data(), vector_t.size());
            }
            catch(Exception &ex)
            {
                setState(state);
                ex.raise();
            }

            return *this;
        }

        /*!
         * @brief This function template deserializes a sequence with a different endianness and without alignment.
         * @param vector_t The variable that will store the sequence read from the buffer.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T>
        CDR& deserializeFast(std::vector<_T> &vector_t, CDRBuffer::Endianness endianness)
        {
            bool auxSwap = m_cdrBuffer.m_swapBytes;
            m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

            try
            {
                deserializeFast(vector_t);
                m_cdrBuffer.m_swapBytes = auxSwap;
            }
            catch(Exception &ex)
            {
                m_cdrBuffer.m_swapBytes = auxSwap;
                ex.raise();
            }

            return *this;
        }

        /*!
         * @brief This function deserializes an array of octets.
         * @param octet_t The variable that will store the array of octets read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArray(uint8_t *octet_t, size_t numElements)
        {
            return deserializeArray((char*)octet_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of octets with a different endianness.
         * @param octet_t The variable that will store the array of octets read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArray(uint8_t *octet_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return deserializeArray((char*)octet_t, numElements, endianness);
        }

        /*!
         * @brief This function deserializes an array of octets without alignment.
         * @param octet_t The variable that will store the array of octets read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArrayFast(uint8_t *octet_t, size_t numElements)
        {
            return deserializeArray((char*)octet_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of octets with a different endianness and without alignment.
         * @param octet_t The variable that will store the array of octets read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArrayFast(uint8_t *octet_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return deserializeArray((char*)octet_t, numElements, endianness);
        }

        /*!
         * @brief This function deserializes an array of characteres.
         * @param char_t The variable that will store the array of characteres read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArray(char *char_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of characteres with a different endianness.
         * @param char_t The variable that will store the array of characteres read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArray(char *char_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return deserializeArray(char_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of characteres without alignment.
         * @param char_t The variable that will store the array of characteres read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArrayFast(char *char_t, size_t numElements)
        {
            return deserializeArray(char_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of characteres with a different endianness and without alignment.
         * @param char_t The variable that will store the array of characteres read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArrayFast(char *char_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return deserializeArray(char_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of unsigned short.
         * @param ushort_t The variable that will store the array of unsigned short read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArray(uint16_t *ushort_t, size_t numElements)
        {
            return deserializeArray((int16_t*)ushort_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of unsigned short with a different endianness.
         * @param ushort_t The variable that will store the array of unsigned short read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArray(uint16_t *ushort_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return deserializeArray((int16_t*)ushort_t, numElements, endianness);
        }

        /*!
         * @brief This function deserializes an array of unsigned short without alignment.
         * @param ushort_t The variable that will store the array of unsigned short read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArrayFast(uint16_t *ushort_t, size_t numElements)
        {
            return deserializeArrayFast((int16_t*)ushort_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of unsigned short with a different endianness and without alignment
         * @param ushort_t The variable that will store the array of unsigned short read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArrayFast(uint16_t *ushort_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return deserializeArrayFast((int16_t*)ushort_t, numElements, endianness);
        }

        /*!
         * @brief This function deserializes an array of short.
         * @param short_t The variable that will store the array of short read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArray(int16_t *short_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of short with a different endianness.
         * @param short_t The variable that will store the array of short read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArray(int16_t *short_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes an array of short without alignment.
         * @param short_t The variable that will store the array of short read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArrayFast(int16_t *short_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of short with a different endianness and without alignment.
         * @param short_t The variable that will store the array of short read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArrayFast(int16_t *short_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes an array of unsigned long.
         * @param ulong_t The variable that will store the array of unsigned long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArray(uint32_t *ulong_t, size_t numElements)
        {
            return deserializeArray((int32_t*)ulong_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of unsigned long with a different endianness.
         * @param ulong_t The variable that will store the array of unsigned long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArray(uint32_t *ulong_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return deserializeArray((int32_t*)ulong_t, numElements, endianness);
        }

        /*!
         * @brief This function deserializes an array of unsigned long without alignment.
         * @param ulong_t The variable that will store the array of unsigned long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArrayFast(uint32_t *ulong_t, size_t numElements)
        {
            return deserializeArrayFast((int32_t*)ulong_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of unsigned long with a different endianness and without alignment.
         * @param ulong_t The variable that will store the array of unsigned long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArrayFast(uint32_t *ulong_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return deserializeArrayFast((int32_t*)ulong_t, numElements, endianness);
        }

        /*!
         * @brief This function deserializes an array of long.
         * @param long_t The variable that will store the array of long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArray(int32_t *long_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of long with a different endianness.
         * @param long_t The variable that will store the array of long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArray(int32_t *long_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes an array of long without alignment.
         * @param long_t The variable that will store the array of long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArrayFast(int32_t *long_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of long with a different endianness and without alignment.
         * @param long_t The variable that will store the array of long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArrayFast(int32_t *long_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes an array of unsigned long long.
         * @param ulonglong_t The variable that will store the array of unsigned long long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArray(uint64_t *ulonglong_t, size_t numElements)
        {
            return deserializeArray((int64_t*)ulonglong_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of unsigned long long with a different endianness.
         * @param ulonglong_t The variable that will store the array of unsigned long long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArray(uint64_t *ulonglong_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return deserializeArray((int64_t*)ulonglong_t, numElements, endianness);
        }

        /*!
         * @brief This function deserializes an array of unsigned long long without alignment.
         * @param ulonglong_t The variable that will store the array of unsigned long long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArrayFast(uint64_t *ulonglong_t, size_t numElements)
        {
            return deserializeArrayFast((int64_t*)ulonglong_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of unsigned long long with a different endianness and without alignment.
         * @param ulonglong_t The variable that will store the array of unsigned long long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        CDR& deserializeArrayFast(uint64_t *ulonglong_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return deserializeArrayFast((int64_t*)ulonglong_t, numElements, endianness);
        }

        /*!
         * @brief This function deserializes an array of long long.
         * @param longlong_t The variable that will store the array of long long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArray(int64_t *longlong_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of long long with a different endianness.
         * @param longlong_t The variable that will store the array of long long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArray(int64_t *longlong_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes an array of long long without alignment.
         * @param longlong_t The variable that will store the array of long long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArrayFast(int64_t *longlong_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of long long with a different endianness and without alignment.
         * @param longlong_t The variable that will store the array of long long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArrayFast(int64_t *longlong_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes an array of float.
         * @param float_t The variable that will store the array of float read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArray(float *float_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of float with a different endianness.
         * @param float_t The variable that will store the array of float read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArray(float *float_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes an array of float without alignment.
         * @param float_t The variable that will store the array of float read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArrayFast(float *float_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of float with a different endianness and without alignment.
         * @param float_t The variable that will store the array of float read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArrayFast(float *float_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes an array of double.
         * @param double_t The variable that will store the array of double read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArray(double *double_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of double with a different endianness.
         * @param double_t The variable that will store the array of double read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArray(double *double_t, size_t numElements, CDRBuffer::Endianness endianness);

        /*!
         * @brief This function deserializes an array of double without alignment.
         * @param double_t The variable that will store the array of double read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArrayFast(double *double_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of double with a different endianness and without alignment.
         * @param double_t The variable that will store the array of double read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        CDR& deserializeArrayFast(double *double_t, size_t numElements, CDRBuffer::Endianness endianness);

    private:

        /*!
         * @brief This function template detects the content type of the STD container array and serializes the array.
         * @param array_t The array that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        CDR& serializeArray(const std::array<_T, _Size> *array_t, size_t numElements)
        {
            return serializeArray(array_t->data(), numElements * array_t->size());
        }

        /*!
         * @brief This function template detects the content type of the STD container array and serializes the array with a different endianness.
         * @param array_t The array that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        CDR& serializeArray(const std::array<_T, _Size> *array_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return serializeArray(array_t->data(), numElements * array_t->size(), endianness);
        }

        /*!
         * @brief This function template detects the content type of the STD container array and serializes the array.
         * @param array_t The array that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        CDR& serializeArrayFast(const std::array<_T, _Size> *array_t, size_t numElements)
        {
            return serializeArrayFast(array_t->data(), numElements * array_t->size());
        }

        /*!
         * @brief This function template detects the content type of the STD container array and serializes the array with a different endianness.
         * @param array_t The array that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        CDR& serializeArrayFast(const std::array<_T, _Size> *array_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return serializeArrayFast(array_t->data(), numElements * array_t->size(), endianness);
        }


        /*!
         * @brief This function template detects the content type of the STD container array and deserializes the array.
         * @param array_t The variable that will store the array read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        CDR& deserializeArray(std::array<_T, _Size> *array_t, size_t numElements)
        {
            return deserializeArray(array_t->data(), numElements * array_t->size());
        }

        /*!
         * @brief This function template detects the content type of STD container array and deserializes the array with a different endianness.
         * @param array_t The variable that will store the array read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        CDR& deserializeArray(std::array<_T, _Size> *array_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return deserializeArray(array_t->data(), numElements * array_t->size(), endianness);
        }

        /*!
         * @brief This function template detects the content type of the STD container array and deserializes the array.
         * @param array_t The variable that will store the array read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        CDR& deserializeArrayFast(std::array<_T, _Size> *array_t, size_t numElements)
        {
            return deserializeArrayFast(array_t->data(), numElements * array_t->size());
        }

        /*!
         * @brief This function template detects the content type of STD container array and deserializes the array with a different endianness.
         * @param array_t The variable that will store the array read from the buffer.
         * @param numElements Number of the elements in the array.
         * @param endianness Endianness that will be used in the serialization of this value.
         * @return Reference to the eProsima::CDR object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        CDR& deserializeArrayFast(std::array<_T, _Size> *array_t, size_t numElements, CDRBuffer::Endianness endianness)
        {
            return deserializeArrayFast(array_t->data(), numElements * array_t->size(), endianness);
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
