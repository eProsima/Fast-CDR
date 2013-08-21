#ifndef _CPP_CDR_CDR_H_
#define _CPP_CDR_CDR_H_

#include "cpp/cdr/Cdr_dll.h"
#include "cpp/Marshalling.h"
#include "cpp/storage/Storage.h"
#include "cpp/exceptions/Exception.h"

namespace eProsima
{
    namespace marshalling
    {
        /*!
         * @brief This class offers an interface to serialize/deserialize some basic types using CDR protocol inside a eProsima::storage::Storage.
         * @ingroup CDRAPIREFERENCE
         */
        class Cdr_DllAPI Cdr : public Marshalling
        {
            public:

                //! @brief This enumeration represents the two type of CDR serialication that it supported by eProsima::marshalling::Cdr.
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
                 * @brief This enumeration represents endianness types.
                 */
                typedef enum
                {
                    //! @brief Big endianness.
                    BIG_ENDIANNESS = 0x0,
                    //! @brief Little endianness.
                    LITTLE_ENDIANNESS = 0x1
                } Endianness;

                //! @brief Default endiness in the system.
                static const Endianness DEFAULT_ENDIAN;

                /*!
                 * @brief This class stores the current state of a CDR serialization. Its usage is dangerours when the eProsima::FastBuffer can use
                 * a user's function to allocate dynamically memory.
                 */
                class Cdr_DllAPI state
                {
                    friend class Cdr;
                    public:

                    /*!
                     * @brief Default constructor.
                     */
                    state(Cdr &cdr);

                    /*!
                     * @brief Destructor.
                     */
                    virtual ~state();

                    private:

                    //! @brief The position in the buffer when the state was created.
                    storage::Storage::iterator *m_currentPosition;

                    //! @brief The position from the aligment is calculated,  when the state was created..
                    storage::Storage::iterator *m_alignPosition;

                    //! @brief This attribute specified if it is needed to swap the bytes when the state was created..
                    bool m_swapBytes;

                    //! @brief Stores the last datasize serialized/deserialized when the state was created.
                    size_t m_lastDataSize;
                };

                /*!
                 * @brief This constructor creates a eProsima::marshalling::Cdr object that could serialize/deserialize
                 * the assigned buffer.
                 *
                 * @param storage A reference to the buffer that contains or will contain the CDR representation.
                 * @param endianness The initial endianness that will be used. By default is the endianness of the system.
                 * @param cdrType Represents the type of CDR that will be use in serialization/deserialization. By default is CORBA CDR.
                 */
                Cdr(storage::Storage &storage, const Endianness endianness = DEFAULT_ENDIAN, const CdrType cdrType = CORBA_CDR);

                /*!
                 * @brief Destructor.
                 */
                virtual ~Cdr();

                /*!
                 * @brief This function reads the encapsulation of the CDR stream.
                 *        If the CDR stream contains a encapsulation, then this function would call before starting to deserialize.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
                 */
                Cdr& read_encapsulation();

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
                 * @brief This function resets the current position in the buffer to the begining.
                 */
                void reset();

                /*!
                 * @brief This function returns the length of the serialized data inside the stream.
                 * @return The length of the serialized data.
                 */
                size_t getSerializedDataLength() const;

                /*!
                 * @brief This function returns the current state of the CDR serialization process.
                 * @return The current state of the CDR serialization process.
                 */
                state getState();

                /*!
                 * @brief This function sets a previous state of the CDR serialization process;
                 * @param state Previous state that will be set again.
                 */
                void setState(state &state);

                /*!
                 * @brief This function resets the alignment to current position in the buffer.
                 */
                inline void resetAlignment(){m_alignPosition->clone(m_currentPosition);}

                /*!
                 * @brief This operator serializes an octet.
                 * @param octet_t The value of the octet that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const uint8_t octet_t){return serialize(octet_t);}

                /*!
                 * @brief This operator serializes a char.
                 * @param char_t The value of the character that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const char char_t){return serialize(char_t);}

                /*!
                 * @brief This operator serializes a unsigned short.
                 * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const uint16_t ushort_t){return serialize(ushort_t);}

                /*!
                 * @brief This operator serializes a short.
                 * @param short_t The value of the short that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const int16_t short_t){return serialize(short_t);}

                /*!
                 * @brief This operator serializes a unsigned long.
                 * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const uint32_t ulong_t){return serialize(ulong_t);}

                /*!
                 * @brief This operator serializes a long.
                 * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const int32_t long_t){return serialize(long_t);}

                /*!
                 * @brief This operator serializes a unsigned long long.
                 * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const uint64_t ulonglong_t){return serialize(ulonglong_t);}

                /*!
                 * @brief This operator serializes a long long.
                 * @param longlong_t The value of the long long that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const int64_t longlong_t){return serialize(longlong_t);}

                /*!
                 * @brief This operator serializes a float.
                 * @param float_t The value of the float that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const float float_t){return serialize(float_t);}

                /*!
                 * @brief This operator serializes a double.
                 * @param double_t The value of the double that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const double double_t){return serialize(double_t);}

                /*!
                 * @brief This operator serializes a boolean.
                 * @param bool_t The value of the boolean that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const bool bool_t){return serialize(bool_t);}

                /*!
                 * @brief This operator serializes a string.
                 * @param string_t The string that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator<<(const std::string &string_t){return serialize(string_t);}

                /*!
                 * @brief This operator template is used to serialize arrays.
                 * @param array_t The array that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    inline Cdr& operator<<(const std::array<_T, _Size> &array_t){return serialize<_T, _Size>(array_t);}

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
                 * @param octet_t The variable that will store the octet read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(uint8_t &octet_t){return deserialize(octet_t);}

                /*!
                 * @brief This operator deserializes a char.
                 * @param char_t The variable that will store the character read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(char &char_t){return deserialize(char_t);}

                /*!
                 * @brief This operator deserializes a unsigned short.
                 * @param ushort_t The variable that will store the unsigned short read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(uint16_t &ushort_t){return deserialize(ushort_t);}

                /*!
                 * @brief This operator deserializes a short.
                 * @param short_t The variable that will store the short read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(int16_t &short_t){return deserialize(short_t);}

                /*!
                 * @brief This operator deserializes a unsigned long.
                 * @param ulong_t The variable that will store the unsigned long read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(uint32_t &ulong_t){return deserialize(ulong_t);}

                /*!
                 * @brief This operator deserializes a long.
                 * @param long_t The variable that will store the long read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(int32_t &long_t){return deserialize(long_t);}

                /*!
                 * @brief This operator deserializes a unsigned long long.
                 * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(uint64_t &ulonglong_t){return deserialize(ulonglong_t);}

                /*!
                 * @brief This operator deserializes a long long.
                 * @param longlong_t The variable that will store the long long read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(int64_t &longlong_t){return deserialize(longlong_t);}

                /*!
                 * @brief This operator deserializes a float.
                 * @param float_t The variable that will store the float read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(float &float_t){return deserialize(float_t);}

                /*!
                 * @brief This operator deserializes a double.
                 * @param double_t The variable that will store the double read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(double &double_t){return deserialize(double_t);}

                /*!
                 * @brief This operator deserializes a boolean.
                 * @param bool_t The variable that will store the boolean read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
                 */
                inline Marshalling& operator>>(bool &bool_t){return deserialize(bool_t);}

                /*!
                 * @brief This operator deserializes a string.
                 * @param string_t The variable that will store the string read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::string &string_t){return deserialize(string_t);}

                /*!
                 * @brief This operator template is used to deserialize arrays.
                 * @param array_t The variable that will store the array read from the buffer.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    inline Cdr& operator>>(std::array<_T, _Size> &array_t){return deserialize<_T, _Size>(array_t);}

                /*!
                 * @brief This operator is used to deserialize sequences of octets.
                 * @param vector_t The variable that will store the sequence of octets read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline Marshalling& operator>>(std::vector<uint8_t> &vector_t){return deserialize(vector_t);}

                /*!
                 * @brief This operator is used to deserialize sequences of chars.
                 * @param vector_t The variable that will store the sequence of chars read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
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
                 * @param octet_t The value of the octet that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const uint8_t octet_t)
                    {
                        return serialize((char)octet_t);
                    }

                /*!
                 * @brief This function serializes an octet with a different endianness.
                 * @param octet_t The value of the octet that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& serialize(const uint8_t octet_t, Endianness endianness)
                    {
                        return serialize((char)octet_t, endianness);
                    }

                /*!
                 * @brief This function serializes a char.
                 * @param char_t The value of the character that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const char char_t);

                /*!
                 * @brief This function serializes a char with a different endianness.
                 * @param char_t The value of the character that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& serialize(const char char_t, Endianness endianness)
                    {
                        return dynamic_cast<Cdr&>(serialize(char_t));
                    }

                /*!
                 * @brief This function serializes a unsigned short.
                 * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const uint16_t ushort_t)
                    {
                        return serialize((int16_t)ushort_t);
                    }

                /*!
                 * @brief This function serializes a unsigned short with a different endianness.
                 * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& serialize(const uint16_t ushort_t, Endianness endianness)
                    {
                        return serialize((int16_t)ushort_t, endianness);
                    }

                /*!
                 * @brief This function serializes a short.
                 * @param short_t The value of the short that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const int16_t short_t);

                /*!
                 * @brief This function serializes a short with a different endianness.
                 * @param short_t The value of the short that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const int16_t short_t, Endianness endianness);

                /*!
                 * @brief This function serializes a unsigned long.
                 * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const uint32_t ulong_t)
                    {
                        return serialize((int32_t)ulong_t);
                    }

                /*!
                 * @brief This function serializes a unsigned long with a different endianness.
                 * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& serialize(const uint32_t ulong_t, Endianness endianness)
                    {
                        return serialize((int32_t)ulong_t, endianness);
                    }

                /*!
                 * @brief This function serializes a long.
                 * @param long_t The value of the long that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const int32_t long_t);

                /*!
                 * @brief This function serializes a long with a different endianness.
                 * @param long_t The value of the long that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const int32_t long_t, Endianness endianness);

                /*!
                 * @brief This function serializes a unsigned long long.
                 * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& serialize(const uint64_t ulonglong_t)
                    {
                        return serialize((int64_t)ulonglong_t);
                    }

                /*!
                 * @brief This function serializes a unsigned long long with a different endianness.
                 * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& serialize(const uint64_t ulonglong_t, Endianness endianness)
                    {
                        return serialize((int64_t)ulonglong_t, endianness);
                    }

                /*!
                 * @brief This function serializes a long long.
                 * @param longlong_t The value of the long long that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const int64_t longlong_t);

                /*!
                 * @brief This function serializes a long long with a different endianness.
                 * @param longlong_t The value of the long long that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const int64_t longlong_t, Endianness endianness);

                /*!
                 * @brief This function serializes a float.
                 * @param float_t The value of the float that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const float float_t);

                /*!
                 * @brief This function serializes a float with a different endianness.
                 * @param float_t The value of the float that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const float float_t, Endianness endianness);

                /*!
                 * @brief This function serializes a double.
                 * @param double_t The value of the double that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const double double_t);

                /*!
                 * @brief This function serializes a double with a different endianness.
                 * @param double_t The value of the double that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const double double_t, Endianness endianness);

                /*!
                 * @brief This function serializes a boolean.
                 * @param bool_t The value of the boolean that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const bool bool_t);

                /*!
                 * @brief This function serializes a boolean with a different endianness.
                 * @param bool_t The value of the boolean that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& serialize(const bool bool_t, Endianness endianness)
                    {
                        return dynamic_cast<Cdr&>(serialize(bool_t));
                    }

                /*!
                 * @brief This function serializes a string.
                 * @param bool_t The string that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::string &string_t);

                /*!
                 * @brief This function serializes a string with a different endianness.
                 * @param bool_t The string that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const std::string &string_t, Endianness endianness);

                /*!
                 * @brief This function template serializes an array.
                 * @param array_t The array that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    inline Cdr& serialize(const std::array<_T, _Size> &array_t)
                    { return dynamic_cast<Cdr&>(serializeArray(array_t.data(), array_t.size()));}

                /*!
                 * @brief This function template serializes an array with a different endianness.
                 * @param array_t The array that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    inline Cdr& serialize(const std::array<_T, _Size> &array_t, Endianness endianness)
                    { return serializeArray(array_t.data(), array_t.size(), endianness);}

                /*!
                 * @brief This function is used to serialize sequences of octets.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<uint8_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of octets with a different endianness.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const std::vector<uint8_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to serialize sequences of chars.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<char> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of chars with a different endianness.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const std::vector<char> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to serialize sequences of unsigned shorts.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<uint16_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of unsigned shorts with a different endianness.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const std::vector<uint16_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to serialize sequences of shorts.
                 * @param vector_t The sequence that will be serialized in the storage mechanism.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<int16_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of shorts with a different endianness.
                 * @param vector_t The sequence that will be serialized in the storage mechanism.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const std::vector<int16_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to serialize sequences of unsigned long.
                 * @param vector_t The sequence that will be serialized in the storage mechanism.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<uint32_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of unsigned longs with a different endianness.
                 * @param vector_t The sequence that will be serialized in the storage mechanism.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const std::vector<uint32_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to serialize sequences of longs.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<int32_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of longs with a different endianness.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const std::vector<int32_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to serialize sequences of unsigned long longs.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<uint64_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of unsigned long longs with a different endianness.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const std::vector<uint64_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to serialize sequences of long longs.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<int64_t> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of long longs with a different endianness.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const std::vector<int64_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to serialize sequences of floats.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<float> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of floats with a different endianness.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const std::vector<float> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to serialize sequences of doubles.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serialize(const std::vector<double> &vector_t);

                /*!
                 * @brief This function is used to serialize sequences of doubles with a different endianness.
                 * @param vector_t The sequence that will be serialized in the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serialize(const std::vector<double> &vector_t, Endianness endianness);

                /*!
                 * @brief This function serializes an array of octets.
                 * @param octet_t The sequence of octets  that will be serialized in the buffer.
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
                 * @brief This function serializes an array of octets with a different endianness.
                 * @param octet_t The array of octets  that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& serializeArray(const uint8_t *octet_t, size_t numElements, Endianness endianness)
                    {
                        return dynamic_cast<Cdr&>(serializeArray((const char*)octet_t, numElements));
                    }

                /*!
                 * @brief This function serializes an array of characteres.
                 * @param char_t The array of characteres  that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serializeArray(const char *char_t, size_t numElements);

                /*!
                 * @brief This function serializes an array of characteres with a different endianness.
                 * @param char_t The array of characteres  that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& serializeArray(const char *char_t, size_t numElements, Endianness endianness)
                    {
                        return dynamic_cast<Cdr&>(serializeArray(char_t, numElements));
                    }

                /*!
                 * @brief This function serializes an array of unsigned shorts.
                 * @param ushort_t The array of unsigned shorts  that will be serialized in the buffer.
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
                 * @brief This function serializes an array of unsigned shorts with a different endianness.
                 * @param ushort_t The array of unsigned shorts  that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& serializeArray(const uint16_t *ushort_t, size_t numElements, Endianness endianness)
                    {
                        return serializeArray((const int16_t*)ushort_t, numElements, endianness);
                    }

                /*!
                 * @brief This function serializes an array of shorts.
                 * @param short_t The array of shorts  that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serializeArray(const int16_t *short_t, size_t numElements);

                /*!
                 * @brief This function serializes an array of shorts with a different endianness.
                 * @param short_t The array of shorts  that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serializeArray(const int16_t *short_t, size_t numElements, Endianness endianness);

                /*!
                 * @brief This function serializes an array of unsigned longs.
                 * @param ulong_t The array of unsigned longs  that will be serialized in the buffer.
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
                 * @brief This function serializes an array of unsigned longs with a different endianness.
                 * @param ulong_t The array of unsigned longs  that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& serializeArray(const uint32_t *ulong_t, size_t numElements, Endianness endianness)
                    {
                        return serializeArray((const int32_t*)ulong_t, numElements, endianness);
                    }

                /*!
                 * @brief This function serializes an array of longs.
                 * @param long_t The array of longs  that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serializeArray(const int32_t *long_t, size_t numElements);

                /*!
                 * @brief This function serializes an array of longs with a different endianness.
                 * @param long_t The array of longs  that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serializeArray(const int32_t *long_t, size_t numElements, Endianness endianness);

                /*!
                 * @brief This function serializes an array of unsigned long longs.
                 * @param ulonglong_t The array of unsigned long longs  that will be serialized in the buffer.
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
                 * @brief This function serializes an array of unsigned long longs with a different endianness.
                 * @param ulonglong_t The array of unsigned long longs  that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& serializeArray(const uint64_t *ulonglong_t, size_t numElements, Endianness endianness)
                    {
                        return serializeArray((const int64_t*)ulonglong_t, numElements, endianness);
                    }

                /*!
                 * @brief This function serializes an array of long longs.
                 * @param longlong_t The array of  long longs  that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serializeArray(const int64_t *longlong_t, size_t numElements);

                /*!
                 * @brief This function serializes an array of long longs with a different endianness.
                 * @param longlong_t The array of  long longs  that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serializeArray(const int64_t *longlong_t, size_t numElements, Endianness endianness);

                /*!
                 * @brief This function serializes an array of floats.
                 * @param float_t The array of floats that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serializeArray(const float *float_t, size_t numElements);

                /*!
                 * @brief This function serializes an array of floats with a different endianness.
                 * @param float_t The array of floats that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serializeArray(const float *float_t, size_t numElements, Endianness endianness);

                /*!
                 * @brief This function serializes an array of doubles.
                 * @param double_t The array of doubles that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Marshalling& serializeArray(const double *double_t, size_t numElements);

                /*!
                 * @brief This function serializes an array of doubles with a different endianness.
                 * @param double_t The array of doubles that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
                 */
                Cdr& serializeArray(const double *double_t, size_t numElements, Endianness endianness);

                /*!
                 * @brief This function deserializes an octet.
                 * @param octet_t The variable that will store the octet read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(uint8_t &octet_t)
                    {
                        return deserialize((char&)octet_t);
                    }

                /*!
                 * @brief This function deserializes an octet with a different endianness.
                 * @param octet_t The variable that will store the octet read from the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& deserialize(uint8_t &octet_t, Endianness endianness)
                    {
                        return deserialize((char&)octet_t, endianness);
                    }

                /*!
                 * @brief This function deserializes a characteres.
                 * @param char_t The variable that will store the character read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(char &char_t);

                /*!
                 * @brief This function deserializes a characteres with a different endianness.
                 * @param char_t The variable that will store the character read from the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& deserialize(char &char_t, Endianness endianness)
                    {
                        return dynamic_cast<Cdr&>(deserialize(char_t));
                    }

                /*!
                 * @brief This function deserializes a unsigned short.
                 * @param ushort_t The variable that will store the unsigned short read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(uint16_t &ushort_t)
                    {
                        return deserialize((int16_t&)ushort_t);
                    }

                /*!
                 * @brief This function deserializes a unsigned short with a different endianness.
                 * @param ushort_t The variable that will store the unsigned short read from the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& deserialize(uint16_t &ushort_t, Endianness endianness)
                    {
                        return deserialize((int16_t&)ushort_t, endianness);
                    }

                /*!
                 * @brief This function deserializes a short.
                 * @param short_t The variable that will store the short read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(int16_t &short_t);

                /*!
                 * @brief This function deserializes a short with a different endianness.
                 * @param short_t The variable that will store the short read from the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(int16_t &short_t, Endianness endianness);

                /*!
                 * @brief This function deserializes a unsigned long.
                 * @param ulong_t The variable that will store the unsigned long read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(uint32_t &ulong_t)
                    {
                        return deserialize((int32_t&)ulong_t);
                    }

                /*!
                 * @brief This function deserializes a unsigned long with a different endianness.
                 * @param ulong_t The variable that will store the unsigned long read from the buffer..
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& deserialize(uint32_t &ulong_t, Endianness endianness)
                    {
                        return deserialize((int32_t&)ulong_t, endianness);
                    }

                /*!
                 * @brief This function deserializes a long.
                 * @param long_t The variable that will store the long read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(int32_t &long_t);

                /*!
                 * @brief This function deserializes a long with a different endianness.
                 * @param long_t The variable that will store the long read from the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(int32_t &long_t, Endianness endianness);

                /*!
                 * @brief This function deserializes a unsigned long long.
                 * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Marshalling& deserialize(uint64_t &ulonglong_t)
                    {
                        return deserialize((int64_t&)ulonglong_t);
                    }

                /*!
                 * @brief This function deserializes a unsigned long long with a different endianness.
                 * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& deserialize(uint64_t &ulonglong_t, Endianness endianness)
                    {
                        return deserialize((int64_t&)ulonglong_t, endianness);
                    }

                /*!
                 * @brief This function deserializes a long long.
                 * @param longlong_t The variable that will store the long long read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(int64_t &longlong_t);

                /*!
                 * @brief This function deserializes a long long with a different endianness.
                 * @param longlong_t The variable that will store the long long read from the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(int64_t &longlong_t, Endianness endianness);

                /*!
                 * @brief This function deserializes a float.
                 * @param float_t The variable that will store the float read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(float &float_t);

                /*!
                 * @brief This function deserializes a float with a different endianness.
                 * @param float_t The variable that will store the float read from the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(float &float_t, Endianness endianness);

                /*!
                 * @brief This function deserializes a double.
                 * @param double_t The variable that will store the double read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(double &double_t);

                /*!
                 * @brief This function deserializes a double with a different endianness.
                 * @param double_t The variable that will store the double read from the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(double &double_t, Endianness endianness);

                /*!
                 * @brief This function deserializes a boolean.
                 * @param bool_t The variable that will store the boolean read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
                 */
                Marshalling& deserialize(bool &bool_t);

                /*!
                 * @brief This function deserializes a boolean with a different endianness.
                 * @param bool_t The variable that will store the boolean read from the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
                 */
                inline
                    Cdr& deserialize(bool &bool_t, Endianness endianness)
                    {
                       return dynamic_cast<Cdr&>(deserialize(bool_t));
                    };

                /*!
                 * @brief This function deserializes a string.
                 * @param string_t The variable that will store the string read from the buffer.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::string &string_t);

                /*!
                 * @brief This function deserializes a string with a different endianness.
                 * @param string_t The variable that will store the string read from the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(std::string &string_t, Endianness endianness);

                /*!
                 * @brief This function template deserializes an array.
                 * @param array_t The variable that will store the array read from the buffer.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    inline Cdr& deserialize(std::array<_T, _Size> &array_t)
                    { return dynamic_cast<Cdr&>(deserializeArray(array_t.data(), array_t.size()));}

                /*!
                 * @brief This function template deserializes an array with a different endianness.
                 * @param array_t The variable that will store the array read from the buffer.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    inline Cdr& deserialize(std::array<_T, _Size> &array_t, Endianness endianness)
                    { return deserializeArray(array_t.data(), array_t.size(), endianness);}

                /*!
                 * @brief This function is used to deserialize sequences of octets.
                 * @param vector_t The variable that will store the sequence of octets read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<uint8_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of octets with a different endianness.
                 * @param vector_t The variable that will store the sequence of octets read from the storage.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(std::vector<uint8_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to deserialize sequences of chars.
                 * @param vector_t The variable that will store the sequence of chars read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<char> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of chars with a different endianness.
                 * @param vector_t The variable that will store the sequence of chars read from the storage.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(std::vector<char> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to deserialize sequences of unsigned shorts.
                 * @param vector_t The variable that will store the sequence of unsigned shorts read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<uint16_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of unsigned shorts with a different endianness.
                 * @param vector_t The variable that will store the sequence of unsigned shorts read from the storage.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(std::vector<uint16_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to deserialize sequences of shorts.
                 * @param vector_t The variable that will store the sequence of shorts read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<int16_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of shorts with a different endianness.
                 * @param vector_t The variable that will store the sequence of shorts read from the storage.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(std::vector<int16_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to deserialize sequences of unsigned long.
                 * @param vector_t The variable that will store the sequence of unsigned longs read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<uint32_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of unsigned long with a different endianness.
                 * @param vector_t The variable that will store the sequence of unsigned longs read from the storage.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(std::vector<uint32_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to deserialize sequences of longs.
                 * @param vector_t The variable that will store the sequence of longs read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<int32_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of longs with a different endianness.
                 * @param vector_t The variable that will store the sequence of longs read from the storage.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(std::vector<int32_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to deserialize sequences of unsigned long longs.
                 * @param vector_t The variable that will store the sequence of unsigned long longs read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<uint64_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of unsigned long longs with a different endianness.
                 * @param vector_t The variable that will store the sequence of unsigned long longs read from the storage.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(std::vector<uint64_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to deserialize sequences of long longs.
                 * @param vector_t The variable that will store the sequence of long longs read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<int64_t> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of long longs with a different endianness.
                 * @param vector_t The variable that will store the sequence of long longs read from the storage.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(std::vector<int64_t> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to deserialize sequences of floats.
                 * @param vector_t The variable that will store the sequence of floats read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<float> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of floats with a different endianness.
                 * @param vector_t The variable that will store the sequence of floats read from the storage.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(std::vector<float> &vector_t, Endianness endianness);

                /*!
                 * @brief This function is used to deserialize sequences of doubles.
                 * @param vector_t The variable that will store the sequence of doubles read from the storage.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserialize(std::vector<double> &vector_t);

                /*!
                 * @brief This function is used to deserialize sequences of doubles with a different endianness.
                 * @param vector_t The variable that will store the sequence of doubles read from the storage.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserialize(std::vector<double> &vector_t, Endianness endianness);

                /*!
                 * @brief This function deserializes an array of octets.
                 * @param octet_t The variable that will store the array of octets read from the buffer.
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
                 * @brief This function deserializes an array of octets with a different endianness.
                 * @param octet_t The variable that will store the array of octets read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& deserializeArray(uint8_t *octet_t, size_t numElements, Endianness endianness)
                    {
                        return dynamic_cast<Cdr&>(deserializeArray((char*)octet_t, numElements));
                    }

                /*!
                 * @brief This function deserializes an array of characteres.
                 * @param char_t The variable that will store the array of characteres read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserializeArray(char *char_t, size_t numElements);

                /*!
                 * @brief This function deserializes an array of characteres with a different endianness.
                 * @param char_t The variable that will store the array of characteres read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& deserializeArray(char *char_t, size_t numElements, Endianness endianness)
                    {
                        return dynamic_cast<Cdr&>(deserializeArray(char_t, numElements));
                    }

                /*!
                 * @brief This function deserializes an array of unsigned short.
                 * @param ushort_t The variable that will store the array of unsigned short read from the buffer.
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
                 * @brief This function deserializes an array of unsigned short with a different endianness.
                 * @param ushort_t The variable that will store the array of unsigned short read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& deserializeArray(uint16_t *ushort_t, size_t numElements, Endianness endianness)
                    {
                        return deserializeArray((int16_t*)ushort_t, numElements, endianness);
                    }

                /*!
                 * @brief This function deserializes an array of short.
                 * @param short_t The variable that will store the array of short read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserializeArray(int16_t *short_t, size_t numElements);

                /*!
                 * @brief This function deserializes an array of short with a different endianness.
                 * @param short_t The variable that will store the array of short read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserializeArray(int16_t *short_t, size_t numElements, Endianness endianness);

                /*!
                 * @brief This function deserializes an array of unsigned long.
                 * @param ulong_t The variable that will store the array of unsigned long read from the buffer.
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
                 * @brief This function deserializes an array of unsigned long with a different endianness.
                 * @param ulong_t The variable that will store the array of unsigned long read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& deserializeArray(uint32_t *ulong_t, size_t numElements, Endianness endianness)
                    {
                        return deserializeArray((int32_t*)ulong_t, numElements, endianness);
                    }

                /*!
                 * @brief This function deserializes an array of long.
                 * @param long_t The variable that will store the array of long read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserializeArray(int32_t *long_t, size_t numElements);

                /*!
                 * @brief This function deserializes an array of long with a different endianness.
                 * @param long_t The variable that will store the array of long read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserializeArray(int32_t *long_t, size_t numElements, Endianness endianness);

                /*!
                 * @brief This function deserializes an array of unsigned long long.
                 * @param ulonglong_t The variable that will store the array of unsigned long long read from the buffer.
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
                 * @brief This function deserializes an array of unsigned long long with a different endianness.
                 * @param ulonglong_t The variable that will store the array of unsigned long long read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                inline
                    Cdr& deserializeArray(uint64_t *ulonglong_t, size_t numElements, Endianness endianness)
                    {
                        return deserializeArray((int64_t*)ulonglong_t, numElements, endianness);
                    }

                /*!
                 * @brief This function deserializes an array of long long.
                 * @param longlong_t The variable that will store the array of long long read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserializeArray(int64_t *longlong_t, size_t numElements);

                /*!
                 * @brief This function deserializes an array of long long with a different endianness.
                 * @param longlong_t The variable that will store the array of long long read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserializeArray(int64_t *longlong_t, size_t numElements, Endianness endianness);

                /*!
                 * @brief This function deserializes an array of float.
                 * @param float_t The variable that will store the array of float read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserializeArray(float *float_t, size_t numElements);

                /*!
                 * @brief This function deserializes an array of float with a different endianness.
                 * @param float_t The variable that will store the array of float read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserializeArray(float *float_t, size_t numElements, Endianness endianness);

                /*!
                 * @brief This function deserializes an array of double.
                 * @param double_t The variable that will store the array of double read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Marshalling object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Marshalling& deserializeArray(double *double_t, size_t numElements);

                /*!
                 * @brief This function deserializes an array of double with a different endianness.
                 * @param double_t The variable that will store the array of double read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                Cdr& deserializeArray(double *double_t, size_t numElements, Endianness endianness);

            private:

                /*!
                 * @brief This function template detects the content type of the STD container array and serializes the array.
                 * @param array_t The array that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    Cdr& serializeArray(const std::array<_T, _Size> *array_t, size_t numElements)
                    {
                        return dynamic_cast<Cdr&>(serializeArray(array_t->data(), numElements * array_t->size()));
                    }

                /*!
                 * @brief This function template detects the content type of the STD container array and serializes the array with a different endianness.
                 * @param array_t The array that will be serialized in the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    Cdr& serializeArray(const std::array<_T, _Size> *array_t, size_t numElements, Endianness endianness)
                    {
                        return serializeArray(array_t->data(), numElements * array_t->size(), endianness);
                    }

                /*!
                 * @brief This function template detects the content type of the STD container array and deserializes the array.
                 * @param array_t The variable that will store the array read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    Cdr& deserializeArray(std::array<_T, _Size> *array_t, size_t numElements)
                    {
                        return dynamic_cast<Cdr&>(deserializeArray(array_t->data(), numElements * array_t->size()));
                    }

                /*!
                 * @brief This function template detects the content type of STD container array and deserializes the array with a different endianness.
                 * @param array_t The variable that will store the array read from the buffer.
                 * @param numElements Number of the elements in the array.
                 * @param endianness Endianness that will be used in the serialization of this value.
                 * @return Reference to the eProsima::marshalling::Cdr object.
                 * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
                 */
                template<class _T, size_t _Size>
                    Cdr& deserializeArray(std::array<_T, _Size> *array_t, size_t numElements, Endianness endianness)
                    {
                        return deserializeArray(array_t->data(), numElements * array_t->size(), endianness);
                    }

                /*!
                 * @brief This function returns the extra bytes regarding the allign.
                 * @param dataSize The size of the data that will be serialized.
                 * @return The size needed for the aligment.
                 */
                inline size_t alignment(size_t dataSize) const {return dataSize > m_lastDataSize ? (dataSize - ((m_currentPosition->substract(m_alignPosition)) % dataSize)) & (dataSize-1) : 0;}

                /*!
                 * @brief This function jumps the number of bytes of the alignment. These bytes should be calculated with the function eProsima::marshalling::Cdr::alignment.
                 * @param align The number of bytes to be skipped.
                 */
                inline void makeAlign(size_t align){m_currentPosition->increasePosition(align);}

                bool resize(size_t minSizeInc);

                //! @brief Reference to the buffer that will be serialized/deserialized.
                storage::Storage &m_storage;

                //! @brief The type of CDR that will be use in serialization/deserialization.
                CdrType m_cdrType;

                //! @brief Using DDS_CDR type, this attribute stores if the stream buffer contains a parameter list or not.
                DDSCdrPlFlag m_plFlag;

                //! @brief This attribute stores the option flags when the CDR type is DDS_CDR;
                uint16_t m_options;

                //! @brief The endianness that will be applied over the buffer.
                unsigned char m_endianness;

                //! @brief This attribute specified if it is needed to swap the bytes.
                bool m_swapBytes;

                //! @brief Stores the last datasize serialized/deserialized. It's used to optimize.
                size_t m_lastDataSize;

                //! @brief The current position in the serialization/deserialization process.
                storage::Storage::iterator *m_currentPosition;

                //! @brief The position from the aligment is calculated.
                storage::Storage::iterator *m_alignPosition;

                //! @brief The last position in the buffer;
                storage::Storage::iterator *m_lastPosition;

                //! @brief Common message for BadParamException exceptions.
                static const std::string BAD_PARAM_MESSAGE_DEFAULT;

                //! @brief Common message for NotEnoughMemoryException exceptions.
                static const std::string NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT;
        };
    }
}

#endif // _CPP_CDR_CDR_H_
