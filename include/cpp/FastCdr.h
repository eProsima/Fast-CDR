#ifndef _CPP_FASTCDR_H_
#define _CPP_FASTCDR_H_

#include "eProsima_cpp/eProsima_cpp_dll.h"
#include "cpp/FastBuffer.h"
#include "cpp/exceptions/Exception.h"
#include <stdint.h>
#include <string>
#include <array>
#include <vector>

namespace eProsima
{
    /*!
     * @brief This class offers an interface to serialize/deserialize some basic types using a modified CDR protocol inside a eProsima::FastBuffer.
     * This modified CDR protocol provides a serialization mechanism more faster than common CDR protocol, because it doesn't use aligment.
     * @ingroup CDRAPIREFERENCE
     */
    class eProsima_cpp_DllExport FastCdr
    {
    public:

        /*!
         * @brief This class stores the current state of a CDR serialization.
         */
        class eProsima_cpp_DllExport state
        {
            friend class FastCdr;
        public:

            /*!
             * @brief Default constructor.
             */
            state(FastCdr &fastcdr);

        private:

            //! @brief The position in the buffer when the state was created.
            FastBuffer::iterator m_currentPosition;
        };
        /*!
         * @brief This constructor creates a eProsima::FastCdr object that could serialize/deserialize
         * the assigned buffer.
         *
         * @param cdrBuffer A reference to the buffer that contains or will contain the CDR representation.
         */
        FastCdr(FastBuffer &cdrBuffer);

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
         * @brief This function returns the current position in the CDR stream.
         * @return Pointer to the current position in the buffer.
         */
        char* getCurrentPosition();

        /*!
         * @brief This function returns the length of the serialized data inside the stream.
         * @return The length of the serialized data.
         */
        inline size_t getSerializedDataLength() const { return m_currentPosition - m_cdrBuffer.begin();}

        /*!
         * @brief This function returns the current state of the CDR stream.
         * @return The current state of the buffer.
         */
        FastCdr::state getState();

        /*!
         * @brief This function sets a previous state of the CDR stream;
         * @param state Previous state that will be set again.
         */
        void setState(FastCdr::state &state);

        /*!
         * @brief This operator serializes an octet.
         * @param octet_t The value of the octet that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline FastCdr& operator<<(const uint8_t octet_t){return serialize(octet_t);}

        /*!
         * @brief This operator serializes a char.
         * @param char_t The value of the character that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline FastCdr& operator<<(const char char_t){return serialize(char_t);}

        /*!
        * @brief This operator serializes a unsigned short.
        * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator<<(const uint16_t ushort_t){return serialize(ushort_t);}

        /*!
        * @brief This operator serializes a short.
        * @param short_t The value of the short that will be serialized in the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator<<(const int16_t short_t){return serialize(short_t);}

        /*!
        * @brief This operator serializes a unsigned long.
        * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator<<(const uint32_t ulong_t){return serialize(ulong_t);}

        /*!
        * @brief This operator serializes a long.
        * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator<<(const int32_t long_t){return serialize(long_t);}

        /*!
        * @brief This operator serializes a unsigned long long.
        * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator<<(const uint64_t ulonglong_t){return serialize(ulonglong_t);}

        /*!
        * @brief This operator serializes a long long.
        * @param longlong_t The value of the long long that will be serialized in the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator<<(const int64_t longlong_t){return serialize(longlong_t);}

        /*!
        * @brief This operator serializes a float.
        * @param float_t The value of the float that will be serialized in the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator<<(const float float_t){return serialize(float_t);}

        /*!
        * @brief This operator serializes a double.
        * @param double_t The value of the double that will be serialized in the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator<<(const double double_t){return serialize(double_t);}

        /*!
          * @brief This operator serializes a boolean.
          * @param bool_t The value of the boolean that will be serialized in the buffer.
          * @return Reference to the eProsima::FastCdr object.
          * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
          */
        inline FastCdr& operator<<(const bool bool_t){return serialize(bool_t);}

        /*!
          * @brief This operator serializes a string.
          * @param string_t The string that will be serialized in the buffer.
          * @return Reference to the eProsima::FastCdr object.
          * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
          */
        inline FastCdr& operator<<(const std::string &string_t){return serialize(string_t);}

        /*!
          * @brief This operator template is used to serialize arrays.
          * @param array_t The array that will be serialized in the buffer.
          * @return Reference to the eProsima::FastCdr object.
          * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
          */
        template<class _T, size_t _Size>
        inline FastCdr& operator<<(const std::array<_T, _Size> &array_t){return serialize<_T, _Size>(array_t);}

        /*!
          * @brief This operator template is used to serialize sequences.
          * @param vector_t The sequence that will be serialized in the buffer.
          * @return Reference to the eProsima::FastCdr object.
          * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
          */
        template<class _T>
        inline FastCdr& operator<<(const std::vector<_T> &vector_t){return serialize<_T>(vector_t);}

        /*!
        * @brief This operator deserializes an octet.
        * @param octet_t The variable that will store the octet read from the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator>>(uint8_t &octet_t){return deserialize(octet_t);}

        /*!
        * @brief This operator deserializes a char.
        * @param char_t The variable that will store the character read from the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator>>(char &char_t){return deserialize(char_t);}

        /*!
        * @brief This operator deserializes a unsigned short.
        * @param ushort_t The variable that will store the unsigned short read from the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator>>(uint16_t &ushort_t){return deserialize(ushort_t);}

        /*!
        * @brief This operator deserializes a short.
        * @param short_t The variable that will store the short read from the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator>>(int16_t &short_t){return deserialize(short_t);}

        /*!
        * @brief This operator deserializes a unsigned long.
        * @param ulong_t The variable that will store the unsigned long read from the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator>>(uint32_t &ulong_t){return deserialize(ulong_t);}

        /*!
        * @brief This operator deserializes a long.
        * @param long_t The variable that will store the long read from the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator>>(int32_t &long_t){return deserialize(long_t);}

        /*!
        * @brief This operator deserializes a unsigned long long.
        * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator>>(uint64_t &ulonglong_t){return deserialize(ulonglong_t);}

        /*!
        * @brief This operator deserializes a long long.
        * @param longlong_t The variable that will store the long long read from the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator>>(int64_t &longlong_t){return deserialize(longlong_t);}

        /*!
        * @brief This operator deserializes a float.
        * @param float_t The variable that will store the float read from the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator>>(float &float_t){return deserialize(float_t);}

        /*!
        * @brief This operator deserializes a double.
        * @param double_t The variable that will store the double read from the buffer.
        * @return Reference to the eProsima::FastCdr object.
        * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
        */
        inline FastCdr& operator>>(double &double_t){return deserialize(double_t);}

        /*!
          * @brief This operator deserializes a boolean.
          * @param bool_t The variable that will store the boolean read from the buffer.
          * @return Reference to the eProsima::FastCdr object.
          * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
          * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
          */
        inline FastCdr& operator>>(bool &bool_t){return deserialize(bool_t);}

        /*!
          * @brief This operator deserializes a string.
          * @param string_t The variable that will store the string read from the buffer.
          * @return Reference to the eProsima::FastCdr object.
          * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
          */
        inline FastCdr& operator>>(std::string &string_t){return deserialize(string_t);}

        /*!
          * @brief This operator template is used to deserialize arrays.
          * @param array_t The variable that will store the array read from the buffer.
          * @return Reference to the eProsima::FastCdr object.
          * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
          */
        template<class _T, size_t _Size>
        inline FastCdr& operator>>(std::array<_T, _Size> &array_t){return deserialize<_T, _Size>(array_t);}

        /*!
          * @brief This operator template is used to deserialize sequences.
          * @param vector_t The variable that will store the sequence read from the buffer.
          * @return Reference to the eProsima::FastCdr object.
          * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
          */
        template<class _T>
        inline FastCdr& operator>>(std::vector<_T> &vector_t){return deserialize<_T>(vector_t);}

        /*!
         * @brief This function serializes an octet.
         * @param octet_t The value of the octet that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& serialize(const uint8_t octet_t)
        {
            return serialize((char)octet_t);
        }

        /*!
         * @brief This function serializes a char.
         * @param char_t The value of the character that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serialize(const char char_t);

        /*!
         * @brief This function serializes a unsigned short.
         * @param ushort_t The value of the unsigned short that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& serialize(const uint16_t ushort_t)
        {
            return serialize((int16_t)ushort_t);
        }

        /*!
         * @brief This function serializes a short.
         * @param short_t The value of the short that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serialize(const int16_t short_t);

        /*!
         * @brief This function serializes a unsigned long.
         * @param ulong_t The value of the unsigned long that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& serialize(const uint32_t ulong_t)
        {
            return serialize((int32_t)ulong_t);
        }

        /*!
         * @brief This function serializes a long.
         * @param long_t The value of the long that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serialize(const int32_t long_t);

        /*!
         * @brief This function serializes a unsigned long long.
         * @param ulonglong_t The value of the unsigned long long that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& serialize(const uint64_t ulonglong_t)
        {
            return serialize((int64_t)ulonglong_t);
        }

        /*!
         * @brief This function serializes a long long.
         * @param longlong_t The value of the long long that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serialize(const int64_t longlong_t);

        /*!
         * @brief This function serializes a float.
         * @param float_t The value of the float that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serialize(const float float_t);

        /*!
         * @brief This function serializes a double.
         * @param double_t The value of the double that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serialize(const double double_t);

        /*!
         * @brief This function serializes a boolean.
         * @param bool_t The value of the boolean that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serialize(const bool bool_t);

        /*!
         * @brief This function serializes a string.
         * @param bool_t The string that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serialize(const std::string &string_t);

        /*!
         * @brief This function template serializes an array.
         * @param array_t The array that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        inline FastCdr& serialize(const std::array<_T, _Size> &array_t)
        { return serializeArray(array_t.data(), array_t.size());}

        /*!
         * @brief This function template serializes a sequence.
         * @param vector_t The sequence that will be serialized in the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        template<class _T>
        FastCdr& serialize(const std::vector<_T> &vector_t)
        {
            FastCdr::state state(*this);

            serialize((int32_t)vector_t.size());

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
         * @brief This function serializes an array of octets.
         * @param octet_t The sequence of octets  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& serializeArray(const uint8_t *octet_t, size_t numElements)
        {
            return serializeArray((const char*)octet_t, numElements);
        }

        /*!
         * @brief This function serializes an array of characteres.
         * @param char_t The array of characteres  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serializeArray(const char *char_t, size_t numElements);

        /*!
         * @brief This function serializes an array of unsigned shorts.
         * @param ushort_t The array of unsigned shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& serializeArray(const uint16_t *ushort_t, size_t numElements)
        {
            return serializeArray((const int16_t*)ushort_t, numElements);
        }

        /*!
         * @brief This function serializes an array of shorts.
         * @param short_t The array of shorts  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serializeArray(const int16_t *short_t, size_t numElements);

        /*!
         * @brief This function serializes an array of unsigned longs.
         * @param ulong_t The array of unsigned longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& serializeArray(const uint32_t *ulong_t, size_t numElements)
        {
            return serializeArray((const int32_t*)ulong_t, numElements);
        }

        /*!
         * @brief This function serializes an array of longs.
         * @param long_t The array of longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serializeArray(const int32_t *long_t, size_t numElements);

        /*!
         * @brief This function serializes an array of unsigned long longs.
         * @param ulonglong_t The array of unsigned long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& serializeArray(const uint64_t *ulonglong_t, size_t numElements)
        {
            return serializeArray((const int64_t*)ulonglong_t, numElements);
        }

        /*!
         * @brief This function serializes an array of long longs.
         * @param longlong_t The array of  long longs  that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serializeArray(const int64_t *longlong_t, size_t numElements);
        
        /*!
         * @brief This function serializes an array of floats.
         * @param float_t The array of floats that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serializeArray(const float *float_t, size_t numElements);

        /*!
         * @brief This function serializes an array of doubles.
         * @param double_t The array of doubles that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to serialize in a position that exceed the internal memory size.
         */
        FastCdr& serializeArray(const double *double_t, size_t numElements);

        /*!
         * @brief This function deserializes an octet.
         * @param octet_t The variable that will store the octet read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& deserialize(uint8_t &octet_t)
        {
            return deserialize((char&)octet_t);
        }

        /*!
         * @brief This function deserializes a characteres.
         * @param char_t The variable that will store the character read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserialize(char &char_t);

        /*!
         * @brief This function deserializes a unsigned short.
         * @param ushort_t The variable that will store the unsigned short read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& deserialize(uint16_t &ushort_t)
        {
            return deserialize((int16_t&)ushort_t);
        }

        /*!
         * @brief This function deserializes a short.
         * @param short_t The variable that will store the short read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserialize(int16_t &short_t);

        /*!
         * @brief This function deserializes a unsigned long.
         * @param ulong_t The variable that will store the unsigned long read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& deserialize(uint32_t &ulong_t)
        {
            return deserialize((int32_t&)ulong_t);
        }

        /*!
         * @brief This function deserializes a long.
         * @param long_t The variable that will store the long read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserialize(int32_t &long_t);

        /*!
         * @brief This function deserializes a unsigned long long.
         * @param ulonglong_t The variable that will store the unsigned long long read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& deserialize(uint64_t &ulonglong_t)
        {
            return deserialize((int64_t&)ulonglong_t);
        }

        /*!
         * @brief This function deserializes a long long.
         * @param longlong_t The variable that will store the long long read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserialize(int64_t &longlong_t);

        /*!
         * @brief This function deserializes a float.
         * @param float_t The variable that will store the float read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserialize(float &float_t);

        /*!
         * @brief This function deserializes a double.
         * @param double_t The variable that will store the double read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserialize(double &double_t);

        /*!
         * @brief This function deserializes a boolean.
         * @param bool_t The variable that will store the boolean read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         * @exception BadParamException This exception is thrown trying to deserialize in an invalid value.
         */
        FastCdr& deserialize(bool &bool_t);

        /*!
         * @brief This function deserializes a string.
         * @param string_t The variable that will store the string read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserialize(std::string &string_t);

        /*!
         * @brief This function template deserializes an array.
         * @param array_t The variable that will store the array read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        inline FastCdr& deserialize(std::array<_T, _Size> &array_t)
        { return deserializeArray(array_t.data(), array_t.size());}

        /*!
         * @brief This function template deserializes a sequence.
         * @param vector_t The variable that will store the sequence read from the buffer.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T>
        FastCdr& deserialize(std::vector<_T> &vector_t)
        {
            uint32_t seqLength = 0;
            FastCdr::state state(*this);

            deserialize(seqLength);

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
         * @brief This function deserializes an array of octets.
         * @param octet_t The variable that will store the array of octets read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& deserializeArray(uint8_t *octet_t, size_t numElements)
        {
            return deserializeArray((char*)octet_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of characteres.
         * @param char_t The variable that will store the array of characteres read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserializeArray(char *char_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of unsigned short.
         * @param ushort_t The variable that will store the array of unsigned short read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& deserializeArray(uint16_t *ushort_t, size_t numElements)
        {
            return deserializeArray((int16_t*)ushort_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of short.
         * @param short_t The variable that will store the array of short read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserializeArray(int16_t *short_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of unsigned longt.
         * @param ulong_t The variable that will store the array of unsigned long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& deserializeArray(uint32_t *ulong_t, size_t numElements)
        {
            return deserializeArray((int32_t*)ulong_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of long.
         * @param long_t The variable that will store the array of long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserializeArray(int32_t *long_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of unsigned long long.
         * @param ulonglong_t The variable that will store the array of unsigned long long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        inline
        FastCdr& deserializeArray(uint64_t *ulonglong_t, size_t numElements)
        {
            return deserializeArray((int64_t*)ulonglong_t, numElements);
        }

        /*!
         * @brief This function deserializes an array of long long.
         * @param longlong_t The variable that will store the array of long long read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserializeArray(int64_t *longlong_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of float.
         * @param float_t The variable that will store the array of float read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserializeArray(float *float_t, size_t numElements);

        /*!
         * @brief This function deserializes an array of double.
         * @param double_t The variable that will store the array of double read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        FastCdr& deserializeArray(double *double_t, size_t numElements);

    private:

        /*!
         * @brief This function template detects the content type of the STD container array and serializes the array.
         * @param array_t The array that will be serialized in the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        FastCdr& serializeArray(const std::array<_T, _Size> *array_t, size_t numElements)
        {
            return serializeArray(array_t->data(), numElements * array_t->size());
        }

        /*!
         * @brief This function template detects the content type of the STD container array and deserializes the array.
         * @param array_t The variable that will store the array read from the buffer.
         * @param numElements Number of the elements in the array.
         * @return Reference to the eProsima::FastCdr object.
         * @exception NotEnoughMemoryException This exception is thrown trying to deserialize in a position that exceed the internal memory size.
         */
        template<class _T, size_t _Size>
        FastCdr& deserializeArray(std::array<_T, _Size> *array_t, size_t numElements)
        {
            return deserializeArray(array_t->data(), numElements * array_t->size());
        }

        bool resize(size_t minSizeInc);

        //! @brief Reference to the buffer that will be serialized/deserialized.
        FastBuffer &m_cdrBuffer;

        //! @brief The current position in the serialization/deserialization process.
        FastBuffer::iterator m_currentPosition;

        //! @brief The last position in the buffer;
        FastBuffer::iterator m_lastPosition;
    };
};

#endif //_CPP_FASTCDR_H_
