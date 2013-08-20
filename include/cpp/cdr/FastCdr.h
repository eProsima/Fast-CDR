#ifndef _CPP_CDR_FASTCDR_H_
#define _CPP_CDR_FASTCDR_H_

#include "cpp/cdr/Cdr_dll.h"
#include "cpp/Marshalling.h"
#include "cpp/storage/Storage.h"
#include "cpp/exceptions/Exception.h"
#include "cpp/exceptions/NotEnoughMemoryException.h"

namespace eProsima
{
    namespace marshalling
    {
        /*!
         * @brief This class offers an interface to serialize/deserialize some basic types using a modified CDR protocol inside a eProsima::storage::Storage.
         * This modified CDR protocol provides a serialization mechanism more faster than common CDR protocol, because it doesn't use aligment.
         * @ingroup CDRAPIREFERENCE
         */
        class Cdr_DllAPI FastCdr : public Marshalling
        {
            public:

                /*!
                 * @brief This class stores the current state of a CDR serialization.
                 */
                class Cdr_DllAPI state
                {
                    friend class FastCdr;
                    public:

                    /*!
                     * @brief Default constructor.
                     */
                    state(FastCdr &fastcdr);

                    private:

                    //! @brief The position in the storage when the state was created.
                    storage::Storage::iterator &m_currentPosition;
                };
                /*!
                 * @brief This constructor creates a eProsima::marshalling::FastCdr object that could serialize/deserialize
                 * the assigned storage.
                 *
                 * @param storage A reference to the storage that contains or will contain the CDR representation.
                 */
                FastCdr(storage::Storage &storage);

                /*!
                 * @brief This function skips a number of bytes in the CDR stream.
                 * @numBytes The number of bytes that will be jumped.
                 * @return True value is returned when the jump operation works successfully. In other case false value is returned.
                 */
                bool jump(uint32_t numBytes);

                /*!
                 * @brief This function resets the current position in the CDR stream to the begining.
                 */
                void reset();

                /*!
                 * @brief This function returns the current position in the CDR stream.
                 * @return Pointer to the current position in the storage.
                 */
                char* getCurrentPosition();

                /*!
                 * @brief This function returns the length of the serialized data inside the CDR stream.
                 * @return The length of the serialized data.
                 */
                inline size_t getSerializedDataLength() const;

                /*!
                 * @brief This function returns the current state of the CDR stream.
                 * @return The current state of the storage.
                 */
                FastCdr::state getState();

                /*!
                 * @brief This function sets a previous state of the CDR stream;
                 * @param state Previous state that will be set again.
                 */
                void setState(FastCdr::state &state);

                /*!
                 * @brief This operator serializes an octet.
                 * @param octet_t The value of the octet that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const uint8_t octet_t){return serialize(octet_t);}

                /*!
                 * @brief This operator serializes a char.
                 * @param char_t The value of the character that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const char char_t){return serialize(char_t);}

                /*!
                 * @brief This operator serializes a unsigned short.
                 * @param ushort_t The value of the unsigned short that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const uint16_t ushort_t){return serialize(ushort_t);}

                /*!
                 * @brief This operator serializes a short.
                 * @param short_t The value of the short that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const int16_t short_t){return serialize(short_t);}

                /*!
                 * @brief This operator serializes a unsigned long.
                 * @param ulong_t The value of the unsigned long that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const uint32_t ulong_t){return serialize(ulong_t);}

                /*!
                 * @brief This operator serializes a long.
                 * @param ulong_t The value of the unsigned long that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const int32_t long_t){return serialize(long_t);}

                /*!
                 * @brief This operator serializes a unsigned long long.
                 * @param ulonglong_t The value of the unsigned long long that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const uint64_t ulonglong_t){return serialize(ulonglong_t);}

                /*!
                 * @brief This operator serializes a long long.
                 * @param longlong_t The value of the long long that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const int64_t longlong_t){return serialize(longlong_t);}

                /*!
                 * @brief This operator serializes a float.
                 * @param float_t The value of the float that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const float float_t){return serialize(float_t);}

                /*!
                 * @brief This operator serializes a double.
                 * @param double_t The value of the double that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const double double_t){return serialize(double_t);}

                /*!
                 * @brief This operator serializes a boolean.
                 * @param bool_t The value of the boolean that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const bool bool_t){return serialize(bool_t);}

                /*!
                 * @brief This operator serializes a string.
                 * @param string_t The string that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const std::string &string_t){return serialize(string_t);}

                /*!
                 * @brief This operator template is used to serialize arrays.
                 * @param array_t The array that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::FastCdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    inline FastCdr& operator<<(const std::array<_T, _Size> &array_t){return serialize<_T, _Size>(array_t);}

                /*!
                 * @brief This operator is used to serialize sequences of octets.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const std::vector<uint8_t> &vector_t){return serialize(vector_t);}

                /*!
                 * @brief This operator is used to serialize sequences of chars.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const std::vector<char> &vector_t){return serialize(vector_t);}

                /*!
                 * @brief This operator is used to serialize sequences of unsigned shorts.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const std::vector<uint16_t> &vector_t){return serialize(vector_t);}

                /*!
                 * @brief This operator is used to serialize sequences of shorts.
                 * @param vector_t The sequence that will be serialized in the storage mechanism.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const std::vector<int16_t> &vector_t){return serialize(vector_t);}

                /*!
                 * @brief This operator is used to serialize sequences of unsigned long.
                 * @param vector_t The sequence that will be serialized in the storage mechanism.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const std::vector<uint32_t> &vector_t){return serialize(vector_t);}

                /*!
                 * @brief This operator is used to serialize sequences of longs.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const std::vector<int32_t> &vector_t){return serialize(vector_t);}

                /*!
                 * @brief This operator is used to serialize sequences of unsigned long longs.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const std::vector<uint64_t> &vector_t){return serialize(vector_t);}

                /*!
                 * @brief This operator is used to serialize sequences of long longs.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const std::vector<int64_t> &vector_t){return serialize(vector_t);}

                /*!
                 * @brief This operator is used to serialize sequences of floats.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const std::vector<float> &vector_t){return serialize(vector_t);}

                /*!
                 * @brief This operator is used to serialize sequences of doubles.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const std::vector<double> &vector_t){return serialize(vector_t);}

                /*!
                 * @brief This operator deserializes an octet.
                 * @param octet_t The variable that will store the octet read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(uint8_t &octet_t){return deserialize(octet_t);}

                /*!
                 * @brief This operator deserializes a char.
                 * @param char_t The variable that will store the character read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(char &char_t){return deserialize(char_t);}

                /*!
                 * @brief This operator deserializes a unsigned short.
                 * @param ushort_t The variable that will store the unsigned short read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(uint16_t &ushort_t){return deserialize(ushort_t);}

                /*!
                 * @brief This operator deserializes a short.
                 * @param short_t The variable that will store the short read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(int16_t &short_t){return deserialize(short_t);}

                /*!
                 * @brief This operator deserializes a unsigned long.
                 * @param ulong_t The variable that will store the unsigned long read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(uint32_t &ulong_t){return deserialize(ulong_t);}

                /*!
                 * @brief This operator deserializes a long.
                 * @param long_t The variable that will store the long read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(int32_t &long_t){return deserialize(long_t);}

                /*!
                 * @brief This operator deserializes a unsigned long long.
                 * @param ulonglong_t The variable that will store the unsigned long long read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(uint64_t &ulonglong_t){return deserialize(ulonglong_t);}

                /*!
                 * @brief This operator deserializes a long long.
                 * @param longlong_t The variable that will store the long long read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(int64_t &longlong_t){return deserialize(longlong_t);}

                /*!
                 * @brief This operator deserializes a float.
                 * @param float_t The variable that will store the float read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(float &float_t){return deserialize(float_t);}

                /*!
                 * @brief This operator deserializes a double.
                 * @param double_t The variable that will store the double read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(double &double_t){return deserialize(double_t);}

                /*!
                 * @brief This operator deserializes a boolean.
                 * @param bool_t The variable that will store the boolean read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
                 */
                inline Marshalling& operator>>(bool &bool_t){return deserialize(bool_t);}

                /*!
                 * @brief This operator deserializes a string.
                 * @param string_t The variable that will store the string read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::string &string_t){return deserialize(string_t);}

                /*!
                 * @brief This operator template is used to deserialize arrays.
                 * @param array_t The variable that will store the array read from the storage.
                 * @return Reference to the eProsima::marshalling::FastCdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    inline FastCdr& operator>>(std::array<_T, _Size> &array_t){return deserialize<_T, _Size>(array_t);}

                /*!
                 * @brief This operator is used to deserialize sequences of octets.
                 * @param vector_t The variable that will store the sequence of octets read from the storage.
                 * @return Reference to the eProsima::marshalling::FastCdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::vector<uint8_t> &vector_t){return deserialize(vector_t);}

                /*!
                 * @brief This operator is used to deserialize sequences of chars.
                 * @param vector_t The variable that will store the sequence of chars read from the storage.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::vector<char> &vector_t){return deserialize(vector_t);}

                /*!
                 * @brief This operator is used to deserialize sequences of unsigned shorts.
                 * @param vector_t The variable that will store the sequence of unsigned shorts read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::vector<uint16_t> &vector_t){return deserialize(vector_t);}

                /*!
                 * @brief This operator is used to deserialize sequences of shorts.
                 * @param vector_t The variable that will store the sequence of shorts read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::vector<int16_t> &vector_t){return deserialize(vector_t);}

                /*!
                 * @brief This operator is used to deserialize sequences of unsigned longs.
                 * @param vector_t The variable that will store the sequence of unsigned longs read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::vector<uint32_t> &vector_t){return deserialize(vector_t);}

                /*!
                 * @brief This operator is used to deserialize sequences of longs.
                 * @param vector_t The variable that will store the sequence of longs read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::vector<int32_t> &vector_t){return deserialize(vector_t);}

                /*!
                 * @brief This operator is used to deserialize sequences of unsigned long longs.
                 * @param vector_t The variable that will store the sequence of unsigned long longs read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::vector<uint64_t> &vector_t){return deserialize(vector_t);}

                /*!
                 * @brief This operator is used to deserialize sequences of long longs.
                 * @param vector_t The variable that will store the sequence of long longs read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::vector<int64_t> &vector_t){return deserialize(vector_t);}

                /*!
                 * @brief This operator is used to deserialize sequences of floats.
                 * @param vector_t The variable that will store the sequence of floats read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::vector<float> &vector_t){return deserialize(vector_t);}

                /*!
                 * @brief This operator is used to deserialize sequences of doubles.
                 * @param vector_t The variable that will store the sequence of doubles read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::vector<double> &vector_t){return deserialize(vector_t);}

                /*!
                 * @brief This function serializes an octet.
                 * @param octet_t The value of the octet that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const uint8_t octet_t)
                    {
                        return serialize((char)octet_t);
                    }

                /*!
                 * @brief This function serializes a char.
                 * @param char_t The value of the character that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const char char_t)
                    {
                        if(((m_lastPosition - m_currentPosition) >= sizeof(char_t)) || resize(sizeof(char_t)))
                        {
                            m_storage.insert(m_currentPosition, char_t);
                            return *this;
                        }

                        throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
                    }

                /*!
                 * @brief This function serializes a unsigned short.
                 * @param ushort_t The value of the unsigned short that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const uint16_t ushort_t)
                    {
                        return serialize((int16_t)ushort_t);
                    }

                /*!
                 * @brief This function serializes a short.
                 * @param short_t The value of the short that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const int16_t short_t)
                    {
                        if(((m_lastPosition - m_currentPosition) >= sizeof(short_t)) || resize(sizeof(short_t)))
                        {
                            m_storage.insert(m_currentPosition, short_t);

                            return *this;
                        }

                        throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
                    }

                /*!
                 * @brief This function serializes a unsigned long.
                 * @param ulong_t The value of the unsigned long that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::FastCdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const uint32_t ulong_t)
                    {
                        return serialize((int32_t)ulong_t);
                    }

                /*!
                 * @brief This function serializes a long.
                 * @param long_t The value of the long that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const int32_t long_t)
                    {
                        if(((m_lastPosition - m_currentPosition) >= sizeof(long_t)) || resize(sizeof(long_t)))
                        {
                            m_storage.insert(m_currentPosition, long_t);

                            return *this;
                        }

                        throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
                    }

                /*!
                 * @brief This function serializes a unsigned long long.
                 * @param ulonglong_t The value of the unsigned long long that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const uint64_t ulonglong_t)
                    {
                        return serialize((int64_t)ulonglong_t);
                    }

                /*!
                 * @brief This function serializes a long long.
                 * @param longlong_t The value of the long long that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const int64_t longlong_t)
                    {
                        if(((m_lastPosition - m_currentPosition) >= sizeof(longlong_t)) || resize(sizeof(longlong_t)))
                        {
                            m_storage.insert(m_currentPosition, longlong_t);

                            return *this;
                        }

                        throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
                    }

                /*!
                 * @brief This function serializes a float.
                 * @param float_t The value of the float that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const float float_t)
                    {
                        if(((m_lastPosition - m_currentPosition) >= sizeof(float_t)) || resize(sizeof(float_t)))
                        {
                            m_storage.insert(m_currentPosition, float_t);

                            return *this;
                        }

                        throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
                    }

                /*!
                 * @brief This function serializes a double.
                 * @param double_t The value of the double that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const double double_t)
                    {
                        if(((m_lastPosition - m_currentPosition) >= sizeof(double_t)) || resize(sizeof(double_t)))
                        {
                            m_storage.insert(m_currentPosition, double_t);

                            return *this;
                        }

                        throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
                    }

                /*!
                 * @brief This function serializes a boolean.
                 * @param bool_t The value of the boolean that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const bool bool_t);

                /*!
                 * @brief This function serializes a string.
                 * @param bool_t The string that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::string &string_t);

                /*!
                 * @brief This function template serializes an array.
                 * @param array_t The array that will be serialized in the storage.
                 * @return Reference to the eProsima::marshalling::FastCdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    inline FastCdr& serialize(const std::array<_T, _Size> &array_t)
                    { return dynamic_cast<FastCdr&>(serializeArray(array_t.data(), array_t.size()));}

                /*!
                 * @brief This function is used to serialize sequences of octets.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<uint8_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of chars.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<char> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of unsigned shorts.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<uint16_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of shorts.
                 * @param vector_t The sequence that will be serialized in the storage mechanism.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<int16_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of unsigned long.
                 * @param vector_t The sequence that will be serialized in the storage mechanism.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<uint32_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of longs.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<int32_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of unsigned long longs.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<uint64_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of long longs.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<int64_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of floats.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<float> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of doubles.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<double> &vector_t);

                /*!
                 * @brief This function serializes an array of octets.
                 * @param octet_t The sequence of octets  that will be serialized in the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serializeArray(const uint8_t *octet_t, size_t numElements)
                    {
                        return serializeArray((const char*)octet_t, numElements);
                    }

                /*!
                 * @brief This function serializes an array of characteres.
                 * @param char_t The array of characteres  that will be serialized in the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serializeArray(const char *char_t, size_t numElements);

                /*!
                 * @brief This function serializes an array of unsigned shorts.
                 * @param ushort_t The array of unsigned shorts  that will be serialized in the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serializeArray(const uint16_t *ushort_t, size_t numElements)
                    {
                        return serializeArray((const int16_t*)ushort_t, numElements);
                    }

                /*!
                 * @brief This function serializes an array of shorts.
                 * @param short_t The array of shorts  that will be serialized in the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serializeArray(const int16_t *short_t, size_t numElements);

                /*!
                 * @brief This function serializes an array of unsigned longs.
                 * @param ulong_t The array of unsigned longs  that will be serialized in the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serializeArray(const uint32_t *ulong_t, size_t numElements)
                    {
                        return serializeArray((const int32_t*)ulong_t, numElements);
                    }

                /*!
                 * @brief This function serializes an array of longs.
                 * @param long_t The array of longs  that will be serialized in the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serializeArray(const int32_t *long_t, size_t numElements);

                /*!
                 * @brief This function serializes an array of unsigned long longs.
                 * @param ulonglong_t The array of unsigned long longs  that will be serialized in the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serializeArray(const uint64_t *ulonglong_t, size_t numElements)
                    {
                        return serializeArray((const int64_t*)ulonglong_t, numElements);
                    }

                /*!
                 * @brief This function serializes an array of long longs.
                 * @param longlong_t The array of  long longs  that will be serialized in the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serializeArray(const int64_t *longlong_t, size_t numElements);

                /*!
                 * @brief This function serializes an array of floats.
                 * @param float_t The array of floats that will be serialized in the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serializeArray(const float *float_t, size_t numElements);

                /*!
                 * @brief This function serializes an array of doubles.
                 * @param double_t The array of doubles that will be serialized in the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serializeArray(const double *double_t, size_t numElements);

                /*!
                 * @brief This function deserializes an octet.
                 * @param octet_t The variable that will store the octet read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(uint8_t &octet_t)
                    {
                        return deserialize((char&)octet_t);
                    }

                /*!
                 * @brief This function deserializes a characteres.
                 * @param char_t The variable that will store the character read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(char &char_t)
                    {
                        if((m_lastPosition - m_currentPosition) >= sizeof(char_t))
                        {
                            m_storage.get(m_currentPosition, char_t);
                            return *this;
                        }

                        throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
                    }

                /*!
                 * @brief This function deserializes a unsigned short.
                 * @param ushort_t The variable that will store the unsigned short read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(uint16_t &ushort_t)
                    {
                        return deserialize((int16_t&)ushort_t);
                    }

                /*!
                 * @brief This function deserializes a short.
                 * @param short_t The variable that will store the short read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(int16_t &short_t)
                    {
                        if((m_lastPosition - m_currentPosition) >= sizeof(short_t))
                        {
                            m_storage.get(m_currentPosition, short_t);

                            return *this;
                        }

                        throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
                    }

                /*!
                 * @brief This function deserializes a unsigned long.
                 * @param ulong_t The variable that will store the unsigned long read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(uint32_t &ulong_t)
                    {
                        return deserialize((int32_t&)ulong_t);
                    }

                /*!
                 * @brief This function deserializes a long.
                 * @param long_t The variable that will store the long read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(int32_t &long_t)
                    {
                        if((m_lastPosition - m_currentPosition) >= sizeof(long_t))
                        {
                            m_storage.get(m_currentPosition, long_t);

                            return *this;
                        }

                        throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
                    }

                /*!
                 * @brief This function deserializes a unsigned long long.
                 * @param ulonglong_t The variable that will store the unsigned long long read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(uint64_t &ulonglong_t)
                    {
                        return deserialize((int64_t&)ulonglong_t);
                    }

                /*!
                 * @brief This function deserializes a long long.
                 * @param longlong_t The variable that will store the long long read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(int64_t &longlong_t)
                    {
                        if((m_lastPosition - m_currentPosition) >= sizeof(longlong_t))
                        {
                            m_storage.get(m_currentPosition, longlong_t);

                            return *this;
                        }

                        throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
                    }

                /*!
                 * @brief This function deserializes a float.
                 * @param float_t The variable that will store the float read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(float &float_t)
                    {
                        if((m_lastPosition - m_currentPosition) >= sizeof(float_t))
                        {
                            m_storage.get(m_currentPosition, float_t);

                            return *this;
                        }

                        throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
                    }

                /*!
                 * @brief This function deserializes a double.
                 * @param double_t The variable that will store the double read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(double &double_t)
                    {
                        if((m_lastPosition - m_currentPosition) >= sizeof(double_t))
                        {
                            m_storage.get(m_currentPosition, double_t);

                            return *this;
                        }

                        throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
                    }

                /*!
                 * @brief This function deserializes a boolean.
                 * @param bool_t The variable that will store the boolean read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
                 */
                Marshalling& deserialize(bool &bool_t);

                /*!
                 * @brief This function deserializes a string.
                 * @param string_t The variable that will store the string read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::string &string_t);

                /*!
                 * @brief This function template deserializes an array.
                 * @param array_t The variable that will store the array read from the storage.
                 * @return Reference to the eProsima::marshalling::FastCdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    inline FastCdr& deserialize(std::array<_T, _Size> &array_t)
                    { return dynamic_cast<FastCdr&>(deserializeArray(array_t.data(), array_t.size()));}

                /*!
                 * @brief This function is used to deserialize sequences of octets.
                 * @param vector_t The variable that will store the sequence of octets read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<uint8_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of chars.
                 * @param vector_t The variable that will store the sequence of chars read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<char> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of unsigned shorts.
                 * @param vector_t The variable that will store the sequence of unsigned shorts read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<uint16_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of shorts.
                 * @param vector_t The variable that will store the sequence of shorts read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<int16_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of unsigned long.
                 * @param vector_t The variable that will store the sequence of unsigned longs read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<uint32_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of longs.
                 * @param vector_t The variable that will store the sequence of longs read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<int32_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of unsigned long longs.
                 * @param vector_t The variable that will store the sequence of unsigned long longs read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<uint64_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of long longs.
                 * @param vector_t The variable that will store the sequence of long longs read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<int64_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of floats.
                 * @param vector_t The variable that will store the sequence of floats read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<float> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of doubles.
                 * @param vector_t The variable that will store the sequence of doubles read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<double> &vector_t);

                /*!
                 * @brief This function deserializes an array of octets.
                 * @param octet_t The variable that will store the array of octets read from the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserializeArray(uint8_t *octet_t, size_t numElements)
                    {
                        return deserializeArray((char*)octet_t, numElements);
                    }

                /*!
                 * @brief This function deserializes an array of characteres.
                 * @param char_t The variable that will store the array of characteres read from the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserializeArray(char *char_t, size_t numElements);

                /*!
                 * @brief This function deserializes an array of unsigned short.
                 * @param ushort_t The variable that will store the array of unsigned short read from the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserializeArray(uint16_t *ushort_t, size_t numElements)
                    {
                        return deserializeArray((int16_t*)ushort_t, numElements);
                    }

                /*!
                 * @brief This function deserializes an array of short.
                 * @param short_t The variable that will store the array of short read from the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserializeArray(int16_t *short_t, size_t numElements);

                /*!
                 * @brief This function deserializes an array of unsigned longt.
                 * @param ulong_t The variable that will store the array of unsigned long read from the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserializeArray(uint32_t *ulong_t, size_t numElements)
                    {
                        return deserializeArray((int32_t*)ulong_t, numElements);
                    }

                /*!
                 * @brief This function deserializes an array of long.
                 * @param long_t The variable that will store the array of long read from the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserializeArray(int32_t *long_t, size_t numElements);

                /*!
                 * @brief This function deserializes an array of unsigned long long.
                 * @param ulonglong_t The variable that will store the array of unsigned long long read from the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserializeArray(uint64_t *ulonglong_t, size_t numElements)
                    {
                        return deserializeArray((int64_t*)ulonglong_t, numElements);
                    }

                /*!
                 * @brief This function deserializes an array of long long.
                 * @param longlong_t The variable that will store the array of long long read from the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserializeArray(int64_t *longlong_t, size_t numElements);

                /*!
                 * @brief This function deserializes an array of float.
                 * @param float_t The variable that will store the array of float read from the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserializeArray(float *float_t, size_t numElements);

                /*!
                 * @brief This function deserializes an array of double.
                 * @param double_t The variable that will store the array of double read from the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserializeArray(double *double_t, size_t numElements);

            private:

                /*!
                 * @brief This function template detects the content type of the STD container array and serializes the array.
                 * @param array_t The array that will be serialized in the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::FastCdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    FastCdr& serializeArray(const std::array<_T, _Size> *array_t, size_t numElements)
                    {
                        return dynamic_cast<FastCdr&>(serializeArray(array_t->data(), numElements * array_t->size()));
                    }

                /*!
                 * @brief This function template detects the content type of the STD container array and deserializes the array.
                 * @param array_t The variable that will store the array read from the storage.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::FastCdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    FastCdr& deserializeArray(std::array<_T, _Size> *array_t, size_t numElements)
                    {
                        return dynamic_cast<FastCdr&>(deserializeArray(array_t->data(), numElements * array_t->size()));
                    }

                bool resize(size_t minSizeInc);

                //! @brief Reference to the storage that will be serialized/deserialized.
                storage::Storage &m_storage;

                //! @brief The current position in the serialization/deserialization process.
                storage::Storage::iterator &m_currentPosition;

                //! @brief The last position in the storage;
                storage::Storage::iterator &m_lastPosition;

                //! @brief Common message for BadParamException exceptions.
                static const std::string BAD_PARAM_MESSAGE_DEFAULT;

                //! @brief Common message for NotEnoughMemoryException exceptions.
                static const std::string NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT;
        };
    }
}

#endif //_CPP_CDR_FASTCDR_H_
