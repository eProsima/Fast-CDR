// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _FASTCDR_CDR_H_
#define _FASTCDR_CDR_H_

#include "fastcdr_dll.h"
#include "FastBuffer.h"
#include "exceptions/NotEnoughMemoryException.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>

#if !__APPLE__ && !__FreeBSD__ && !__VXWORKS__
#include <malloc.h>
#else
#include <stdlib.h>
#endif // if !__APPLE__ && !__FreeBSD__ && !__VXWORKS__

#if HAVE_CXX0X
#include <array>
#endif // if HAVE_CXX0X

namespace eprosima {
namespace fastcdr {
/*!
 * @brief This class offers an interface to serialize/deserialize some basic types using CDR protocol inside an eprosima::fastcdr::FastBuffer.
 * @ingroup FASTCDRAPIREFERENCE
 */
class Cdr_DllAPI Cdr
{
public:

    //! @brief This enumeration represents the two kinds of CDR serialization supported by eprosima::fastcdr::CDR.
    typedef enum
    {
        //! @brief Common CORBA CDR serialization.
        CORBA_CDR,
        //! @brief DDS CDR serialization.
        DDS_CDR
    } CdrType;

    //! @brief This enumeration represents the two posible values of the flag that points if the content is a parameter list (only in DDS CDR).

#ifdef HAVE_CXX0X
    typedef enum : uint8_t
#else
    typedef enum
#endif // ifdef HAVE_CXX0X
    {
        //! @brief Specifies that the content is not a parameter list.
        DDS_CDR_WITHOUT_PL = 0x0,
        //! @brief Specifies that the content is a parameter list.
        DDS_CDR_WITH_PL = 0x2
    } DDSCdrPlFlag;

    /*!
     * @brief This enumeration represents endianness types.
     */
#ifdef HAVE_CXX0X
    typedef enum : uint8_t
#else
    typedef enum
#endif // ifdef HAVE_CXX0X
    {
        //! @brief Big endianness.
        BIG_ENDIANNESS = 0x0,
        //! @brief Little endianness.
        LITTLE_ENDIANNESS = 0x1
    } Endianness;

    //! @brief Default endiness in the system.
    static const Endianness DEFAULT_ENDIAN;

    /*!
     * @brief This class stores the current state of a CDR serialization.
     */
    class Cdr_DllAPI state
    {
        friend class Cdr;

    public:

        /*!
         * @brief Default constructor.
         */
        state(
                const Cdr& cdr);

        /*!
         * @brief Copy constructor.
         */
        state(
                const state&);

    private:

        state& operator =(
                const state&) = delete;

        //! @brief The position in the buffer when the state was created.
        const FastBuffer::iterator m_currentPosition;

        //! @brief The position from the aligment is calculated,  when the state was created..
        const FastBuffer::iterator m_alignPosition;

        //! @brief This attribute specified if it is needed to swap the bytes when the state was created..
        bool m_swapBytes;

        //! @brief Stores the last datasize serialized/deserialized when the state was created.
        size_t m_lastDataSize;
    };

    /*!
     * @brief This constructor creates an eprosima::fastcdr::Cdr object that can serialize/deserialize
     * the assigned buffer.
     *
     * @param cdrBuffer A reference to the buffer that contains (or will contain) the CDR representation.
     * @param endianness The initial endianness that will be used. The default value is the endianness of the system.
     * @param cdrType Represents the type of CDR that will be used in serialization/deserialization. The default value is CORBA CDR.
     */
    Cdr(
            FastBuffer& cdrBuffer,
            const Endianness endianness = DEFAULT_ENDIAN,
            const CdrType cdrType = CORBA_CDR);

    /*!
     * @brief This function reads the encapsulation of the CDR stream.
     *        If the CDR stream contains an encapsulation, then this function should be called before starting to deserialize.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     * @exception exception::BadParamException This exception is thrown when trying to deserialize an invalid value.
     */
    Cdr& read_encapsulation();

    /*!
     * @brief This function writes the encapsulation of the CDR stream.
     *        If the CDR stream should contain an encapsulation, then this function should be called before starting to serialize.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize_encapsulation();

    /*!
     * @brief This function returns the parameter list flag when the CDR type is eprosima::fastcdr::DDS_CDR.
     * @return The flag that specifies if the content is a parameter list.
     */
    DDSCdrPlFlag getDDSCdrPlFlag() const;

    /*!
     * @brief This function sets the parameter list flag when the CDR type is eprosima::fastcdr::DDS_CDR.
     * @param plFlag New value for the flag that specifies if the content is a parameter list.
     */
    void setDDSCdrPlFlag(
            DDSCdrPlFlag plFlag);

    /*!
     * @brief This function returns the option flags when the CDR type is eprosima::fastcdr::DDS_CDR.
     * @return The option flags.
     */
    uint16_t getDDSCdrOptions() const;

    /*!
     * @brief This function sets the option flags when the CDR type is eprosima::fastcdr::DDS_CDR.
     * @param options New value for the option flags.
     */
    void setDDSCdrOptions(
            uint16_t options);

    /*!
     * @brief This function sets the current endianness used by the CDR type.
     * @param endianness The new endianness value.
     */
    void changeEndianness(
            Endianness endianness);

    /*!
     * @brief This function returns the current endianness used by the CDR type.
     * @return The endianness.
     */
    Endianness endianness() const
    {
        return static_cast<Endianness>(m_endianness);
    }

    /*!
     * @brief This function skips a number of bytes in the CDR stream buffer.
     * @param numBytes The number of bytes that will be jumped.
     * @return True is returned when it works successfully. Otherwise, false is returned.
     */
    bool jump(
            size_t numBytes);

    /*!
     * @brief This function resets the current position in the buffer to the beginning.
     */
    void reset();

    /*!
     * @brief This function returns the pointer to the current used buffer.
     * @return Pointer to the starting position of the buffer.
     */
    char* getBufferPointer();

    /*!
     * @brief This function returns the current position in the CDR stream.
     * @return Pointer to the current position in the buffer.
     */
    char* getCurrentPosition();

    /*!
     * @brief This function returns the length of the serialized data inside the stream.
     * @return The length of the serialized data.
     */
    inline size_t getSerializedDataLength() const
    {
        return m_currentPosition - m_cdrBuffer.begin();
    }

    /*!
     * @brief Get the number of bytes needed to align a position to certain data size.
     * @param current_alignment Position to be aligned.
     * @param dataSize Size of next data to process (should be power of two).
     * @return Number of required alignment bytes.
     */
    inline static size_t alignment(
            size_t current_alignment,
            size_t dataSize)
    {
        return (dataSize - (current_alignment % dataSize)) & (dataSize - 1);
    }

    /*!
     * @brief This function returns the current state of the CDR serialization process.
     * @return The current state of the CDR serialization process.
     */
    state getState();

    /*!
     * @brief This function sets a previous state of the CDR serialization process;
     * @param state Previous state that will be set.
     */
    void setState(
            state& state);

    /*!
     * @brief This function moves the alignment forward.
     * @param numBytes The number of bytes the alignment should advance.
     * @return True If alignment was moved successfully.
     */
    bool moveAlignmentForward(
            size_t numBytes);

    /*!
     * @brief This function resets the alignment to the current position in the buffer.
     */
    inline void resetAlignment()
    {
        m_alignPosition = m_currentPosition;
    }

    /*!
     * @brief This operator serializes an octet.
     * @param octet_t The value of the octet that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const uint8_t octet_t)
    {
        return serialize(octet_t);
    }

    /*!
     * @brief This operator serializes a character.
     * @param char_t The value of the character that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const char char_t)
    {
        return serialize(char_t);
    }

    /*!
     * @brief This operator serializes a int8_t.
     * @param int8 The value of the int8_t that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const int8_t int8)
    {
        return serialize(int8);
    }

    /*!
     * @brief This operator serializes an unsigned short.
     * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const uint16_t ushort_t)
    {
        return serialize(ushort_t);
    }

    /*!
     * @brief This operator serializes a short.
     * @param short_t The value of the short that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const int16_t short_t)
    {
        return serialize(short_t);
    }

    /*!
     * @brief This operator serializes an unsigned long.
     * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const uint32_t ulong_t)
    {
        return serialize(ulong_t);
    }

    /*!
     * @brief This operator serializes a long.
     * @param long_t The value of the long that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const int32_t long_t)
    {
        return serialize(long_t);
    }

    /*!
     * @brief This operator serializes a wide-char.
     * @param wchar The value of the wide-char that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const wchar_t wchar)
    {
        return serialize(wchar);
    }

    /*!
     * @brief This operator serializes an unsigned long long.
     * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const uint64_t ulonglong_t)
    {
        return serialize(ulonglong_t);
    }

    /*!
     * @brief This operator serializes a long long.
     * @param longlong_t The value of the long long that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const int64_t longlong_t)
    {
        return serialize(longlong_t);
    }

    /*!
     * @brief This operator serializes a float.
     * @param float_t The value of the float that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const float float_t)
    {
        return serialize(float_t);
    }

    /*!
     * @brief This operator serializes a double.
     * @param double_t The value of the double that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const double double_t)
    {
        return serialize(double_t);
    }

    /*!
     * @brief This operator serializes a long double.
     * @param ldouble_t The value of the long double that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const long double ldouble_t)
    {
        return serialize(ldouble_t);
    }

    /*!
     * @brief This operator serializes a boolean.
     * @param bool_t The value of the boolean that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const bool bool_t)
    {
        return serialize(bool_t);
    }

    /*!
     * @brief This operator serializes a null-terminated c-string.
     * @param string_t Pointer to the begining of the string that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const char* string_t)
    {
        return serialize(string_t);
    }

    /*!
     * @brief This operator serializes a null-terminated c-string.
     * @param string_t Pointer to the begining of the string that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            char* string_t)
    {
        return serialize(string_t);
    }

    /*!
     * @brief This operator serializes a string.
     * @param string_t The string that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const std::string& string_t)
    {
        return serialize(string_t);
    }

    /*!
     * @brief This operator serializes a wstring.
     * @param string_t The wstring that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator <<(
            const std::wstring& string_t)
    {
        return serialize(string_t);
    }

#if HAVE_CXX0X
    /*!
     * @brief This operator template is used to serialize arrays.
     * @param array_t The array that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T, size_t _Size>
    inline Cdr& operator <<(
            const std::array<_T, _Size>& array_t)
    {
        return serialize<_T, _Size>(array_t);
    }

#endif // if HAVE_CXX0X

    /*!
     * @brief This operator template is used to serialize sequences.
     * @param vector_t The sequence that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T>
    inline Cdr& operator <<(
            const std::vector<_T>& vector_t)
    {
        return serialize<_T>(vector_t);
    }

    /*!
     * @brief This operator template is used to serialize maps.
     * @param map_t The map that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _K, class _T>
    inline Cdr& operator <<(
            const std::map<_K, _T>& map_t)
    {
        return serialize<_K, _T>(map_t);
    }

    /*!
     * @brief This operator template is used to serialize any other non-basic type.
     * @param type_t A reference to the object that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T>
    inline Cdr& operator <<(
            const _T& type_t)
    {
        type_t.serialize(*this);
        return *this;
    }

    /*!
     * @brief This operator deserializes an octet.
     * @param octet_t The variable that will store the octet read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            uint8_t& octet_t)
    {
        return deserialize(octet_t);
    }

    /*!
     * @brief This operator deserializes a character.
     * @param char_t The variable that will store the character read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            char& char_t)
    {
        return deserialize(char_t);
    }

    /*!
     * @brief This operator deserializes a int8_t.
     * @param int8 The variable that will store the int8_t read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            int8_t& int8)
    {
        return deserialize(int8);
    }

    /*!
     * @brief This operator deserializes an unsigned short.
     * @param ushort_t The variable that will store the unsigned short read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            uint16_t& ushort_t)
    {
        return deserialize(ushort_t);
    }

    /*!
     * @brief This operator deserializes a short.
     * @param short_t The variable that will store the short read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            int16_t& short_t)
    {
        return deserialize(short_t);
    }

    /*!
     * @brief This operator deserializes an unsigned long.
     * @param ulong_t The variable that will store the unsigned long read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            uint32_t& ulong_t)
    {
        return deserialize(ulong_t);
    }

    /*!
     * @brief This operator deserializes a long.
     * @param long_t The variable that will store the long read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            int32_t& long_t)
    {
        return deserialize(long_t);
    }

    // TODO in FastCdr
    /*!
     * @brief This operator deserializes a wide-char.
     * @param wchar The variable that will store the wide-char read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            wchar_t& wchar)
    {
        return deserialize(wchar);
    }

    /*!
     * @brief This operator deserializes a unsigned long long.
     * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            uint64_t& ulonglong_t)
    {
        return deserialize(ulonglong_t);
    }

    /*!
     * @brief This operator deserializes a long long.
     * @param longlong_t The variable that will store the long long read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            int64_t& longlong_t)
    {
        return deserialize(longlong_t);
    }

    /*!
     * @brief This operator deserializes a float.
     * @param float_t The variable that will store the float read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            float& float_t)
    {
        return deserialize(float_t);
    }

    /*!
     * @brief This operator deserializes a double.
     * @param double_t The variable that will store the double read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            double& double_t)
    {
        return deserialize(double_t);
    }

    /*!
     * @brief This operator deserializes a long double.
     * @param ldouble_t The variable that will store the long double read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            long double& ldouble_t)
    {
        return deserialize(ldouble_t);
    }

    /*!
     * @brief This operator deserializes a boolean.
     * @param bool_t The variable that will store the boolean read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     * @exception exception::BadParamException This exception is thrown when trying to deserialize an invalid value.
     */
    inline Cdr& operator >>(
            bool& bool_t)
    {
        return deserialize(bool_t);
    }

    /*!
     * @brief This operator deserializes a null-terminated c-string.
     * @param string_t The variable that will store the c-string read from the buffer.
     *                 Please note that a newly allocated string will be returned.
     *                 The caller should free the returned pointer when appropiate.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     * @exception exception::BadParamException This exception is thrown when trying to deserialize an invalid value.
     */
    inline Cdr& operator >>(
            char*& string_t)
    {
        return deserialize(string_t);
    }

    /*!
     * @brief This operator deserializes a string.
     * @param string_t The variable that will store the string read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            std::string& string_t)
    {
        return deserialize(string_t);
    }

    /*!
     * @brief This operator deserializes a string.
     * @param string_t The variable that will store the string read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline Cdr& operator >>(
            std::wstring& string_t)
    {
        return deserialize(string_t);
    }

#if HAVE_CXX0X
    /*!
     * @brief This operator template is used to deserialize arrays.
     * @param array_t The variable that will store the array read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T, size_t _Size>
    inline Cdr& operator >>(
            std::array<_T, _Size>& array_t)
    {
        return deserialize<_T, _Size>(array_t);
    }

#endif // if HAVE_CXX0X

    /*!
     * @brief This operator template is used to deserialize sequences.
     * @param vector_t The variable that will store the sequence read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T>
    inline Cdr& operator >>(
            std::vector<_T>& vector_t)
    {
        return deserialize<_T>(vector_t);
    }

    /*!
     * @brief This operator template is used to deserialize maps.
     * @param map_t The variable that will store the map read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _K, class _T>
    inline Cdr& operator >>(
            std::map<_K, _T>& map_t)
    {
        return deserialize<_K, _T>(map_t);
    }

    /*!
     * @brief This operator template is used to deserialize any other non-basic type.
     * @param type_t The variable that will store the object read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T>
    inline Cdr& operator >>(
            _T& type_t)
    {
        type_t.deserialize(*this);
        return *this;
    }

    /*!
     * @brief This function serializes an octet.
     * @param octet_t The value of the octet that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const uint8_t octet_t)
    {
        return serialize(static_cast<char>(octet_t));
    }

    /*!
     * @brief This function serializes an octet with a different endianness.
     * @param octet_t The value of the octet that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const uint8_t octet_t,
            Endianness endianness)
    {
        return serialize(static_cast<char>(octet_t), endianness);
    }

    /*!
     * @brief This function serializes a character.
     * @param char_t The value of the character that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const char char_t);

    /*!
     * @brief This function serializes a character with a different endianness.
     * @param char_t The value of the character that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const char char_t,
            Endianness endianness)
    {
        (void) endianness;
        return serialize(char_t);
    }

    /*!
     * @brief This function serializes an int8_t.
     * @param int8 The value of the int8_t that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const int8_t int8)
    {
        return serialize(static_cast<char>(int8));
    }

    /*!
     * @brief This function serializes an int8_t with a different endianness.
     * @param int8 The value of the int8_t that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const int8_t int8,
            Endianness endianness)
    {
        return serialize(static_cast<char>(int8), endianness);
    }

    /*!
     * @brief This function serializes an unsigned short.
     * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const uint16_t ushort_t)
    {
        return serialize(static_cast<int16_t>(ushort_t));
    }

    /*!
     * @brief This function serializes an unsigned short with a different endianness.
     * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const uint16_t ushort_t,
            Endianness endianness)
    {
        return serialize(static_cast<int16_t>(ushort_t), endianness);
    }

    /*!
     * @brief This function serializes a short.
     * @param short_t The value of the short that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const int16_t short_t);

    /*!
     * @brief This function serializes a short with a different endianness.
     * @param short_t The value of the short that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const int16_t short_t,
            Endianness endianness);

    /*!
     * @brief This function serializes an unsigned long.
     * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const uint32_t ulong_t)
    {
        return serialize(static_cast<int32_t>(ulong_t));
    }

    /*!
     * @brief This function serializes an unsigned long with a different endianness.
     * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const uint32_t ulong_t,
            Endianness endianness)
    {
        return serialize(static_cast<int32_t>(ulong_t), endianness);
    }

    /*!
     * @brief This function serializes a long.
     * @param long_t The value of the long that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const int32_t long_t);

    /*!
     * @brief This function serializes a long with a different endianness.
     * @param long_t The value of the long that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const int32_t long_t,
            Endianness endianness);

    /*!
     * @brief This function serializes a wide-char.
     * @param wchar The value of the wide-char that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const wchar_t wchar)
    {
        return serialize(static_cast<uint32_t>(wchar));
    }

    /*!
     * @brief This function serializes a wide-char with a different endianness.
     * @param wchar The value of the wide-char that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const wchar_t wchar,
            Endianness endianness)
    {
        return serialize(static_cast<uint32_t>(wchar), endianness);
    }

    /*!
     * @brief This function serializes an unsigned long long.
     * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const uint64_t ulonglong_t)
    {
        return serialize(static_cast<int64_t>(ulonglong_t));
    }

    /*!
     * @brief This function serializes an unsigned long long with a different endianness.
     * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const uint64_t ulonglong_t,
            Endianness endianness)
    {
        return serialize(static_cast<int64_t>(ulonglong_t), endianness);
    }

    /*!
     * @brief This function serializes a long long.
     * @param longlong_t The value of the long long that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const int64_t longlong_t);

    /*!
     * @brief This function serializes a long long with a different endianness.
     * @param longlong_t The value of the long long that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const int64_t longlong_t,
            Endianness endianness);

    /*!
     * @brief This function serializes a float.
     * @param float_t The value of the float that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const float float_t);

    /*!
     * @brief This function serializes a float with a different endianness.
     * @param float_t The value of the float that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const float float_t,
            Endianness endianness);

    /*!
     * @brief This function serializes a double.
     * @param double_t The value of the double that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const double double_t);

    /*!
     * @brief This function serializes a double with a different endianness.
     * @param double_t The value of the double that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const double double_t,
            Endianness endianness);


    /*!
     * @brief This function serializes a long double.
     * @param ldouble_t The value of the long double that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     * @note Due to internal representation differences, WIN32 and *NIX like systems are not compatible.
     */
    Cdr& serialize(
            const long double ldouble_t);

    /*!
     * @brief This function serializes a long double with a different endianness.
     * @param ldouble_t The value of the long double that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     * @note Due to internal representation differences, WIN32 and *NIX like systems are not compatible.
     */
    Cdr& serialize(
            const long double ldouble_t,
            Endianness endianness);

    /*!
     * @brief This function serializes a boolean.
     * @param bool_t The value of the boolean that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const bool bool_t);

    /*!
     * @brief This function serializes a boolean with a different endianness.
     * @param bool_t The value of the boolean that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const bool bool_t,
            Endianness endianness)
    {
        (void) endianness;
        return serialize(bool_t);
    }

    /*!
     * @brief This function serializes a string.
     * @param string_t The pointer to the string that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline Cdr& serialize(
            char* string_t)
    {
        return serialize(static_cast<const char*>(string_t));
    }

    /*!
     * @brief This function serializes a string.
     * @param string_t The pointer to the string that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const char* string_t);

    /*!
     * @brief This function serializes a wstring.
     * @param string_t The pointer to the wstring that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const wchar_t* string_t);

    /*!
     * @brief This function serializes a string with a different endianness.
     * @param string_t The pointer to the string that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const char* string_t,
            Endianness endianness);

    /*!
     * @brief This function serializes a wstring with a different endianness.
     * @param string_t The pointer to the wstring that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serialize(
            const wchar_t* string_t,
            Endianness endianness);

    /*!
     * @brief This function serializes a std::string.
     * @param string_t The string that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const std::string& string_t)
    {
        return serialize(string_t.c_str());
    }

    /*!
     * @brief This function serializes a std::wstring.
     * @param string_t The wstring that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const std::wstring& string_t)
    {
        return serialize(string_t.c_str());
    }

    /*!
     * @brief This function serializes a std::string with a different endianness.
     * @param string_t The string that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serialize(
            const std::string& string_t,
            Endianness endianness)
    {
        return serialize(string_t.c_str(), endianness);
    }

#if HAVE_CXX0X
    /*!
     * @brief This function template serializes an array.
     * @param array_t The array that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T, size_t _Size>
    inline Cdr& serialize(
            const std::array<_T, _Size>& array_t)
    {
        return serializeArray(array_t.data(), array_t.size());
    }

    /*!
     * @brief This function template serializes an array with a different endianness.
     * @param array_t The array that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T, size_t _Size>
    inline Cdr& serialize(
            const std::array<_T, _Size>& array_t,
            Endianness endianness)
    {
        return serializeArray(array_t.data(), array_t.size(), endianness);
    }

#endif // if HAVE_CXX0X

#if !defined(_MSC_VER) && HAVE_CXX0X
    /*!
     * @brief This function template serializes a sequence of booleans.
     * @param vector_t The sequence that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T = bool>
    Cdr& serialize(
            const std::vector<bool>& vector_t)
    {
        return serializeBoolSequence(vector_t);
    }

#endif // if !defined(_MSC_VER) && HAVE_CXX0X

    /*!
     * @brief This function template serializes a sequence.
     * @param vector_t The sequence that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& serialize(
            const std::vector<_T>& vector_t)
    {
        state state_before_error(*this);

        *this << static_cast<int32_t>(vector_t.size());

        try
        {
            return serializeArray(vector_t.data(), vector_t.size());
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            setState(state_before_error);
            ex.raise();
        }

        return *this;
    }

    /*!
     * @brief This function template serializes a map.
     * @param map_t The map that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _K, class _T>
    Cdr& serialize(
            const std::map<_K, _T>& map_t)
    {
        state state_(*this);

        *this << static_cast<int32_t>(map_t.size());

        try
        {
            for (auto it_pair = map_t.begin(); it_pair != map_t.end(); ++it_pair)
            {
                *this << it_pair->first;
                *this << it_pair->second;
            }
            //return serializeArray(map_t.data(), map_t.size());
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            setState(state_);
            ex.raise();
        }

        return *this;
    }

#ifdef _MSC_VER
    /*!
     * @brief This function template serializes a sequence of booleans.
     * @param vector_t The sequence that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<>
    Cdr& serialize<bool>(
            const std::vector<bool>& vector_t)
    {
        return serializeBoolSequence(vector_t);
    }

#endif // ifdef _MSC_VER

    /*!
     * @brief This function template serializes a sequence with a different endianness.
     * @param vector_t The sequence that will be serialized in the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& serialize(
            const std::vector<_T>& vector_t,
            Endianness endianness)
    {
        bool auxSwap = m_swapBytes;
        m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

        try
        {
            serialize(vector_t);
            m_swapBytes = auxSwap;
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            m_swapBytes = auxSwap;
            ex.raise();
        }

        return *this;
    }

    /*!
     * @brief This function template serializes a non-basic object.
     * @param type_t The object that will be serialized in the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T>
    inline Cdr& serialize(
            const _T& type_t)
    {
        type_t.serialize(*this);
        return *this;
    }

    /*!
     * @brief This function serializes an array of octets.
     * @param octet_t The sequence of octets that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const uint8_t* octet_t,
            size_t numElements)
    {
        return serializeArray(reinterpret_cast<const char*>(octet_t), numElements);
    }

    /*!
     * @brief This function serializes an array of octets with a different endianness.
     * @param octet_t The array of octets that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const uint8_t* octet_t,
            size_t numElements,
            Endianness endianness)
    {
        (void) endianness;
        return serializeArray(reinterpret_cast<const char*>(octet_t), numElements);
    }

    /*!
     * @brief This function serializes an array of characters.
     * @param char_t The array of characters that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const char* char_t,
            size_t numElements);

    /*!
     * @brief This function serializes an array of characters with a different endianness.
     * @param char_t The array of characters that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const char* char_t,
            size_t numElements,
            Endianness endianness)
    {
        (void) endianness;
        return serializeArray(char_t, numElements);
    }

    /*!
     * @brief This function serializes an array of int8_t.
     * @param int8 The sequence of int8_t that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const int8_t* int8,
            size_t numElements)
    {
        return serializeArray(reinterpret_cast<const char*>(int8), numElements);
    }

    /*!
     * @brief This function serializes an array of int8_t with a different endianness.
     * @param int8 The array of int8_t that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const int8_t* int8,
            size_t numElements,
            Endianness endianness)
    {
        (void) endianness;
        return serializeArray(reinterpret_cast<const char*>(int8), numElements);
    }

    /*!
     * @brief This function serializes an array of unsigned shorts.
     * @param ushort_t The array of unsigned shorts that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const uint16_t* ushort_t,
            size_t numElements)
    {
        return serializeArray(reinterpret_cast<const int16_t*>(ushort_t), numElements);
    }

    /*!
     * @brief This function serializes an array of unsigned shorts with a different endianness.
     * @param ushort_t The array of unsigned shorts that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const uint16_t* ushort_t,
            size_t numElements,
            Endianness endianness)
    {
        return serializeArray(reinterpret_cast<const int16_t*>(ushort_t), numElements, endianness);
    }

    /*!
     * @brief This function serializes an array of shorts.
     * @param short_t The array of shorts that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const int16_t* short_t,
            size_t numElements);

    /*!
     * @brief This function serializes an array of shorts with a different endianness.
     * @param short_t The array of shorts that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const int16_t* short_t,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function serializes an array of unsigned longs.
     * @param ulong_t The array of unsigned longs that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const uint32_t* ulong_t,
            size_t numElements)
    {
        return serializeArray(reinterpret_cast<const int32_t*>(ulong_t), numElements);
    }

    /*!
     * @brief This function serializes an array of unsigned longs with a different endianness.
     * @param ulong_t The array of unsigned longs that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const uint32_t* ulong_t,
            size_t numElements,
            Endianness endianness)
    {
        return serializeArray(reinterpret_cast<const int32_t*>(ulong_t), numElements, endianness);
    }

    /*!
     * @brief This function serializes an array of longs.
     * @param long_t The array of longs that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const int32_t* long_t,
            size_t numElements);

    /*!
     * @brief This function serializes an array of longs with a different endianness.
     * @param long_t The array of longs that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const int32_t* long_t,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function serializes an array of wide-chars.
     * @param wchar The array of wide-chars that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const wchar_t* wchar,
            size_t numElements);

    /*!
     * @brief This function serializes an array of wide-chars with a different endianness.
     * @param wchar The array of longs that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const wchar_t* wchar,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function serializes an array of unsigned long longs.
     * @param ulonglong_t The array of unsigned long longs that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const uint64_t* ulonglong_t,
            size_t numElements)
    {
        return serializeArray(reinterpret_cast<const int64_t*>(ulonglong_t), numElements);
    }

    /*!
     * @brief This function serializes an array of unsigned long longs with a different endianness.
     * @param ulonglong_t The array of unsigned long longs that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const uint64_t* ulonglong_t,
            size_t numElements,
            Endianness endianness)
    {
        return serializeArray(reinterpret_cast<const int64_t*>(ulonglong_t), numElements, endianness);
    }

    /*!
     * @brief This function serializes an array of long longs.
     * @param longlong_t The array of long longs that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const int64_t* longlong_t,
            size_t numElements);

    /*!
     * @brief This function serializes an array of long longs with a different endianness.
     * @param longlong_t The array of long longs that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const int64_t* longlong_t,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function serializes an array of floats.
     * @param float_t The array of floats that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const float* float_t,
            size_t numElements);

    /*!
     * @brief This function serializes an array of floats with a different endianness.
     * @param float_t The array of floats that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const float* float_t,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function serializes an array of doubles.
     * @param double_t The array of doubles that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const double* double_t,
            size_t numElements);

    /*!
     * @brief This function serializes an array of doubles with a different endianness.
     * @param double_t The array of doubles that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const double* double_t,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function serializes an array of long doubles.
     * @param ldouble_t The array of long doubles that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const long double* ldouble_t,
            size_t numElements);

    /*!
     * @brief This function serializes an array of long doubles with a different endianness.
     * @param ldouble_t The array of long doubles that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const long double* ldouble_t,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function serializes an array of booleans.
     * @param bool_t The array of booleans that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    Cdr& serializeArray(
            const bool* bool_t,
            size_t numElements);

    /*!
     * @brief This function serializes an array of booleans with a different endianness.
     * @param bool_t The array of booleans that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const bool* bool_t,
            size_t numElements,
            Endianness endianness)
    {
        (void) endianness;
        return serializeArray(bool_t, numElements);
    }

    /*!
     * @brief This function serializes an array of strings.
     * @param string_t The array of strings that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const std::string* string_t,
            size_t numElements)
    {
        for (size_t count = 0; count < numElements; ++count)
        {
            serialize(string_t[count].c_str());
        }
        return *this;
    }

    /*!
     * @brief This function serializes an array of wide-strings.
     * @param string_t The array of wide-strings that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const std::wstring* string_t,
            size_t numElements)
    {
        for (size_t count = 0; count < numElements; ++count)
        {
            serialize(string_t[count].c_str());
        }
        return *this;
    }

    /*!
     * @brief This function serializes an array of strings with a different endianness.
     * @param string_t The array of strings that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const std::string* string_t,
            size_t numElements,
            Endianness endianness)
    {
        bool auxSwap = m_swapBytes;
        m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

        try
        {
            serializeArray(string_t, numElements);
            m_swapBytes = auxSwap;
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            m_swapBytes = auxSwap;
            ex.raise();
        }

        return *this;
    }

    /*!
     * @brief This function serializes an array of wide-strings with a different endianness.
     * @param string_t The array of wide-strings that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& serializeArray(
            const std::wstring* string_t,
            size_t numElements,
            Endianness endianness)
    {
        bool auxSwap = m_swapBytes;
        m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

        try
        {
            serializeArray(string_t, numElements);
            m_swapBytes = auxSwap;
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            m_swapBytes = auxSwap;
            ex.raise();
        }

        return *this;
    }

    /*!
     * @brief This function template serializes an array of sequences of objects.
     * @param vector_t The array of sequences of objects that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& serializeArray(
            const std::vector<_T>* vector_t,
            size_t numElements)
    {
        for (size_t count = 0; count < numElements; ++count)
        {
            serialize(vector_t[count]);
        }
        return *this;
    }

    /*!
     * @brief This function template serializes an array of non-basic objects.
     * @param type_t The array of objects that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& serializeArray(
            const _T* type_t,
            size_t numElements)
    {
        for (size_t count = 0; count < numElements; ++count)
        {
            type_t[count].serialize(*this);
        }
        return *this;
    }

    /*!
     * @brief This function template serializes an array of non-basic objects with a different endianness.
     * @param type_t The array of objects that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& serializeArray(
            const _T* type_t,
            size_t numElements,
            Endianness endianness)
    {
        bool auxSwap = m_swapBytes;
        m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

        try
        {
            serializeArray(type_t, numElements);
            m_swapBytes = auxSwap;
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            m_swapBytes = auxSwap;
            ex.raise();
        }

        return *this;
    }

    /*!
     * @brief This function template serializes a raw sequence.
     * @param sequence_t Pointer to the sequence that will be serialized in the buffer.
     * @param numElements The number of elements contained in the sequence.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& serializeSequence(
            const _T* sequence_t,
            size_t numElements)
    {
        state state_before_error(*this);

        serialize(static_cast<int32_t>(numElements));

        try
        {
            return serializeArray(sequence_t, numElements);
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            setState(state_before_error);
            ex.raise();
        }

        return *this;
    }

    /*!
     * @brief This function template serializes a raw sequence with a different endianness.
     * @param sequence_t Pointer to the sequence that will be serialized in the buffer.
     * @param numElements The number of elements contained in the sequence.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& serializeSequence(
            const _T* sequence_t,
            size_t numElements,
            Endianness endianness)
    {
        bool auxSwap = m_swapBytes;
        m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

        try
        {
            serializeSequence(sequence_t, numElements);
            m_swapBytes = auxSwap;
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            m_swapBytes = auxSwap;
            ex.raise();
        }

        return *this;
    }

    /*!
     * @brief This function deserializes an octet.
     * @param octet_t The variable that will store the octet read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            uint8_t& octet_t)
    {
        return deserialize(reinterpret_cast<char&>(octet_t));
    }

    /*!
     * @brief This function deserializes an octet with a different endianness.
     * @param octet_t The variable that will store the octet read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            uint8_t& octet_t,
            Endianness endianness)
    {
        return deserialize(reinterpret_cast<char&>(octet_t), endianness);
    }

    /*!
     * @brief This function deserializes a character.
     * @param char_t The variable that will store the character read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            char& char_t);

    /*!
     * @brief This function deserializes a character with a different endianness.
     * @param char_t The variable that will store the character read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            char& char_t,
            Endianness endianness)
    {
        (void) endianness;
        return deserialize(char_t);
    }

    /*!
     * @brief This function deserializes an int8_t.
     * @param int8 The variable that will store the int8_t read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            int8_t& int8)
    {
        return deserialize(reinterpret_cast<char&>(int8));
    }

    /*!
     * @brief This function deserializes an int8_t with a different endianness.
     * @param int8 The variable that will store the int8_t read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            int8_t& int8,
            Endianness endianness)
    {
        return deserialize(reinterpret_cast<char&>(int8), endianness);
    }

    /*!
     * @brief This function deserializes an unsigned short.
     * @param ushort_t The variable that will store the unsigned short read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            uint16_t& ushort_t)
    {
        return deserialize(reinterpret_cast<int16_t&>(ushort_t));
    }

    /*!
     * @brief This function deserializes an unsigned short with a different endianness.
     * @param ushort_t The variable that will store the unsigned short read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            uint16_t& ushort_t,
            Endianness endianness)
    {
        return deserialize(reinterpret_cast<int16_t&>(ushort_t), endianness);
    }

    /*!
     * @brief This function deserializes a short.
     * @param short_t The variable that will store the short read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            int16_t& short_t);

    /*!
     * @brief This function deserializes a short with a different endianness.
     * @param short_t The variable that will store the short read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            int16_t& short_t,
            Endianness endianness);

    /*!
     * @brief This function deserializes an unsigned long.
     * @param ulong_t The variable that will store the unsigned long read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            uint32_t& ulong_t)
    {
        return deserialize(reinterpret_cast<int32_t&>(ulong_t));
    }

    /*!
     * @brief This function deserializes an unsigned long with a different endianness.
     * @param ulong_t The variable that will store the unsigned long read from the buffer..
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            uint32_t& ulong_t,
            Endianness endianness)
    {
        return deserialize(reinterpret_cast<int32_t&>(ulong_t), endianness);
    }

    /*!
     * @brief This function deserializes a long.
     * @param long_t The variable that will store the long read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            int32_t& long_t);

    /*!
     * @brief This function deserializes a long with a different endianness.
     * @param long_t The variable that will store the long read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            int32_t& long_t,
            Endianness endianness);

    /*!
     * @brief This function deserializes a wide-char.
     * @param wchar The variable that will store the wide-char read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            wchar_t& wchar)
    {
        uint32_t ret;
        deserialize(ret);
        wchar = static_cast<wchar_t>(ret);
        return *this;
    }

    /*!
     * @brief This function deserializes a wide-char with a different endianness.
     * @param wchar The variable that will store the wide-char read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            wchar_t& wchar,
            Endianness endianness)
    {
        uint32_t ret;
        deserialize(ret, endianness);
        wchar = static_cast<wchar_t>(ret);
        return *this;
    }

    /*!
     * @brief This function deserializes an unsigned long long.
     * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            uint64_t& ulonglong_t)
    {
        return deserialize(reinterpret_cast<int64_t&>(ulonglong_t));
    }

    /*!
     * @brief This function deserializes an unsigned long long with a different endianness.
     * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            uint64_t& ulonglong_t,
            Endianness endianness)
    {
        return deserialize(reinterpret_cast<int64_t&>(ulonglong_t), endianness);
    }

    /*!
     * @brief This function deserializes a long long.
     * @param longlong_t The variable that will store the long long read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            int64_t& longlong_t);

    /*!
     * @brief This function deserializes a long long with a different endianness.
     * @param longlong_t The variable that will store the long long read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            int64_t& longlong_t,
            Endianness endianness);

    /*!
     * @brief This function deserializes a float.
     * @param float_t The variable that will store the float read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            float& float_t);

    /*!
     * @brief This function deserializes a float with a different endianness.
     * @param float_t The variable that will store the float read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            float& float_t,
            Endianness endianness);

    /*!
     * @brief This function deserializes a double.
     * @param double_t The variable that will store the double read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            double& double_t);

    /*!
     * @brief This function deserializes a double with a different endianness.
     * @param double_t The variable that will store the double read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            double& double_t,
            Endianness endianness);

    /*!
     * @brief This function deserializes a long double.
     * @param ldouble_t The variable that will store the long double read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     * @note Due to internal representation differences, WIN32 and *NIX like systems are not compatible.
     */
    Cdr& deserialize(
            long double& ldouble_t);

    /*!
     * @brief This function deserializes a long double with a different endianness.
     * @param ldouble_t The variable that will store the long double read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     * @note Due to internal representation differences, WIN32 and *NIX like systems are not compatible.
     */
    Cdr& deserialize(
            long double& ldouble_t,
            Endianness endianness);

    /*!
     * @brief This function deserializes a boolean.
     * @param bool_t The variable that will store the boolean read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     * @exception exception::BadParamException This exception is thrown when trying to deserialize an invalid value.
     */
    Cdr& deserialize(
            bool& bool_t);

    /*!
     * @brief This function deserializes a boolean with a different endianness.
     * @param bool_t The variable that will store the boolean read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     * @exception exception::BadParamException This exception is thrown when trying to deserialize an invalid value.
     */
    inline
    Cdr& deserialize(
            bool& bool_t,
            Endianness endianness)
    {
        (void) endianness;
        return deserialize(bool_t);
    }

    /*!
     * @brief This function deserializes a string.
     * This function allocates memory to store the string. The user pointer will be set to point this allocated memory.
     * The user will have to free this allocated memory using free()
     * @param string_t The pointer that will point to the string read from the buffer.
     * The user will have to free the allocated memory using free()
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            char*& string_t);

    /*!
     * @brief This function deserializes a wide string.
     * This function allocates memory to store the wide string. The user pointer will be set to point this allocated memory.
     * The user will have to free this allocated memory using free()
     * @param string_t The pointer that will point to the wide string read from the buffer.
     * The user will have to free the allocated memory using free()
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            wchar_t*& string_t);

    /*!
     * @brief This function deserializes a string with a different endianness.
     * This function allocates memory to store the string. The user pointer will be set to point this allocated memory.
     * The user will have to free this allocated memory using free()
     * @param string_t The pointer that will point to the string read from the buffer.
     * @param endianness Endianness that will be used in the deserialization of this value.
     * The user will have to free the allocated memory using free()
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            char*& string_t,
            Endianness endianness);

    /*!
     * @brief This function deserializes a wide string with a different endianness.
     * This function allocates memory to store the wide string. The user pointer will be set to point this allocated memory.
     * The user will have to free this allocated memory using free()
     * @param string_t The pointer that will point to the wide string read from the buffer.
     * @param endianness Endianness that will be used in the deserialization of this value.
     * The user will have to free the allocated memory using free()
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserialize(
            wchar_t*& string_t,
            Endianness endianness);

    /*!
     * @brief This function deserializes a std::string.
     * @param string_t The variable that will store the string read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            std::string& string_t)
    {
        uint32_t length = 0;
        const char* str = readString(length);
        string_t = std::string(str, length);
        return *this;
    }

    /*!
     * @brief This function deserializes a std::string.
     * @param string_t The variable that will store the string read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            std::wstring& string_t)
    {
        uint32_t length = 0;
        string_t = readWString(length);
        return *this;
    }

    /*!
     * @brief This function deserializes a string with a different endianness.
     * @param string_t The variable that will store the string read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            std::string& string_t,
            Endianness endianness)
    {
        bool auxSwap = m_swapBytes;
        m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

        try
        {
            deserialize(string_t);
            m_swapBytes = auxSwap;
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            m_swapBytes = auxSwap;
            ex.raise();
        }

        return *this;
    }

    /*!
     * @brief This function deserializes a string with a different endianness.
     * @param string_t The variable that will store the string read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserialize(
            std::wstring& string_t,
            Endianness endianness)
    {
        bool auxSwap = m_swapBytes;
        m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

        try
        {
            deserialize(string_t);
            m_swapBytes = auxSwap;
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            m_swapBytes = auxSwap;
            ex.raise();
        }

        return *this;
    }

#if HAVE_CXX0X
    /*!
     * @brief This function template deserializes an array.
     * @param array_t The variable that will store the array read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T, size_t _Size>
    inline Cdr& deserialize(
            std::array<_T, _Size>& array_t)
    {
        return deserializeArray(array_t.data(), array_t.size());
    }

    /*!
     * @brief This function template deserializes an array with a different endianness.
     * @param array_t The variable that will store the array read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T, size_t _Size>
    inline Cdr& deserialize(
            std::array<_T, _Size>& array_t,
            Endianness endianness)
    {
        return deserializeArray(array_t.data(), array_t.size(), endianness);
    }

#endif // if HAVE_CXX0X

#if !defined(_MSC_VER) && HAVE_CXX0X
    /*!
     * @brief This function template deserializes a sequence.
     * @param vector_t The variable that will store the sequence read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T = bool>
    Cdr& deserialize(
            std::vector<bool>& vector_t)
    {
        return deserializeBoolSequence(vector_t);
    }

#endif // if !defined(_MSC_VER) && HAVE_CXX0X

    /*!
     * @brief This function template deserializes a sequence.
     * @param vector_t The variable that will store the sequence read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& deserialize(
            std::vector<_T>& vector_t)
    {
        uint32_t seqLength = 0;
        state state_before_error(*this);

        *this >> seqLength;

        if (seqLength == 0)
        {
            vector_t.clear();
            return *this;
        }

        if ((m_lastPosition - m_currentPosition) < seqLength)
        {
            setState(state_before_error);
            throw eprosima::fastcdr::exception::NotEnoughMemoryException(
                      eprosima::fastcdr::exception::NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
        }

        try
        {
            vector_t.resize(seqLength);
            return deserializeArray(vector_t.data(), vector_t.size());
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            setState(state_before_error);
            ex.raise();
        }

        return *this;
    }

    /*!
     * @brief This function template deserializes a map.
     * @param map_t The variable that will store the map read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _K, class _T>
    Cdr& deserialize(
            std::map<_K, _T>& map_t)
    {
        uint32_t seqLength = 0;
        state state_(*this);

        *this >> seqLength;

        try
        {
            for (uint32_t i = 0; i < seqLength; ++i)
            {
                _K key;
                _T value;
                *this >> key;
                *this >> value;
                map_t.emplace(std::pair<_K, _T>(std::move(key), std::move(value)));
            }
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            setState(state_);
            ex.raise();
        }

        return *this;
    }

#ifdef _MSC_VER
    /*!
     * @brief This function template deserializes a sequence.
     * @param vector_t The variable that will store the sequence read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<>
    Cdr& deserialize<bool>(
            std::vector<bool>& vector_t)
    {
        return deserializeBoolSequence(vector_t);
    }

#endif // ifdef _MSC_VER

    /*!
     * @brief This function template deserializes a sequence with a different endianness.
     * @param vector_t The variable that will store the sequence read from the buffer.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& deserialize(
            std::vector<_T>& vector_t,
            Endianness endianness)
    {
        bool auxSwap = m_swapBytes;
        m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

        try
        {
            deserialize(vector_t);
            m_swapBytes = auxSwap;
        }
        catch (exception::Exception& ex)
        {
            m_swapBytes = auxSwap;
            ex.raise();
        }

        return *this;
    }

    /*!
     * @brief This function template deserializes a non-basic object.
     * @param type_t The variable that will store the object read from the buffer.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T>
    inline Cdr& deserialize(
            _T& type_t)
    {
        type_t.deserialize(*this);
        return *this;
    }

    /*!
     * @brief This function deserializes an array of octets.
     * @param octet_t The variable that will store the array of octets read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            uint8_t* octet_t,
            size_t numElements)
    {
        return deserializeArray(reinterpret_cast<char*>(octet_t), numElements);
    }

    /*!
     * @brief This function deserializes an array of octets with a different endianness.
     * @param octet_t The variable that will store the array of octets read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            uint8_t* octet_t,
            size_t numElements,
            Endianness endianness)
    {
        return deserializeArray(reinterpret_cast<char*>(octet_t), numElements, endianness);
    }

    /*!
     * @brief This function deserializes an array of characters.
     * @param char_t The variable that will store the array of characters read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            char* char_t,
            size_t numElements);

    /*!
     * @brief This function deserializes an array of characters with a different endianness.
     * @param char_t The variable that will store the array of characters read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            char* char_t,
            size_t numElements,
            Endianness endianness)
    {
        (void) endianness;
        return deserializeArray(char_t, numElements);
    }

    /*!
     * @brief This function deserializes an array of int8_t.
     * @param int8 The variable that will store the array of int8_t read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            int8_t* int8,
            size_t numElements)
    {
        return deserializeArray(reinterpret_cast<char*>(int8), numElements);
    }

    /*!
     * @brief This function deserializes an array of int8_t with a different endianness.
     * @param int8 The variable that will store the array of int8_t read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            int8_t* int8,
            size_t numElements,
            Endianness endianness)
    {
        return deserializeArray(reinterpret_cast<char*>(int8), numElements, endianness);
    }

    /*!
     * @brief This function deserializes an array of unsigned shorts.
     * @param ushort_t The variable that will store the array of unsigned shorts read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            uint16_t* ushort_t,
            size_t numElements)
    {
        return deserializeArray(reinterpret_cast<int16_t*>(ushort_t), numElements);
    }

    /*!
     * @brief This function deserializes an array of unsigned shorts with a different endianness.
     * @param ushort_t The variable that will store the array of unsigned shorts read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            uint16_t* ushort_t,
            size_t numElements,
            Endianness endianness)
    {
        return deserializeArray(reinterpret_cast<int16_t*>(ushort_t), numElements, endianness);
    }

    /*!
     * @brief This function deserializes an array of shorts.
     * @param short_t The variable that will store the array of shorts read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            int16_t* short_t,
            size_t numElements);

    /*!
     * @brief This function deserializes an array of shorts with a different endianness.
     * @param short_t The variable that will store the array of shorts read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            int16_t* short_t,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function deserializes an array of unsigned longs.
     * @param ulong_t The variable that will store the array of unsigned longs read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            uint32_t* ulong_t,
            size_t numElements)
    {
        return deserializeArray(reinterpret_cast<int32_t*>(ulong_t), numElements);
    }

    /*!
     * @brief This function deserializes an array of unsigned longs with a different endianness.
     * @param ulong_t The variable that will store the array of unsigned longs read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            uint32_t* ulong_t,
            size_t numElements,
            Endianness endianness)
    {
        return deserializeArray(reinterpret_cast<int32_t*>(ulong_t), numElements, endianness);
    }

    /*!
     * @brief This function deserializes an array of longs.
     * @param long_t The variable that will store the array of longs read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            int32_t* long_t,
            size_t numElements);

    /*!
     * @brief This function deserializes an array of longs with a different endianness.
     * @param long_t The variable that will store the array of longs read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            int32_t* long_t,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function deserializes an array of wide-chars.
     * @param wchar The variable that will store the array of wide-chars read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            wchar_t* wchar,
            size_t numElements);

    /*!
     * @brief This function deserializes an array of wide-chars with a different endianness.
     * @param wchar The variable that will store the array of wide-chars read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            wchar_t* wchar,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function deserializes an array of unsigned long longs.
     * @param ulonglong_t The variable that will store the array of unsigned long longs read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            uint64_t* ulonglong_t,
            size_t numElements)
    {
        return deserializeArray(reinterpret_cast<int64_t*>(ulonglong_t), numElements);
    }

    /*!
     * @brief This function deserializes an array of unsigned long longs with a different endianness.
     * @param ulonglong_t The variable that will store the array of unsigned long longs read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            uint64_t* ulonglong_t,
            size_t numElements,
            Endianness endianness)
    {
        return deserializeArray(reinterpret_cast<int64_t*>(ulonglong_t), numElements, endianness);
    }

    /*!
     * @brief This function deserializes an array of long longs.
     * @param longlong_t The variable that will store the array of long longs read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            int64_t* longlong_t,
            size_t numElements);

    /*!
     * @brief This function deserializes an array of long longs with a different endianness.
     * @param longlong_t The variable that will store the array of long longs read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            int64_t* longlong_t,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function deserializes an array of floats.
     * @param float_t The variable that will store the array of floats read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            float* float_t,
            size_t numElements);

    /*!
     * @brief This function deserializes an array of floats with a different endianness.
     * @param float_t The variable that will store the array of floats read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            float* float_t,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function deserializes an array of doubles.
     * @param double_t The variable that will store the array of doubles read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            double* double_t,
            size_t numElements);

    /*!
     * @brief This function deserializes an array of doubles with a different endianness.
     * @param double_t The variable that will store the array of doubles read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            double* double_t,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function deserializes an array of long doubles.
     * @param ldouble_t The variable that will store the array of long doubles read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            long double* ldouble_t,
            size_t numElements);

    /*!
     * @brief This function deserializes an array of long doubles with a different endianness.
     * @param ldouble_t The variable that will store the array of long doubles read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            long double* ldouble_t,
            size_t numElements,
            Endianness endianness);

    /*!
     * @brief This function deserializes an array of booleans.
     * @param bool_t The variable that will store the array of booleans read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    Cdr& deserializeArray(
            bool* bool_t,
            size_t numElements);

    /*!
     * @brief This function deserializes an array of booleans with a different endianness.
     * @param bool_t The variable that will store the array of booleans read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            bool* bool_t,
            size_t numElements,
            Endianness endianness)
    {
        (void) endianness;
        return deserializeArray(bool_t, numElements);
    }

    /*!
     * @brief This function deserializes an array of strings.
     * @param string_t The variable that will store the array of strings read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            std::string* string_t,
            size_t numElements)
    {
        for (size_t count = 0; count < numElements; ++count)
        {
            deserialize(string_t[count]);
        }
        return *this;
    }

    /*!
     * @brief This function deserializes an array of wide-strings.
     * @param string_t The variable that will store the array of wide-strings read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            std::wstring* string_t,
            size_t numElements)
    {
        for (size_t count = 0; count < numElements; ++count)
        {
            deserialize(string_t[count]);
        }
        return *this;
    }

    /*!
     * @brief This function deserializes an array of strings with a different endianness.
     * @param string_t The variable that will store the array of strings read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            std::string* string_t,
            size_t numElements,
            Endianness endianness)
    {
        bool auxSwap = m_swapBytes;
        m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

        try
        {
            deserializeArray(string_t, numElements);
            m_swapBytes = auxSwap;
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            m_swapBytes = auxSwap;
            ex.raise();
        }

        return *this;
    }

    /*!
     * @brief This function deserializes an array of wide-strings with a different endianness.
     * @param string_t The variable that will store the array of wide-strings read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    inline
    Cdr& deserializeArray(
            std::wstring* string_t,
            size_t numElements,
            Endianness endianness)
    {
        bool auxSwap = m_swapBytes;
        m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

        try
        {
            deserializeArray(string_t, numElements);
            m_swapBytes = auxSwap;
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            m_swapBytes = auxSwap;
            ex.raise();
        }

        return *this;
    }

    /*!
     * @brief This function deserializes an array of sequences of objects.
     * @param vector_t The variable that will store the array of sequences of objects read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& deserializeArray(
            std::vector<_T>* vector_t,
            size_t numElements)
    {
        for (size_t count = 0; count < numElements; ++count)
        {
            deserialize(vector_t[count]);
        }
        return *this;
    }

    /*!
     * @brief This function template deserializes an array of non-basic objects.
     * @param type_t The variable that will store the array of objects read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& deserializeArray(
            _T* type_t,
            size_t numElements)
    {
        for (size_t count = 0; count < numElements; ++count)
        {
            type_t[count].deserialize(*this);
        }
        return *this;
    }

    /*!
     * @brief This function template deserializes an array of non-basic objects with a different endianness.
     * @param type_t The variable that will store the array of objects read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& deserializeArray(
            _T* type_t,
            size_t numElements,
            Endianness endianness)
    {
        bool auxSwap = m_swapBytes;
        m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

        try
        {
            deserializeArray(type_t, numElements);
            m_swapBytes = auxSwap;
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            m_swapBytes = auxSwap;
            ex.raise();
        }

        return *this;
    }

#if !defined(_MSC_VER) && HAVE_CXX0X
    /*!
     * @brief This function template deserializes a string sequence.
     * This function allocates memory to store the sequence. The user pointer will be set to point this allocated memory.
     * The user will have to free this allocated memory using free()
     * @param sequence_t The pointer that will store the sequence read from the buffer.
     * @param numElements This variable return the number of elements of the sequence.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T = std::string>
    Cdr& deserializeSequence(
            std::string*& sequence_t,
            size_t& numElements)
    {
        return deserializeStringSequence(sequence_t, numElements);
    }

    /*!
     * @brief This function template deserializes a wide-string sequence.
     * This function allocates memory to store the sequence. The user pointer will be set to point this allocated memory.
     * The user will have to free this allocated memory using free()
     * @param sequence_t The pointer that will store the sequence read from the buffer.
     * @param numElements This variable return the number of elements of the sequence.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T = std::wstring>
    Cdr& deserializeSequence(
            std::wstring*& sequence_t,
            size_t& numElements)
    {
        return deserializeWStringSequence(sequence_t, numElements);
    }

#endif // if !defined(_MSC_VER) && HAVE_CXX0X

    /*!
     * @brief This function template deserializes a raw sequence.
     * This function allocates memory to store the sequence. The user pointer will be set to point this allocated memory.
     * The user will have to free this allocated memory using free()
     * @param sequence_t The pointer that will store the sequence read from the buffer.
     * @param numElements This variable return the number of elements of the sequence.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& deserializeSequence(
            _T*& sequence_t,
            size_t& numElements)
    {
        uint32_t seqLength = 0;
        state state_before_error(*this);

        deserialize(seqLength);

        try
        {
            sequence_t = reinterpret_cast<_T*>(calloc(seqLength, sizeof(_T)));
            deserializeArray(sequence_t, seqLength);
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            free(sequence_t);
            sequence_t = NULL;
            setState(state_before_error);
            ex.raise();
        }

        numElements = seqLength;
        return *this;
    }

#ifdef _MSC_VER
    /*!
     * @brief This function template deserializes a string sequence.
     * This function allocates memory to store the sequence. The user pointer will be set to point this allocated memory.
     * The user will have to free this allocated memory using free()
     * @param sequence_t The pointer that will store the sequence read from the buffer.
     * @param numElements This variable return the number of elements of the sequence.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<>
    Cdr& deserializeSequence<std::string>(
            std::string*& sequence_t,
            size_t& numElements)
    {
        return deserializeStringSequence(sequence_t, numElements);
    }

    /*!
     * @brief This function template deserializes a wide-string sequence.
     * This function allocates memory to store the sequence. The user pointer will be set to point this allocated memory.
     * The user will have to free this allocated memory using free()
     * @param sequence_t The pointer that will store the sequence read from the buffer.
     * @param numElements This variable return the number of elements of the sequence.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<>
    Cdr& deserializeSequence<std::wstring>(
            std::wstring*& sequence_t,
            size_t& numElements)
    {
        return deserializeWStringSequence(sequence_t, numElements);
    }

#endif // ifdef _MSC_VER

    /*!
     * @brief This function template deserializes a raw sequence with a different endianness.
     * This function allocates memory to store the sequence. The user pointer will be set to point this allocated memory.
     * The user will have to free this allocated memory using free()
     * @param sequence_t The pointer that will store the sequence read from the buffer.
     * @param numElements This variable return the number of elements of the sequence.
     * @param endianness Endianness that will be used in the deserialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T>
    Cdr& deserializeSequence(
            _T*& sequence_t,
            size_t& numElements,
            Endianness endianness)
    {
        bool auxSwap = m_swapBytes;
        m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

        try
        {
            deserializeSequence(sequence_t, numElements);
            m_swapBytes = auxSwap;
        }
        catch (eprosima::fastcdr::exception::Exception& ex)
        {
            m_swapBytes = auxSwap;
            ex.raise();
        }

        return *this;
    }

private:

    Cdr(
            const Cdr&) = delete;

    Cdr& operator =(
            const Cdr&) = delete;

    Cdr& serializeBoolSequence(
            const std::vector<bool>& vector_t);

    Cdr& deserializeBoolSequence(
            std::vector<bool>& vector_t);

    Cdr& deserializeStringSequence(
            std::string*& sequence_t,
            size_t& numElements);

    Cdr& deserializeWStringSequence(
            std::wstring*& sequence_t,
            size_t& numElements);

#if HAVE_CXX0X
    /*!
     * @brief This function template detects the content type of the STD container array and serializes the array.
     * @param array_t The array that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T, size_t _Size>
    Cdr& serializeArray(
            const std::array<_T, _Size>* array_t,
            size_t numElements)
    {
        return serializeArray(array_t->data(), numElements * array_t->size());
    }

    /*!
     * @brief This function template detects the content type of the STD container array and serializes the array with a different endianness.
     * @param array_t The array that will be serialized in the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to serialize a position that exceeds the internal memory size.
     */
    template<class _T, size_t _Size>
    Cdr& serializeArray(
            const std::array<_T, _Size>* array_t,
            size_t numElements,
            Endianness endianness)
    {
        return serializeArray(array_t->data(), numElements * array_t->size(), endianness);
    }

    /*!
     * @brief This function template detects the content type of the STD container array and deserializes the array.
     * @param array_t The variable that will store the array read from the buffer.
     * @param numElements Number of the elements in the array.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T, size_t _Size>
    Cdr& deserializeArray(
            std::array<_T, _Size>* array_t,
            size_t numElements)
    {
        return deserializeArray(array_t->data(), numElements * array_t->size());
    }

    /*!
     * @brief This function template detects the content type of STD container array and deserializes the array with a different endianness.
     * @param array_t The variable that will store the array read from the buffer.
     * @param numElements Number of the elements in the array.
     * @param endianness Endianness that will be used in the serialization of this value.
     * @return Reference to the eprosima::fastcdr::Cdr object.
     * @exception exception::NotEnoughMemoryException This exception is thrown when trying to deserialize a position that exceeds the internal memory size.
     */
    template<class _T, size_t _Size>
    Cdr& deserializeArray(
            std::array<_T, _Size>* array_t,
            size_t numElements,
            Endianness endianness)
    {
        return deserializeArray(array_t->data(), numElements * array_t->size(), endianness);
    }

#endif // if HAVE_CXX0X

    /*!
     * @brief This function returns the extra bytes regarding the allignment.
     * @param dataSize The size of the data that will be serialized.
     * @return The size needed for the aligment.
     */
    inline size_t alignment(
            size_t dataSize) const
    {
        return dataSize >
               m_lastDataSize ? (dataSize - ((m_currentPosition - m_alignPosition) % dataSize)) &
               (dataSize - 1) : 0;
    }

    /*!
     * @brief This function jumps the number of bytes of the alignment. These bytes should be calculated with the function eprosima::fastcdr::Cdr::alignment.
     * @param align The number of bytes to be skipped.
     */
    inline void makeAlign(
            size_t align)
    {
        m_currentPosition += align;
    }

    /*!
     * @brief This function resizes the internal buffer. It only applies if the FastBuffer object was created with the default constructor.
     * @param minSizeInc Minimun size increase for the internal buffer
     * @return True if the resize was succesful, false if it was not
     */
    bool resize(
            size_t minSizeInc);

    //TODO
    const char* readString(
            uint32_t& length);
    const std::wstring readWString(
            uint32_t& length);

    //! @brief Reference to the buffer that will be serialized/deserialized.
    FastBuffer& m_cdrBuffer;

    //! @brief The type of CDR that will be use in serialization/deserialization.
    CdrType m_cdrType;

    //! @brief Using DDS_CDR type, this attribute stores if the stream buffer contains a parameter list or not.
    DDSCdrPlFlag m_plFlag;

    //! @brief This attribute stores the option flags when the CDR type is DDS_CDR;
    uint16_t m_options;

    //! @brief The endianness that will be applied over the buffer.
    uint8_t m_endianness;

    //! @brief This attribute specifies if it is needed to swap the bytes.
    bool m_swapBytes;

    //! @brief Stores the last datasize serialized/deserialized. It's used to optimize.
    size_t m_lastDataSize;

    //! @brief The current position in the serialization/deserialization process.
    FastBuffer::iterator m_currentPosition;

    //! @brief The position from where the aligment is calculated.
    FastBuffer::iterator m_alignPosition;

    //! @brief The last position in the buffer;
    FastBuffer::iterator m_lastPosition;
};
}     //namespace fastcdr
} //namespace eprosima

#endif // _CDR_CDR_H_
