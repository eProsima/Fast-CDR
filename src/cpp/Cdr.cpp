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

#include <fastcdr/Cdr.h>
#include <fastcdr/exceptions/BadParamException.h>

#include <limits>

namespace eprosima {
namespace fastcdr {

using namespace exception;

#if FASTCDR_IS_BIG_ENDIAN_TARGET
const Cdr::Endianness Cdr::DEFAULT_ENDIAN = BIG_ENDIANNESS;
#else
const Cdr::Endianness Cdr::DEFAULT_ENDIAN = LITTLE_ENDIANNESS;
#endif // if FASTCDR_IS_BIG_ENDIAN_TARGET

constexpr uint16_t PID_SENTINEL = 0x3F02;
constexpr uint16_t PID_SENTINEL_LENGTH = 0x0;

constexpr uint8_t operator ""_8u(
        unsigned long long int value)
{
    return static_cast<uint8_t>(value);
}

inline size_t alignment_on_state(
        const FastBuffer::iterator& origin,
        const FastBuffer::iterator& offset,
        size_t dataSize)
{
    return (dataSize - ((offset - origin) % dataSize)) & (dataSize - 1);
}

Cdr::state::state(
        const Cdr& cdr)
    : offset_(cdr.offset_)
    , origin_(cdr.origin_)
    , m_swapBytes(cdr.m_swapBytes)
    , m_lastDataSize(cdr.m_lastDataSize)
    , next_member_id_(cdr.next_member_id_)
    , previous_enconding_(cdr.current_encoding_)
{
}

Cdr::state::state(
        const state& current_state)
    : offset_(current_state.offset_)
    , origin_(current_state.origin_)
    , m_swapBytes(current_state.m_swapBytes)
    , m_lastDataSize(current_state.m_lastDataSize)
    , next_member_id_(current_state.next_member_id_)
    , previous_enconding_(current_state.previous_enconding_)
{
}

bool Cdr::state::operator ==(
        const Cdr::state& other_state) const
{
    return
        other_state.offset_ == offset_ &&
        other_state.origin_ == origin_ &&
        other_state.m_swapBytes == m_swapBytes &&
        (0 == other_state.m_lastDataSize ||
        0 == m_lastDataSize ||
        other_state.m_lastDataSize == m_lastDataSize
        );
}

Cdr::Cdr(
        FastBuffer& cdrBuffer,
        const Endianness endianness,
        const CdrVersion cdr_version)
    : m_cdrBuffer(cdrBuffer)
    , cdr_version_(cdr_version)
    , m_endianness(endianness)
    , m_swapBytes(endianness == DEFAULT_ENDIAN ? false : true)
    , offset_(cdrBuffer.begin())
    , origin_(cdrBuffer.begin())
    , end_(cdrBuffer.end())
{
    switch (cdr_version_)
    {
        case CdrVersion::XCDRv2:
            begin_serialize_member_ = &Cdr::xcdr2_begin_serialize_member;
            end_serialize_member_ = &Cdr::xcdr2_end_serialize_member;
            begin_serialize_opt_member_ = &Cdr::xcdr2_begin_serialize_member;
            end_serialize_opt_member_ = &Cdr::xcdr2_end_serialize_member;
            begin_serialize_type_ = &Cdr::xcdr2_begin_serialize_type;
            end_serialize_type_ = &Cdr::xcdr2_end_serialize_type;
            deserialize_type_ = &Cdr::xcdr2_deserialize_type;
            align64_ = 4;
            break;
        case CdrVersion::XCDRv1:
            begin_serialize_member_ = &Cdr::xcdr1_begin_serialize_member;
            end_serialize_member_ = &Cdr::xcdr1_end_serialize_member;
            begin_serialize_opt_member_ = &Cdr::xcdr1_begin_serialize_opt_member;
            end_serialize_opt_member_ = &Cdr::xcdr1_end_serialize_opt_member;
            begin_serialize_type_ = &Cdr::xcdr1_begin_serialize_type;
            end_serialize_type_ = &Cdr::xcdr1_end_serialize_type;
            deserialize_type_ = &Cdr::xcdr1_deserialize_type;
            encoding_flag_ = EncodingAlgorithmFlag::PLAIN_CDR;
            current_encoding_ = EncodingAlgorithmFlag::PLAIN_CDR;
            break;
        default:
            encoding_flag_ = EncodingAlgorithmFlag::PLAIN_CDR;
            current_encoding_ = EncodingAlgorithmFlag::PLAIN_CDR;
            break;
    }
}

Cdr& Cdr::read_encapsulation()
{
    uint8_t dummy = 0, encapsulation = 0;
    state state_before_error(*this);

    try
    {
        // If it is DDS_CDR, the first step is to get the dummy byte.
        if (CdrVersion::CORBA_CDR < cdr_version_)
        {
            (*this) >> dummy;
            if (0 != dummy)
            {
                throw BadParamException("Unexpected non-zero initial byte received in Cdr::read_encapsulation");
            }
        }

        // Get the ecampsulation byte.
        (*this) >> encapsulation;


        // If it is a different endianness, make changes.
        const uint8_t endianness = encapsulation & 0x1_8u;
        if (m_endianness != endianness)
        {
            m_swapBytes = !m_swapBytes;
            m_endianness = endianness;
        }

        // Check encapsulationKind correctness
        const uint8_t encoding_flag = encapsulation & ~0x1_8u;
        switch (encoding_flag)
        {
            case EncodingAlgorithmFlag::PLAIN_CDR2:
            case EncodingAlgorithmFlag::DELIMIT_CDR2:
            case EncodingAlgorithmFlag::PL_CDR2:
                if (CdrVersion::XCDRv2 != cdr_version_)
                {
                    throw BadParamException(
                              "Unexpected encoding algorithm received in Cdr::read_encapsulation. XCDRv2 should be selected.");
                }
                break;
            case EncodingAlgorithmFlag::PL_CDR:
                if (CdrVersion::XCDRv1 != cdr_version_)
                {
                    throw BadParamException(
                              "Unexpected encoding algorithm received in Cdr::read_encapsulation. XCDRv1 should be selected");
                }
                break;
            case EncodingAlgorithmFlag::PLAIN_CDR:
                if (CdrVersion::XCDRv1 < cdr_version_)
                {
                    throw BadParamException(
                              "Unexpected encoding algorithm received in Cdr::read_encapsulation. XCDRv2 shouldn't be selected");
                }
                break;
            default:
                throw BadParamException("Unexpected encoding algorithm received in Cdr::read_encapsulation for DDS CDR");
        }

        encoding_flag_ = static_cast<EncodingAlgorithmFlag>(encoding_flag);
        current_encoding_ = encoding_flag_;

        if (CdrVersion::CORBA_CDR < cdr_version_)
        {
            (*this) >> m_options;
        }
    }
    catch (Exception& ex)
    {
        set_state(state_before_error);
        ex.raise();
    }

    resetAlignment();
    return *this;
}

Cdr& Cdr::serialize_encapsulation()
{
    uint8_t dummy = 0, encapsulation = 0;
    state state_before_error(*this);

    try
    {
        // If it is DDS_CDR, the first step is to serialize the dummy byte.
        if (CdrVersion::CORBA_CDR < cdr_version_)
        {
            (*this) << dummy;
        }

        // Construct encapsulation byte.
        encapsulation = (encoding_flag_ | m_endianness);

        // Serialize the encapsulation byte.
        (*this) << encapsulation;

        current_encoding_ = encoding_flag_;
    }
    catch (Exception& ex)
    {
        set_state(state_before_error);
        ex.raise();
    }

    try
    {
        if (CdrVersion::CORBA_CDR < cdr_version_)
        {
            (*this) << m_options;
        }
    }
    catch (Exception& ex)
    {
        set_state(state_before_error);
        ex.raise();
    }

    resetAlignment();
    return *this;
}

EncodingAlgorithmFlag Cdr::get_encoding_flag() const
{
    return encoding_flag_;
}

void Cdr::set_encoding_flag(
        EncodingAlgorithmFlag encoding_flag)
{
    assert(offset_ == m_cdrBuffer.begin());
    encoding_flag_ = encoding_flag;
}

uint16_t Cdr::getDDSCdrOptions() const
{
    return m_options;
}

void Cdr::setDDSCdrOptions(
        uint16_t options)
{
    m_options = options;
}

void Cdr::changeEndianness(
        Endianness endianness)
{
    if (m_endianness != endianness)
    {
        m_swapBytes = !m_swapBytes;
        m_endianness = endianness;
    }
}

bool Cdr::jump(
        size_t numBytes)
{
    bool returnedValue = false;

    if (((end_ - offset_) >= numBytes) || resize(numBytes))
    {
        offset_ += numBytes;
        m_lastDataSize = 0;
        returnedValue = true;
    }

    return returnedValue;
}

char* Cdr::getBufferPointer()
{
    return m_cdrBuffer.getBuffer();
}

char* Cdr::getCurrentPosition()
{
    return &offset_;
}

Cdr::state Cdr::get_state() const
{
    return Cdr::state(*this);
}

void Cdr::set_state(
        const state& current_state)
{
    offset_ >> current_state.offset_;
    origin_ >> current_state.origin_;
    m_swapBytes = current_state.m_swapBytes;
    m_lastDataSize = current_state.m_lastDataSize;
    next_member_id_ = current_state.next_member_id_;
}

void Cdr::reset()
{
    offset_ = m_cdrBuffer.begin();
    origin_ = m_cdrBuffer.begin();
    m_swapBytes = m_endianness == DEFAULT_ENDIAN ? false : true;
    m_lastDataSize = 0;
    encoding_flag_ = CdrVersion::XCDRv2 ==
            cdr_version_ ? EncodingAlgorithmFlag::PLAIN_CDR2 : EncodingAlgorithmFlag::PLAIN_CDR;
    current_encoding_ = encoding_flag_;
    next_member_id_ = MEMBER_ID_INVALID;
    m_options = 0;
}

bool Cdr::moveAlignmentForward(
        size_t numBytes)
{
    bool returnedValue = false;

    if (((end_ - origin_) >= numBytes) || resize(numBytes))
    {
        origin_ += numBytes;
        m_lastDataSize = 0;
        returnedValue = true;
    }

    return returnedValue;
}

bool Cdr::resize(
        size_t minSizeInc)
{
    if (m_cdrBuffer.resize(minSizeInc))
    {
        offset_ << m_cdrBuffer.begin();
        origin_ << m_cdrBuffer.begin();
        end_ = m_cdrBuffer.end();
        return true;
    }

    return false;
}

Cdr& Cdr::serialize(
        const char char_t)
{
    if (((end_ - offset_) >= sizeof(char_t)) || resize(sizeof(char_t)))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(char_t);

        offset_++ << char_t;
        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(
        const int16_t short_t)
{
    size_t align = alignment(sizeof(short_t));
    size_t sizeAligned = sizeof(short_t) + align;

    if (((end_ - offset_) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(short_t);

        // Align.
        makeAlign(align);

        if (m_swapBytes)
        {
            const char* dst = reinterpret_cast<const char*>(&short_t);

            offset_++ << dst[1];
            offset_++ << dst[0];
        }
        else
        {
            offset_ << short_t;
            offset_ += sizeof(short_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(
        const int16_t short_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(short_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(
        const int32_t long_t)
{
    size_t align = alignment(sizeof(long_t));
    size_t sizeAligned = sizeof(long_t) + align;

    if (((end_ - offset_) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(long_t);

        // Align.
        makeAlign(align);

        if (m_swapBytes)
        {
            const char* dst = reinterpret_cast<const char*>(&long_t);

            offset_++ << dst[3];
            offset_++ << dst[2];
            offset_++ << dst[1];
            offset_++ << dst[0];
        }
        else
        {
            offset_ << long_t;
            offset_ += sizeof(long_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(
        const int32_t long_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(long_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(
        const int64_t longlong_t)
{
    size_t align = alignment(align64_);
    size_t sizeAligned = sizeof(longlong_t) + align;

    if (((end_ - offset_) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = align64_;

        // Align.
        makeAlign(align);

        if (m_swapBytes)
        {
            const char* dst = reinterpret_cast<const char*>(&longlong_t);

            offset_++ << dst[7];
            offset_++ << dst[6];
            offset_++ << dst[5];
            offset_++ << dst[4];
            offset_++ << dst[3];
            offset_++ << dst[2];
            offset_++ << dst[1];
            offset_++ << dst[0];
        }
        else
        {
            offset_ << longlong_t;
            offset_ += sizeof(longlong_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(
        const int64_t longlong_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(longlong_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(
        const float float_t)
{
    size_t align = alignment(sizeof(float_t));
    size_t sizeAligned = sizeof(float_t) + align;

    if (((end_ - offset_) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(float_t);

        // Align.
        makeAlign(align);

        if (m_swapBytes)
        {
            const char* dst = reinterpret_cast<const char*>(&float_t);

            offset_++ << dst[3];
            offset_++ << dst[2];
            offset_++ << dst[1];
            offset_++ << dst[0];
        }
        else
        {
            offset_ << float_t;
            offset_ += sizeof(float_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(
        const float float_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(float_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(
        const double double_t)
{
    size_t align = alignment(align64_);
    size_t sizeAligned = sizeof(double_t) + align;

    if (((end_ - offset_) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = align64_;

        // Align.
        makeAlign(align);

        if (m_swapBytes)
        {
            const char* dst = reinterpret_cast<const char*>(&double_t);

            offset_++ << dst[7];
            offset_++ << dst[6];
            offset_++ << dst[5];
            offset_++ << dst[4];
            offset_++ << dst[3];
            offset_++ << dst[2];
            offset_++ << dst[1];
            offset_++ << dst[0];
        }
        else
        {
            offset_ << double_t;
            offset_ += sizeof(double_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(
        const double double_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(double_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(
        const long double ldouble_t)
{
    size_t align = alignment(align64_);
    size_t sizeAligned = sizeof(ldouble_t) + align;

    if (((end_ - offset_) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = align64_;

        // Align.
        makeAlign(align);

        if (m_swapBytes)
        {
#if FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
            __float128 tmp = ldouble_t;
            const char* dst = reinterpret_cast<const char*>(&tmp);
#else
            const char* dst = reinterpret_cast<const char*>(&ldouble_t);
#endif // FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
#if FASTCDR_HAVE_FLOAT128 || FASTCDR_SIZEOF_LONG_DOUBLE == 16
            offset_++ << dst[15];
            offset_++ << dst[14];
            offset_++ << dst[13];
            offset_++ << dst[12];
            offset_++ << dst[11];
            offset_++ << dst[10];
            offset_++ << dst[9];
            offset_++ << dst[8];
            offset_++ << dst[7];
            offset_++ << dst[6];
            offset_++ << dst[5];
            offset_++ << dst[4];
            offset_++ << dst[3];
            offset_++ << dst[2];
            offset_++ << dst[1];
            offset_++ << dst[0];
#else
#if FASTCDR_SIZEOF_LONG_DOUBLE == 8
            // Filled with 0's.
            offset_++ << static_cast<char>(0);
            offset_++ << static_cast<char>(0);
            offset_++ << static_cast<char>(0);
            offset_++ << static_cast<char>(0);
            offset_++ << static_cast<char>(0);
            offset_++ << static_cast<char>(0);
            offset_++ << static_cast<char>(0);
            offset_++ << static_cast<char>(0);
            offset_++ << dst[7];
            offset_++ << dst[6];
            offset_++ << dst[5];
            offset_++ << dst[4];
            offset_++ << dst[3];
            offset_++ << dst[2];
            offset_++ << dst[1];
            offset_++ << dst[0];
#else
#error unsupported long double type and no __float128 available
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 8
#endif // FASTCDR_HAVE_FLOAT128 || FASTCDR_SIZEOF_LONG_DOUBLE == 16
        }
        else
        {
#if FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
            __float128 tmp = ldouble_t;
            offset_ << tmp;
            offset_ += 16;
#else
#if FASTCDR_SIZEOF_LONG_DOUBLE == 8
            offset_ << static_cast<long double>(0);
            offset_ += sizeof(ldouble_t);
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 8
#if FASTCDR_SIZEOF_LONG_DOUBLE == 8 || FASTCDR_SIZEOF_LONG_DOUBLE == 16
            offset_ << ldouble_t;
            offset_ += sizeof(ldouble_t);
#else
#error unsupported long double type and no __float128 available
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 8 || FASTCDR_SIZEOF_LONG_DOUBLE == 16
#endif // FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(
        const long double ldouble_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(ldouble_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(
        const bool bool_t)
{
    if (((end_ - offset_) >= sizeof(uint8_t)) || resize(sizeof(uint8_t)))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(uint8_t);

        if (bool_t)
        {
            offset_++ << static_cast<uint8_t>(1);
        }
        else
        {
            offset_++ << static_cast<uint8_t>(0);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(
        const char* string_t)
{
    uint32_t length = 0;

    if (string_t != nullptr)
    {
        length = size_to_uint32(strlen(string_t)) + 1;
    }

    if (length > 0)
    {
        Cdr::state state_before_error(*this);
        serialize(length);

        if (((end_ - offset_) >= length) || resize(length))
        {
            // Save last datasize.
            m_lastDataSize = sizeof(uint8_t);

            offset_.memcopy(string_t, length);
            offset_ += length;
        }
        else
        {
            set_state(state_before_error);
            throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
        }
    }
    else
    {
        serialize(length);
    }

    serialized_member_size_ = SERIALIZED_MEMBER_SIZE;

    return *this;
}

Cdr& Cdr::serialize(
        const wchar_t* string_t)
{
    uint32_t bytesLength = 0;
    size_t wstrlen = 0;

    if (string_t != nullptr)
    {
        wstrlen = wcslen(string_t);
        bytesLength = size_to_uint32(wstrlen * 4);
    }

    if (bytesLength > 0)
    {
        Cdr::state state_(*this);
        serialize(size_to_uint32(wstrlen));

        if (((end_ - offset_) >= bytesLength) || resize(bytesLength))
        {
            // Save last datasize.
            m_lastDataSize = sizeof(uint32_t);

#if defined(_WIN32)
            serializeArray(string_t, wstrlen);
#else
            if (m_swapBytes)
            {
                const char* dst = reinterpret_cast<const char*>(string_t);
                const char* end = dst + bytesLength;

                for (; dst < end; dst += sizeof(*string_t))
                {
                    offset_++ << dst[3];
                    offset_++ << dst[2];
                    offset_++ << dst[1];
                    offset_++ << dst[0];
                }
            }
            else
            {
                offset_.memcopy(string_t, bytesLength);
                offset_ += bytesLength; // size on bytes
            }
#endif // if defined(_WIN32)
        }
        else
        {
            set_state(state_);
            throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
        }
    }
    else
    {
        serialize(bytesLength);
    }

    serialized_member_size_ = SERIALIZED_MEMBER_SIZE_4;

    return *this;
}

Cdr& Cdr::serialize(
        const char* string_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(string_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(
        const wchar_t* string_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(string_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serializeArray(
        const bool* bool_t,
        size_t numElements)
{
    size_t totalSize = sizeof(*bool_t) * numElements;

    if (((end_ - offset_) >= totalSize) || resize(totalSize))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*bool_t);

        for (size_t count = 0; count < numElements; ++count)
        {
            uint8_t value = 0;

            if (bool_t[count])
            {
                value = 1;
            }
            offset_++ << value;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(
        const char* char_t,
        size_t numElements)
{
    size_t totalSize = sizeof(*char_t) * numElements;

    if (((end_ - offset_) >= totalSize) || resize(totalSize))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*char_t);

        offset_.memcopy(char_t, totalSize);
        offset_ += totalSize;
        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(
        const int16_t* short_t,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    size_t align = alignment(sizeof(*short_t));
    size_t totalSize = sizeof(*short_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if (((end_ - offset_) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*short_t);

        // Align if there are any elements
        if (numElements)
        {
            makeAlign(align);
        }

        if (m_swapBytes)
        {
            const char* dst = reinterpret_cast<const char*>(short_t);
            const char* end = dst + totalSize;

            for (; dst < end; dst += sizeof(*short_t))
            {
                offset_++ << dst[1];
                offset_++ << dst[0];
            }
        }
        else
        {
            offset_.memcopy(short_t, totalSize);
            offset_ += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(
        const int16_t* short_t,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(short_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serializeArray(
        const int32_t* long_t,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    size_t align = alignment(sizeof(*long_t));
    size_t totalSize = sizeof(*long_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if (((end_ - offset_) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*long_t);

        // Align if there are any elements
        if (numElements)
        {
            makeAlign(align);
        }

        if (m_swapBytes)
        {
            const char* dst = reinterpret_cast<const char*>(long_t);
            const char* end = dst + totalSize;

            for (; dst < end; dst += sizeof(*long_t))
            {
                offset_++ << dst[3];
                offset_++ << dst[2];
                offset_++ << dst[1];
                offset_++ << dst[0];
            }
        }
        else
        {
            offset_.memcopy(long_t, totalSize);
            offset_ += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(
        const int32_t* long_t,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(long_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serializeArray(
        const wchar_t* wchar,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    for (size_t count = 0; count < numElements; ++count)
    {
        serialize(wchar[count]);
    }
    return *this;
}

Cdr& Cdr::serializeArray(
        const wchar_t* wchar,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(wchar, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serializeArray(
        const int64_t* longlong_t,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    size_t align = alignment(align64_);
    size_t totalSize = sizeof(*longlong_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if (((end_ - offset_) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = align64_;

        // Align if there are any elements
        if (numElements)
        {
            makeAlign(align);
        }

        if (m_swapBytes)
        {
            const char* dst = reinterpret_cast<const char*>(longlong_t);
            const char* end = dst + totalSize;

            for (; dst < end; dst += sizeof(*longlong_t))
            {
                offset_++ << dst[7];
                offset_++ << dst[6];
                offset_++ << dst[5];
                offset_++ << dst[4];
                offset_++ << dst[3];
                offset_++ << dst[2];
                offset_++ << dst[1];
                offset_++ << dst[0];
            }
        }
        else
        {
            offset_.memcopy(longlong_t, totalSize);
            offset_ += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(
        const int64_t* longlong_t,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(longlong_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serializeArray(
        const float* float_t,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    size_t align = alignment(sizeof(*float_t));
    size_t totalSize = sizeof(*float_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if (((end_ - offset_) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*float_t);

        // Align if there are any elements
        if (numElements)
        {
            makeAlign(align);
        }

        if (m_swapBytes)
        {
            const char* dst = reinterpret_cast<const char*>(float_t);
            const char* end = dst + totalSize;

            for (; dst < end; dst += sizeof(*float_t))
            {
                offset_++ << dst[3];
                offset_++ << dst[2];
                offset_++ << dst[1];
                offset_++ << dst[0];
            }
        }
        else
        {
            offset_.memcopy(float_t, totalSize);
            offset_ += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(
        const float* float_t,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(float_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serializeArray(
        const double* double_t,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    size_t align = alignment(align64_);
    size_t totalSize = sizeof(*double_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if (((end_ - offset_) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = align64_;

        // Align if there are any elements
        if (numElements)
        {
            makeAlign(align);
        }

        if (m_swapBytes)
        {
            const char* dst = reinterpret_cast<const char*>(double_t);
            const char* end = dst + totalSize;

            for (; dst < end; dst += sizeof(*double_t))
            {
                offset_++ << dst[7];
                offset_++ << dst[6];
                offset_++ << dst[5];
                offset_++ << dst[4];
                offset_++ << dst[3];
                offset_++ << dst[2];
                offset_++ << dst[1];
                offset_++ << dst[0];
            }
        }
        else
        {
            offset_.memcopy(double_t, totalSize);
            offset_ += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(
        const double* double_t,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(double_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serializeArray(
        const long double* ldouble_t,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    size_t align = alignment(align64_);
    // Fix for Windows ( long doubles only store 8 bytes )
    size_t totalSize = 16 * numElements; // sizeof(*ldouble_t)
    size_t sizeAligned = totalSize + align;

    if (((end_ - offset_) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = align64_;

        // Align if there are any elements
        if (numElements)
        {
            makeAlign(align);
        }

#if FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
        if (m_swapBytes)
        {
            for (size_t i = 0; i < numElements; ++i, ++ldouble_t)
            {
                __float128 tmp = *ldouble_t;
                const char* dst = reinterpret_cast<const char*>(&tmp);
                offset_++ << dst[15];
                offset_++ << dst[14];
                offset_++ << dst[13];
                offset_++ << dst[12];
                offset_++ << dst[11];
                offset_++ << dst[10];
                offset_++ << dst[9];
                offset_++ << dst[8];
                offset_++ << dst[7];
                offset_++ << dst[6];
                offset_++ << dst[5];
                offset_++ << dst[4];
                offset_++ << dst[3];
                offset_++ << dst[2];
                offset_++ << dst[1];
                offset_++ << dst[0];
            }
        }
        else
        {
            for (size_t i = 0; i < numElements; ++i, ++ldouble_t)
            {
                __float128 tmp = *ldouble_t;
                offset_ << tmp;
                offset_ += 16;
            }
        }
#else
#if FASTCDR_SIZEOF_LONG_DOUBLE == 8 || FASTCDR_SIZEOF_LONG_DOUBLE == 16
        if (m_swapBytes)
        {
            const char* dst = reinterpret_cast<const char*>(ldouble_t);
            const char* end = dst + totalSize;

            for (; dst < end; dst += sizeof(*ldouble_t))
            {
#if FASTCDR_SIZEOF_LONG_DOUBLE == 16
                offset_++ << dst[15];
                offset_++ << dst[14];
                offset_++ << dst[13];
                offset_++ << dst[12];
                offset_++ << dst[11];
                offset_++ << dst[10];
                offset_++ << dst[9];
                offset_++ << dst[8];
#else
                offset_++ << static_cast<char>(0);
                offset_++ << static_cast<char>(0);
                offset_++ << static_cast<char>(0);
                offset_++ << static_cast<char>(0);
                offset_++ << static_cast<char>(0);
                offset_++ << static_cast<char>(0);
                offset_++ << static_cast<char>(0);
                offset_++ << static_cast<char>(0);
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 16
                offset_++ << dst[7];
                offset_++ << dst[6];
                offset_++ << dst[5];
                offset_++ << dst[4];
                offset_++ << dst[3];
                offset_++ << dst[2];
                offset_++ << dst[1];
                offset_++ << dst[0];
            }
        }
        else
        {
#if FASTCDR_SIZEOF_LONG_DOUBLE == 16
            offset_.memcopy(ldouble_t, totalSize);
            offset_ += totalSize;
#else
            for (size_t i = 0; i < numElements; ++i)
            {
                offset_ << static_cast<long double>(0);
                offset_ += 8;
                offset_ << ldouble_t[i];
                offset_ += 8;
            }
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 16
        }
#else
#error unsupported long double type and no __float128 available
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 8 || FASTCDR_SIZEOF_LONG_DOUBLE == 16
#endif // FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(
        const long double* ldouble_t,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(ldouble_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(
        char& char_t)
{
    if ((end_ - offset_) >= sizeof(char_t))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(char_t);

        offset_++ >> char_t;
        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(
        int16_t& short_t)
{
    size_t align = alignment(sizeof(short_t));
    size_t sizeAligned = sizeof(short_t) + align;

    if ((end_ - offset_) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(short_t);

        // Align
        makeAlign(align);

        if (m_swapBytes)
        {
            char* dst = reinterpret_cast<char*>(&short_t);

            offset_++ >> dst[1];
            offset_++ >> dst[0];
        }
        else
        {
            offset_ >> short_t;
            offset_ += sizeof(short_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(
        int16_t& short_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(short_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(
        int32_t& long_t)
{
    size_t align = alignment(sizeof(long_t));
    size_t sizeAligned = sizeof(long_t) + align;

    if ((end_ - offset_) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(long_t);

        // Align
        makeAlign(align);

        if (m_swapBytes)
        {
            char* dst = reinterpret_cast<char*>(&long_t);

            offset_++ >> dst[3];
            offset_++ >> dst[2];
            offset_++ >> dst[1];
            offset_++ >> dst[0];
        }
        else
        {
            offset_ >> long_t;
            offset_ += sizeof(long_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(
        int32_t& long_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(long_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(
        int64_t& longlong_t)
{
    size_t align = alignment(align64_);
    size_t sizeAligned = sizeof(longlong_t) + align;

    if ((end_ - offset_) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = align64_;

        // Align.
        makeAlign(align);

        if (m_swapBytes)
        {
            char* dst = reinterpret_cast<char*>(&longlong_t);

            offset_++ >> dst[7];
            offset_++ >> dst[6];
            offset_++ >> dst[5];
            offset_++ >> dst[4];
            offset_++ >> dst[3];
            offset_++ >> dst[2];
            offset_++ >> dst[1];
            offset_++ >> dst[0];
        }
        else
        {
            offset_ >> longlong_t;
            offset_ += sizeof(longlong_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(
        int64_t& longlong_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(longlong_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(
        float& float_t)
{
    size_t align = alignment(sizeof(float_t));
    size_t sizeAligned = sizeof(float_t) + align;

    if ((end_ - offset_) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(float_t);

        // Align.
        makeAlign(align);

        if (m_swapBytes)
        {
            char* dst = reinterpret_cast<char*>(&float_t);

            offset_++ >> dst[3];
            offset_++ >> dst[2];
            offset_++ >> dst[1];
            offset_++ >> dst[0];
        }
        else
        {
            offset_ >> float_t;
            offset_ += sizeof(float_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(
        float& float_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(float_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(
        double& double_t)
{
    size_t align = alignment(align64_);
    size_t sizeAligned = sizeof(double_t) + align;

    if ((end_ - offset_) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = align64_;

        // Align.
        makeAlign(align);

        if (m_swapBytes)
        {
            char* dst = reinterpret_cast<char*>(&double_t);

            offset_++ >> dst[7];
            offset_++ >> dst[6];
            offset_++ >> dst[5];
            offset_++ >> dst[4];
            offset_++ >> dst[3];
            offset_++ >> dst[2];
            offset_++ >> dst[1];
            offset_++ >> dst[0];
        }
        else
        {
            offset_ >> double_t;
            offset_ += sizeof(double_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(
        double& double_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(double_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(
        long double& ldouble_t)
{
    size_t align = alignment(align64_);
    size_t sizeAligned = sizeof(ldouble_t) + align;

    if ((end_ - offset_) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = align64_;

        // Align.
        makeAlign(align);

        if (m_swapBytes)
        {
#if FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
            __float128 tmp = ldouble_t;
            char* dst = reinterpret_cast<char*>(&tmp);
#else
            char* dst = reinterpret_cast<char*>(&ldouble_t);
#endif // FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
#if FASTCDR_HAVE_FLOAT128 || FASTCDR_SIZEOF_LONG_DOUBLE == 16
            offset_++ >> dst[15];
            offset_++ >> dst[14];
            offset_++ >> dst[13];
            offset_++ >> dst[12];
            offset_++ >> dst[11];
            offset_++ >> dst[10];
            offset_++ >> dst[9];
            offset_++ >> dst[8];
            offset_++ >> dst[7];
            offset_++ >> dst[6];
            offset_++ >> dst[5];
            offset_++ >> dst[4];
            offset_++ >> dst[3];
            offset_++ >> dst[2];
            offset_++ >> dst[1];
            offset_++ >> dst[0];
#if FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
            ldouble_t = static_cast<long double>(tmp);
#endif // FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
#else
#if FASTCDR_SIZEOF_LONG_DOUBLE == 8
            offset_ += 8;
            offset_++ >> dst[7];
            offset_++ >> dst[6];
            offset_++ >> dst[5];
            offset_++ >> dst[4];
            offset_++ >> dst[3];
            offset_++ >> dst[2];
            offset_++ >> dst[1];
            offset_++ >> dst[0];
#else
#error unsupported long double type and no __float128 available
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 8
#endif // FASTCDR_HAVE_FLOAT128 || FASTCDR_SIZEOF_LONG_DOUBLE == 16
        }
        else
        {
#if FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
            __float128 tmp;
            offset_ >> tmp;
            offset_ += 16;
            ldouble_t = static_cast<long double>(tmp);
#else
#if FASTCDR_SIZEOF_LONG_DOUBLE == 8 || FASTCDR_SIZEOF_LONG_DOUBLE == 16
#if FASTCDR_SIZEOF_LONG_DOUBLE == 8
            offset_ += 8;
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 8
            offset_ >> ldouble_t;
            offset_ += sizeof(ldouble_t);
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 8 || FASTCDR_SIZEOF_LONG_DOUBLE == 16
#endif // FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(
        long double& ldouble_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(ldouble_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(
        bool& bool_t)
{
    uint8_t value = 0;

    if ((end_ - offset_) >= sizeof(uint8_t))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(uint8_t);

        offset_++ >> value;

        if (value == 1)
        {
            bool_t = true;
            return *this;
        }
        else if (value == 0)
        {
            bool_t = false;
            return *this;
        }

        throw BadParamException("Unexpected byte value in Cdr::deserialize(bool), expected 0 or 1");
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(
        char*& string_t)
{
    uint32_t length = 0;
    Cdr::state state_before_error(*this);

    deserialize(length);

    if (length == 0)
    {
        string_t = NULL;
        return *this;
    }
    else if ((end_ - offset_) >= length)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(uint8_t);

        // Allocate memory.
        string_t =
                reinterpret_cast<char*>(calloc(length + ((&offset_)[length - 1] == '\0' ? 0 : 1),
                sizeof(char)));
        memcpy(string_t, &offset_, length);
        offset_ += length;
        return *this;
    }

    set_state(state_before_error);
    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(
        wchar_t*& string_t)
{
    uint32_t length = 0;
    Cdr::state state_before_error(*this);

    deserialize(length);

    if (length == 0)
    {
        string_t = NULL;
        return *this;
    }
    else if ((end_ - offset_) >= length)
    {
        // Save last datasize.
        m_lastDataSize = 4;
        // Allocate memory.
        string_t = reinterpret_cast<wchar_t*>(calloc(length + 1, sizeof(wchar_t))); // WStrings never serialize terminating zero

#if defined(_WIN32)
        for (size_t idx = 0; idx < length; ++idx)
        {
            uint32_t temp;
            offset_ >> temp;
            string_t[idx] = static_cast<wchar_t>(temp);
            offset_ += 4;
        }
#else
        memcpy(string_t, &offset_, length * sizeof(wchar_t));
        offset_ += length * sizeof(wchar_t);
#endif // if defined(_WIN32)
        return *this;
    }

    set_state(state_before_error);
    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(
        char*& string_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(string_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(
        wchar_t*& string_t,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(string_t);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

const char* Cdr::readString(
        uint32_t& length)
{
    const char* returnedValue = "";
    state state_before_error(*this);

    *this >> length;

    if (length == 0)
    {
        return returnedValue;
    }
    else if ((end_ - offset_) >= length)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(uint8_t);

        returnedValue = &offset_;
        offset_ += length;
        if (returnedValue[length - 1] == '\0')
        {
            --length;
        }
        return returnedValue;
    }

    set_state(state_before_error);
    throw eprosima::fastcdr::exception::NotEnoughMemoryException(
              eprosima::fastcdr::exception::NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

const std::wstring Cdr::readWString(
        uint32_t& length)
{
    std::wstring returnedValue = L"";
    state state_(*this);

    *this >> length;
    uint32_t bytesLength = length * 4;

    if (bytesLength == 0)
    {
        return returnedValue;
    }
    else if ((end_ - offset_) >= bytesLength)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(uint32_t);

        returnedValue.resize(length);
        deserializeArray(const_cast<wchar_t*>(returnedValue.c_str()), length);
        if (returnedValue[length - 1] == L'\0')
        {
            --length;
            returnedValue.erase(length);
        }
        return returnedValue;
    }

    set_state(state_);
    throw eprosima::fastcdr::exception::NotEnoughMemoryException(
              eprosima::fastcdr::exception::NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(
        bool* bool_t,
        size_t numElements)
{
    size_t totalSize = sizeof(*bool_t) * numElements;

    if ((end_ - offset_) >= totalSize)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*bool_t);

        for (size_t count = 0; count < numElements; ++count)
        {
            uint8_t value = 0;
            offset_++ >> value;

            if (value == 1)
            {
                bool_t[count] = true;
            }
            else if (value == 0)
            {
                bool_t[count] = false;
            }
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(
        char* char_t,
        size_t numElements)
{
    size_t totalSize = sizeof(*char_t) * numElements;

    if ((end_ - offset_) >= totalSize)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*char_t);

        offset_.rmemcopy(char_t, totalSize);
        offset_ += totalSize;
        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(
        int16_t* short_t,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    size_t align = alignment(sizeof(*short_t));
    size_t totalSize = sizeof(*short_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if ((end_ - offset_) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*short_t);

        // Align if there are any elements
        if (numElements)
        {
            makeAlign(align);
        }

        if (m_swapBytes)
        {
            char* dst = reinterpret_cast<char*>(short_t);
            char* end = dst + totalSize;

            for (; dst < end; dst += sizeof(*short_t))
            {
                offset_++ >> dst[1];
                offset_++ >> dst[0];
            }
        }
        else
        {
            offset_.rmemcopy(short_t, totalSize);
            offset_ += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(
        int16_t* short_t,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(short_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserializeArray(
        int32_t* long_t,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    size_t align = alignment(sizeof(*long_t));
    size_t totalSize = sizeof(*long_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if ((end_ - offset_) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*long_t);

        // Align if there are any elements
        if (numElements)
        {
            makeAlign(align);
        }

        if (m_swapBytes)
        {
            char* dst = reinterpret_cast<char*>(long_t);
            char* end = dst + totalSize;

            for (; dst < end; dst += sizeof(*long_t))
            {
                offset_++ >> dst[3];
                offset_++ >> dst[2];
                offset_++ >> dst[1];
                offset_++ >> dst[0];
            }
        }
        else
        {
            offset_.rmemcopy(long_t, totalSize);
            offset_ += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(
        int32_t* long_t,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(long_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserializeArray(
        wchar_t* wchar,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    uint32_t value;
    for (size_t count = 0; count < numElements; ++count)
    {
        deserialize(value);
        wchar[count] = static_cast<wchar_t>(value);
    }
    return *this;
}

Cdr& Cdr::deserializeArray(
        wchar_t* wchar,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(wchar, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserializeArray(
        int64_t* longlong_t,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    size_t align = alignment(align64_);
    size_t totalSize = sizeof(*longlong_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if ((end_ - offset_) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = align64_;

        // Align if there are any elements
        if (numElements)
        {
            makeAlign(align);
        }

        if (m_swapBytes)
        {
            char* dst = reinterpret_cast<char*>(longlong_t);
            char* end = dst + totalSize;

            for (; dst < end; dst += sizeof(*longlong_t))
            {
                offset_++ >> dst[7];
                offset_++ >> dst[6];
                offset_++ >> dst[5];
                offset_++ >> dst[4];
                offset_++ >> dst[3];
                offset_++ >> dst[2];
                offset_++ >> dst[1];
                offset_++ >> dst[0];
            }
        }
        else
        {
            offset_.rmemcopy(longlong_t, totalSize);
            offset_ += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(
        int64_t* longlong_t,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(longlong_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserializeArray(
        float* float_t,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    size_t align = alignment(sizeof(*float_t));
    size_t totalSize = sizeof(*float_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if ((end_ - offset_) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*float_t);

        // Align if there are any elements
        if (numElements)
        {
            makeAlign(align);
        }

        if (m_swapBytes)
        {
            char* dst = reinterpret_cast<char*>(float_t);
            char* end = dst + totalSize;

            for (; dst < end; dst += sizeof(*float_t))
            {
                offset_++ >> dst[3];
                offset_++ >> dst[2];
                offset_++ >> dst[1];
                offset_++ >> dst[0];
            }
        }
        else
        {
            offset_.rmemcopy(float_t, totalSize);
            offset_ += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(
        float* float_t,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(float_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserializeArray(
        double* double_t,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    size_t align = alignment(align64_);
    size_t totalSize = sizeof(*double_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if ((end_ - offset_) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = align64_;

        // Align if there are any elements
        if (numElements)
        {
            makeAlign(align);
        }

        if (m_swapBytes)
        {
            char* dst = reinterpret_cast<char*>(double_t);
            char* end = dst + totalSize;

            for (; dst < end; dst += sizeof(*double_t))
            {
                offset_++ >> dst[7];
                offset_++ >> dst[6];
                offset_++ >> dst[5];
                offset_++ >> dst[4];
                offset_++ >> dst[3];
                offset_++ >> dst[2];
                offset_++ >> dst[1];
                offset_++ >> dst[0];
            }
        }
        else
        {
            offset_.rmemcopy(double_t, totalSize);
            offset_ += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(
        double* double_t,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(double_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserializeArray(
        long double* ldouble_t,
        size_t numElements)
{
    if (numElements == 0)
    {
        return *this;
    }

    size_t align = alignment(align64_);
    // Fix for Windows ( long doubles only store 8 bytes )
    size_t totalSize = 16 * numElements;
    size_t sizeAligned = totalSize + align;

    if ((end_ - offset_) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = align64_;

        // Align if there are any elements
        if (numElements)
        {
            makeAlign(align);
        }

#if FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16
        if (m_swapBytes)
        {
            for (size_t i = 0; i < numElements; ++i)
            {
                __float128 tmp;
                char* dst = reinterpret_cast<char*>(&tmp);
                offset_++ >> dst[15];
                offset_++ >> dst[14];
                offset_++ >> dst[13];
                offset_++ >> dst[12];
                offset_++ >> dst[11];
                offset_++ >> dst[10];
                offset_++ >> dst[9];
                offset_++ >> dst[8];
                offset_++ >> dst[7];
                offset_++ >> dst[6];
                offset_++ >> dst[5];
                offset_++ >> dst[4];
                offset_++ >> dst[3];
                offset_++ >> dst[2];
                offset_++ >> dst[1];
                offset_++ >> dst[0];
                ldouble_t[i] = static_cast<long double>(tmp);
            }
        }
        else
        {
            for (size_t i = 0; i < numElements; ++i)
            {
                __float128 tmp;
                offset_ >> tmp;
                offset_ += 16;
                ldouble_t[i] = static_cast<long double>(tmp);
            }
        }
#else
#if FASTCDR_SIZEOF_LONG_DOUBLE == 8 || FASTCDR_SIZEOF_LONG_DOUBLE == 16
        if (m_swapBytes)
        {
            char* dst = reinterpret_cast<char*>(ldouble_t);
            char* end = dst + numElements * sizeof(*ldouble_t);

            for (; dst < end; dst += sizeof(*ldouble_t))
            {
#if FASTCDR_SIZEOF_LONG_DOUBLE == 16
                offset_++ >> dst[15];
                offset_++ >> dst[14];
                offset_++ >> dst[13];
                offset_++ >> dst[12];
                offset_++ >> dst[11];
                offset_++ >> dst[10];
                offset_++ >> dst[9];
                offset_++ >> dst[8];
#else
                offset_ += 8;
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 16
                offset_++ >> dst[7];
                offset_++ >> dst[6];
                offset_++ >> dst[5];
                offset_++ >> dst[4];
                offset_++ >> dst[3];
                offset_++ >> dst[2];
                offset_++ >> dst[1];
                offset_++ >> dst[0];
            }
        }
        else
        {
#if FASTCDR_SIZEOF_LONG_DOUBLE == 16
            offset_.rmemcopy(ldouble_t, totalSize);
            offset_ += totalSize;
#else
            for (size_t i = 0; i < numElements; ++i)
            {
                offset_ += 8; // ignore first 8 bytes
                offset_ >> ldouble_t[i];
                offset_ += 8;
            }
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 16
        }
#endif // FASTCDR_SIZEOF_LONG_DOUBLE == 8 || FASTCDR_SIZEOF_LONG_DOUBLE == 16
#endif // FASTCDR_HAVE_FLOAT128 && FASTCDR_SIZEOF_LONG_DOUBLE < 16

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(
        long double* ldouble_t,
        size_t numElements,
        Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(ldouble_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch (Exception& ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serializeBoolSequence(
        const std::vector<bool>& vector_t)
{
    state state_before_error(*this);

    *this << static_cast<int32_t>(vector_t.size());

    size_t totalSize = vector_t.size() * sizeof(bool);

    if (((end_ - offset_) >= totalSize) || resize(totalSize))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(bool);

        for (size_t count = 0; count < vector_t.size(); ++count)
        {
            uint8_t value = 0;
            std::vector<bool>::const_reference ref = vector_t[count];

            if (ref)
            {
                value = 1;
            }
            offset_++ << value;
        }
    }
    else
    {
        set_state(state_before_error);
        throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
    }

    return *this;
}

Cdr& Cdr::deserializeBoolSequence(
        std::vector<bool>& vector_t)
{
    uint32_t seqLength = 0;
    state state_before_error(*this);

    *this >> seqLength;

    size_t totalSize = seqLength * sizeof(bool);

    if ((end_ - offset_) >= totalSize)
    {
        vector_t.resize(seqLength);
        // Save last datasize.
        m_lastDataSize = sizeof(bool);

        for (uint32_t count = 0; count < seqLength; ++count)
        {
            uint8_t value = 0;
            offset_++ >> value;

            if (value == 1)
            {
                vector_t[count] = true;
            }
            else if (value == 0)
            {
                vector_t[count] = false;
            }
            else
            {
                throw BadParamException("Unexpected byte value in Cdr::deserializeBoolSequence, expected 0 or 1");
            }
        }
    }
    else
    {
        set_state(state_before_error);
        throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
    }

    return *this;
}

Cdr& Cdr::deserializeStringSequence(
        std::string*& sequence_t,
        size_t& numElements)
{
    uint32_t seqLength = 0;
    state state_before_error(*this);

    deserialize(seqLength);

    try
    {
        sequence_t = new std::string[seqLength];
        deserializeArray(sequence_t, seqLength);
    }
    catch (eprosima::fastcdr::exception::Exception& ex)
    {
        delete [] sequence_t;
        sequence_t = NULL;
        set_state(state_before_error);
        ex.raise();
    }

    numElements = seqLength;
    return *this;
}

Cdr& Cdr::deserializeWStringSequence(
        std::wstring*& sequence_t,
        size_t& numElements)
{
    uint32_t seqLength = 0;
    state state_before_error(*this);

    deserialize(seqLength);

    try
    {
        sequence_t = new std::wstring[seqLength];
        deserializeArray(sequence_t, seqLength);
    }
    catch (eprosima::fastcdr::exception::Exception& ex)
    {
        delete [] sequence_t;
        sequence_t = NULL;
        set_state(state_before_error);
        ex.raise();
    }

    numElements = seqLength;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// XCDR extensions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Cdr::xcdr1_serialize_short_member_header(
        const MemberId& member_id)
{
    assert(0x3F00 >= member_id.id);

    makeAlign(alignment(4));

    uint16_t flags_and_member_id = (member_id.must_understand ? 0x4000 : 0x0) | static_cast<uint16_t>(member_id.id);
    serialize(flags_and_member_id);
    uint16_t size = 0;
    serialize(size);
    resetAlignment();
}

void Cdr::xcdr1_end_short_member_header(
        const MemberId&,
        size_t member_serialized_size)
{
    assert(std::numeric_limits<uint16_t>::max() >= member_serialized_size );
    makeAlign(alignment(4));
    jump(sizeof(uint16_t));
    uint16_t size = static_cast<uint16_t>(member_serialized_size);
    serialize(size);
}

void Cdr::xcdr1_serialize_long_member_header(
        const MemberId& member_id)
{
    makeAlign(alignment(4));

    uint16_t flags_and_extended_pid = (member_id.must_understand ? 0x4000 : 0x0) | static_cast<uint16_t>(0x3F01);
    serialize(flags_and_extended_pid);
    uint16_t size = 8;
    serialize(size);
    uint32_t id = member_id.id;
    serialize(id);
    uint32_t msize = 0;
    serialize(msize);
    resetAlignment();
}

void Cdr::xcdr1_end_long_member_header(
        const MemberId&,
        size_t member_serialized_size)
{
    makeAlign(alignment(4));
    jump(sizeof(uint16_t) + sizeof(uint32_t));
    uint32_t msize = static_cast<uint32_t>(member_serialized_size);
    serialize(msize);
}

void Cdr::xcdr1_change_to_short_member_header(
        const MemberId& member_id,
        size_t member_serialized_size)
{
    makeAlign(alignment(4));

    uint16_t flags_and_member_id = (member_id.must_understand ? 0x4000 : 0x0) |
            static_cast<uint16_t>(member_id.id);
    serialize(flags_and_member_id);
    uint16_t size = static_cast<uint16_t>(member_serialized_size);
    serialize(size);
    memmove(&offset_, &offset_ + 8, member_serialized_size);
}

void Cdr::xcdr1_change_to_long_member_header(
        const MemberId& member_id,
        size_t member_serialized_size)
{
    size_t align = alignment(4);

    if (0 < (end_ - offset_ - member_serialized_size - 11 - align))
    {
        memmove(&offset_ + 12, &offset_ + 4, member_serialized_size);
    }
    else
    {
        throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
    }
    makeAlign(align);
    uint16_t flags_and_extended_pid = (member_id.must_understand ? 0x4000 : 0x0) | static_cast<uint16_t>(0x3F01);
    serialize(flags_and_extended_pid);
    uint16_t size = 8;
    serialize(size);
    uint32_t id = member_id.id;
    serialize(id);
    uint32_t msize = static_cast<uint32_t>(member_serialized_size);
    serialize(msize);
}

bool Cdr::xcdr1_deserialize_member_header(
        MemberId& member_id,
        Cdr::state& current_state)
{
    bool ret_value = true;
    makeAlign(alignment(4));
    uint16_t flags_and_member_id = 0;
    deserialize(flags_and_member_id);
    member_id.must_understand = (flags_and_member_id & 0x4000);
    uint16_t id = (flags_and_member_id & 0x3FFF);


    if (0x3F01 > id)
    {
        member_id.id = id;
        uint16_t size = 0;
        deserialize(size);
        current_state.member_size_ = size;
        current_state.header_serialized_ = XCdrHeaderSelection::SHORT_HEADER;
        resetAlignment();
    }
    else if (0x3F01 == id) // PID_EXTENDED
    {
        uint16_t size = 0;
        deserialize(size);
        assert(8 == size); // TODO Throw exception
        uint32_t mid = 0;
        deserialize(mid);
        member_id.id = mid;
        deserialize(current_state.member_size_);
        current_state.header_serialized_ = XCdrHeaderSelection::LONG_HEADER;
        resetAlignment();
    }
    else if (0x3F02 == id) // PID_EXTENDED
    {
        uint16_t size = 0;
        deserialize(size);
        assert(0 == size);
        current_state.member_size_ = size;
        ret_value = false;
    }

    return ret_value;
}

void Cdr::xcdr2_serialize_short_member_header(
        const MemberId& member_id)
{
    assert(0x10000000 > member_id.id);

    uint32_t flags_and_member_id = (member_id.must_understand ? 0x80000000 : 0x0) | member_id.id;
    serialize(flags_and_member_id);
}

void Cdr::xcdr2_end_short_member_header(
        const MemberId& member_id,
        size_t member_serialized_size)
{
    assert(8 >= member_serialized_size);

    uint32_t lc = 0;
    switch (member_serialized_size)
    {
        case 2:
            lc = 0x10000000;
            break;
        case 4:
            lc = 0x20000000;
            break;
        case 8:
            lc = 0x30000000;
            break;
        default:
            break;
    }
    uint32_t flags_and_member_id = (member_id.must_understand ? 0x80000000 : 0x0) | lc | member_id.id;
    serialize(flags_and_member_id);
}

void Cdr::xcdr2_serialize_long_member_header(
        const MemberId& member_id)
{
    assert(0x10000000 > member_id.id);

    uint32_t flags_and_member_id = (member_id.must_understand ? 0x80000000 : 0x0) | member_id.id;
    serialize(flags_and_member_id);
    uint32_t size = 0;
    serialize(size);
}

void Cdr::xcdr2_end_long_member_header(
        const MemberId& member_id,
        size_t member_serialized_size)
{
    uint32_t lc = 0 == member_serialized_size ?
            (SERIALIZED_MEMBER_SIZE_4 == serialized_member_size_ ? 0x60000000  : 0x50000000)
        : 0x40000000;
    uint32_t flags_and_member_id = (member_id.must_understand ? 0x80000000 : 0x0) | lc | member_id.id;
    serialize(flags_and_member_id);
    if (0 < member_serialized_size)
    {
        uint32_t size = static_cast<uint32_t>(member_serialized_size);
        serialize(size);
    }
}

void Cdr::xcdr2_change_to_short_member_header(
        const MemberId& member_id,
        size_t member_serialized_size)
{
    assert(8 >= member_serialized_size);

    uint32_t lc = ((member_serialized_size - 1) << 28) & 0x70000000;
    uint32_t flags_and_member_id = (member_id.must_understand ? 0x80000000 : 0x0) | lc | member_id.id;
    serialize(flags_and_member_id);
    memmove(&offset_, &offset_ + 4, member_serialized_size);
}

void Cdr::xcdr2_change_to_long_member_header(
        const MemberId& member_id,
        size_t member_serialized_size)
{
    if (0 < (end_ - offset_ - member_serialized_size - 7))
    {
        memmove(&offset_ + 8, &offset_ + 4, member_serialized_size);
    }
    else
    {
        throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
    }
    uint32_t lc = 0x40000000;
    uint32_t flags_and_member_id = (member_id.must_understand ? 0x80000000 : 0x0) | lc | member_id.id;
    serialize(flags_and_member_id);
    uint32_t size = static_cast<uint32_t>(member_serialized_size);
    serialize(size);
}

void Cdr::xcdr2_shrink_to_long_member_header(
        const MemberId& member_id,
        const FastBuffer::iterator& offset)
{
    memmove(&offset_ + 4, &offset_ + 8, offset - offset_ - 8);
    uint32_t lc = SERIALIZED_MEMBER_SIZE_4 == serialized_member_size_ ? 0x60000000  : 0x50000000;
    uint32_t flags_and_member_id = (member_id.must_understand ? 0x80000000 : 0x0) | lc | member_id.id;
    serialize(flags_and_member_id);
}

Cdr& Cdr::xcdr1_begin_serialize_member(
        const MemberId& member_id,
        bool is_present,
        Cdr::state& current_state,
        Cdr::XCdrHeaderSelection header_selection)
{
    static_cast<void>(current_state);
    static_cast<void>(is_present);
    assert(is_present);
    assert(MEMBER_ID_INVALID != member_id);
    assert(MEMBER_ID_INVALID == next_member_id_ || member_id == next_member_id_);
    assert(current_state == Cdr::state(*this));
    assert(EncodingAlgorithmFlag::PLAIN_CDR == current_encoding_ ||
            EncodingAlgorithmFlag::PL_CDR == current_encoding_);

    if (EncodingAlgorithmFlag::PL_CDR == current_encoding_)
    {
        if (0x3F00 >= member_id.id)
        {
            switch (header_selection)
            {
                case XCdrHeaderSelection::AUTO_WITH_SHORT_HEADER_BY_DEFAULT:
                case XCdrHeaderSelection::SHORT_HEADER:
                    xcdr1_serialize_short_member_header(member_id);
                    current_state.header_serialized_ = XCdrHeaderSelection::SHORT_HEADER;
                    break;
                case XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT:
                case XCdrHeaderSelection::LONG_HEADER:
                    xcdr1_serialize_long_member_header(member_id);
                    current_state.header_serialized_ = XCdrHeaderSelection::LONG_HEADER;
                    break;
            }
        }
        else
        {
            switch (header_selection)
            {
                case XCdrHeaderSelection::AUTO_WITH_SHORT_HEADER_BY_DEFAULT:
                case XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT:
                case XCdrHeaderSelection::LONG_HEADER:
                    xcdr1_serialize_long_member_header(member_id);
                    current_state.header_serialized_ = XCdrHeaderSelection::LONG_HEADER;
                    break;
                default:
                    throw BadParamException(
                              "Cannot encode XCDR1 ShortMemberHeader when member_id is bigger than 0x3F00");
            }
        }
        current_state.header_selection_ = header_selection;
    }
    current_state.next_member_id_ = member_id;
    next_member_id_ = MEMBER_ID_INVALID;

    return *this;
}

Cdr& Cdr::xcdr1_end_serialize_member(
        const Cdr::state& current_state)
{
    assert(MEMBER_ID_INVALID != current_state.next_member_id_);
    assert(EncodingAlgorithmFlag::PLAIN_CDR == current_encoding_ ||
            EncodingAlgorithmFlag::PL_CDR == current_encoding_);

    if (EncodingAlgorithmFlag::PL_CDR == current_encoding_)
    {
        const size_t member_serialized_size = offset_ - origin_;
        set_state(current_state);
        if (member_serialized_size > std::numeric_limits<uint16_t>::max())
        {
            switch (current_state.header_serialized_)
            {
                case XCdrHeaderSelection::SHORT_HEADER:
                    if (AUTO_WITH_SHORT_HEADER_BY_DEFAULT == current_state.header_selection_)
                    {
                        xcdr1_change_to_long_member_header(current_state.next_member_id_, member_serialized_size);
                    }
                    else
                    {
                        assert(false);         // TODO throw exception
                    }
                    break;
                case XCdrHeaderSelection::LONG_HEADER:
                    xcdr1_end_long_member_header(current_state.next_member_id_, member_serialized_size);
                    break;
                default:
                    assert(false);         // header_serialized_ must have only SHORT_HEADER or LONG_HEADER
            }
        }
        else
        {
            switch (current_state.header_serialized_)
            {
                case XCdrHeaderSelection::SHORT_HEADER:
                    xcdr1_end_short_member_header(current_state.next_member_id_, member_serialized_size);
                    break;
                case XCdrHeaderSelection::LONG_HEADER:
                    if (LONG_HEADER == current_state.header_selection_ ||
                            0x3F00 < current_state.next_member_id_.id)
                    {
                        xcdr1_end_long_member_header(current_state.next_member_id_, member_serialized_size);
                    }
                    else if (AUTO_WITH_LONG_HEADER_BY_DEFAULT == current_state.header_selection_)
                    {
                        xcdr1_change_to_short_member_header(current_state.next_member_id_, member_serialized_size);
                    }
                    break;
                default:
                    assert(false);         // header_serialized_ must have only SHORT_HEADER or LONG_HEADER
            }
        }
        jump(member_serialized_size);
    }

    next_member_id_ = MEMBER_ID_INVALID;

    return *this;
}

Cdr& Cdr::xcdr1_begin_serialize_opt_member(
        const MemberId& member_id,
        bool is_present,
        Cdr::state& current_state,
        Cdr::XCdrHeaderSelection header_selection)
{
    static_cast<void>(current_state);
    assert(MEMBER_ID_INVALID != member_id);
    assert(MEMBER_ID_INVALID == next_member_id_ || member_id == next_member_id_);
    assert(current_state == Cdr::state(*this));
    assert(EncodingAlgorithmFlag::PLAIN_CDR == current_encoding_ ||
            EncodingAlgorithmFlag::PL_CDR == current_encoding_);

    if (is_present || EncodingAlgorithmFlag::PL_CDR != current_encoding_)
    {
        if (0x3F00 >= member_id.id)
        {
            switch (header_selection)
            {
                case XCdrHeaderSelection::AUTO_WITH_SHORT_HEADER_BY_DEFAULT:
                case XCdrHeaderSelection::SHORT_HEADER:
                    xcdr1_serialize_short_member_header(member_id);
                    current_state.header_serialized_ = XCdrHeaderSelection::SHORT_HEADER;
                    break;
                case XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT:
                case XCdrHeaderSelection::LONG_HEADER:
                    xcdr1_serialize_long_member_header(member_id);
                    current_state.header_serialized_ = XCdrHeaderSelection::LONG_HEADER;
                    break;
            }
        }
        else
        {
            switch (header_selection)
            {
                case XCdrHeaderSelection::AUTO_WITH_SHORT_HEADER_BY_DEFAULT:
                case XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT:
                case XCdrHeaderSelection::LONG_HEADER:
                    xcdr1_serialize_long_member_header(member_id);
                    current_state.header_serialized_ = XCdrHeaderSelection::LONG_HEADER;
                    break;
                default:
                    throw BadParamException(
                              "Cannot encode XCDR1 ShortMemberHeader when member_id is bigger than 0x3F00");
            }
        }
        current_state.header_selection_ = header_selection;
    }
    current_state.member_size_ = is_present ? 1 : 0;
    current_state.next_member_id_ = member_id;
    next_member_id_ = MEMBER_ID_INVALID;

    return *this;
}

Cdr& Cdr::xcdr1_end_serialize_opt_member(
        const Cdr::state& current_state)
{
    assert(MEMBER_ID_INVALID != current_state.next_member_id_);
    assert(EncodingAlgorithmFlag::PLAIN_CDR == current_encoding_ ||
            EncodingAlgorithmFlag::PL_CDR == current_encoding_);

    if (0 < current_state.member_size_)
    {
        const size_t member_serialized_size = offset_ - origin_;
        set_state(current_state);
        if (member_serialized_size > std::numeric_limits<uint16_t>::max())
        {
            switch (current_state.header_serialized_)
            {
                case XCdrHeaderSelection::SHORT_HEADER:
                    if (AUTO_WITH_SHORT_HEADER_BY_DEFAULT == current_state.header_selection_)
                    {
                        xcdr1_change_to_long_member_header(current_state.next_member_id_, member_serialized_size);
                    }
                    else
                    {
                        assert(false);         // TODO throw exception
                    }
                    break;
                case XCdrHeaderSelection::LONG_HEADER:
                    xcdr1_end_long_member_header(current_state.next_member_id_, member_serialized_size);
                    break;
                default:
                    assert(false);         // header_serialized_ must have only SHORT_HEADER or LONG_HEADER
            }
        }
        else
        {
            switch (current_state.header_serialized_)
            {
                case XCdrHeaderSelection::SHORT_HEADER:
                    xcdr1_end_short_member_header(current_state.next_member_id_, member_serialized_size);
                    break;
                case XCdrHeaderSelection::LONG_HEADER:
                    if (LONG_HEADER == current_state.header_selection_ ||
                            0x3F00 < current_state.next_member_id_.id)
                    {
                        xcdr1_end_long_member_header(current_state.next_member_id_, member_serialized_size);
                    }
                    else if (AUTO_WITH_LONG_HEADER_BY_DEFAULT == current_state.header_selection_)
                    {
                        xcdr1_change_to_short_member_header(current_state.next_member_id_, member_serialized_size);
                    }
                    break;
                default:
                    assert(false);         // header_serialized_ must have only SHORT_HEADER or LONG_HEADER
            }
        }
        jump(member_serialized_size);
    }
    else if (EncodingAlgorithmFlag::PL_CDR != encoding_flag_)
    {
        // Reset state to POP(origin=0) because before serializing member the algorithm did PUSH(origin=0).
        auto offset = offset_;
        set_state(current_state);
        offset_ = offset;
    }

    next_member_id_ = MEMBER_ID_INVALID;

    return *this;
}

Cdr& Cdr::xcdr2_begin_serialize_member(
        const MemberId& member_id,
        bool is_present,
        Cdr::state& current_state,
        XCdrHeaderSelection header_selection)
{
    static_cast<void>(current_state);
    assert(MEMBER_ID_INVALID != member_id);
    assert(MEMBER_ID_INVALID == next_member_id_ || member_id == next_member_id_);
    assert(current_state == Cdr::state(*this));
    assert(EncodingAlgorithmFlag::PLAIN_CDR2 == current_encoding_ ||
            EncodingAlgorithmFlag::DELIMIT_CDR2 == current_encoding_ ||
            EncodingAlgorithmFlag::PL_CDR2 == current_encoding_);

    if (is_present && EncodingAlgorithmFlag::PL_CDR2 == current_encoding_)
    {
        assert(0x10000000 > member_id.id);
        switch (header_selection)
        {
            case XCdrHeaderSelection::SHORT_HEADER:
            case XCdrHeaderSelection::AUTO_WITH_SHORT_HEADER_BY_DEFAULT:
                xcdr2_serialize_short_member_header(member_id);
                current_state.header_serialized_ = XCdrHeaderSelection::SHORT_HEADER;
                break;
            case XCdrHeaderSelection::LONG_HEADER:
            case XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT:
                xcdr2_serialize_long_member_header(member_id);
                current_state.header_serialized_ = XCdrHeaderSelection::LONG_HEADER;
                break;
        }
        current_state.header_selection_ = header_selection;
    }

    current_state.member_size_ = is_present ? 1 : 0;
    current_state.next_member_id_ = member_id;
    next_member_id_ = MEMBER_ID_INVALID;

    return *this;
}

Cdr& Cdr::xcdr2_end_serialize_member(
        const Cdr::state& current_state)
{
    assert(MEMBER_ID_INVALID != current_state.next_member_id_);
    assert(EncodingAlgorithmFlag::PLAIN_CDR2 == current_encoding_ ||
            EncodingAlgorithmFlag::DELIMIT_CDR2 == current_encoding_ ||
            EncodingAlgorithmFlag::PL_CDR2 == current_encoding_);

    if (0 < current_state.member_size_ && EncodingAlgorithmFlag::PL_CDR2 == current_encoding_)
    {
        auto last_offset = offset_;
        set_state(current_state);
        makeAlign(alignment(sizeof(uint32_t)));
        if (NO_SERIALIZED_MEMBER_SIZE == serialized_member_size_)
        {
            const size_t member_serialized_size = last_offset - offset_ -
                    (current_state.header_serialized_ == XCdrHeaderSelection::SHORT_HEADER ? 4 : 8);
            if (8 < member_serialized_size)
            {
                switch (current_state.header_serialized_)
                {
                    case XCdrHeaderSelection::SHORT_HEADER:
                        if (AUTO_WITH_SHORT_HEADER_BY_DEFAULT == current_state.header_selection_)
                        {
                            xcdr2_change_to_long_member_header(current_state.next_member_id_, member_serialized_size);
                        }
                        else
                        {
                            assert(false);     // TODO throw exception
                        }
                        break;
                    case XCdrHeaderSelection::LONG_HEADER:
                        xcdr2_end_long_member_header(current_state.next_member_id_, member_serialized_size);
                        break;
                    default:
                        assert(false);     // header_serialized_ must have only SHORT_HEADER or LONG_HEADER
                }
            }
            else
            {
                switch (current_state.header_serialized_)
                {
                    case XCdrHeaderSelection::SHORT_HEADER:
                        xcdr2_end_short_member_header(current_state.next_member_id_, member_serialized_size);
                        break;
                    case XCdrHeaderSelection::LONG_HEADER:
                        if (AUTO_WITH_LONG_HEADER_BY_DEFAULT == current_state.header_selection_)
                        {
                            xcdr2_change_to_short_member_header(current_state.next_member_id_, member_serialized_size);
                        }
                        else
                        {
                            xcdr2_end_long_member_header(current_state.next_member_id_, member_serialized_size);
                        }
                        break;
                    default:
                        assert(false);     // header_serialized_ must have only SHORT_HEADER or LONG_HEADER
                }
            }

            jump(member_serialized_size);
        }
        else
        {
            // Use inner type DHEADER as NEXTINT
            switch (current_state.header_serialized_)
            {
                case XCdrHeaderSelection::SHORT_HEADER:
                    if (AUTO_WITH_SHORT_HEADER_BY_DEFAULT == current_state.header_selection_)
                    {
                        xcdr2_end_long_member_header(current_state.next_member_id_, 0);
                        offset_ = last_offset;
                    }
                    else
                    {
                        assert(false);         // TODO throw exception
                    }
                    break;
                case XCdrHeaderSelection::LONG_HEADER:
                    xcdr2_shrink_to_long_member_header(current_state.next_member_id_, last_offset);
                    offset_ = last_offset;
                    offset_ -= sizeof(uint32_t);
                    break;
                default:
                    assert(false);         // header_serialized_ must have only SHORT_HEADER or LONG_HEADER
            }

            m_lastDataSize = 0;
        }
    }

    next_member_id_ = MEMBER_ID_INVALID;
    serialized_member_size_ = NO_SERIALIZED_MEMBER_SIZE;

    return *this;
}

void Cdr::xcdr2_deserialize_member_header(
        MemberId& member_id,
        Cdr::state& current_state)
{
    uint32_t flags_and_member_id = 0;
    deserialize(flags_and_member_id);
    member_id.must_understand = (flags_and_member_id & 0x80000000);
    uint32_t mid = (flags_and_member_id & 0x0FFFFFFF);
    uint32_t lc = (flags_and_member_id & 0x70000000) >> 28;

    member_id.id = mid;

    if (4 > lc)
    {
        switch (lc)
        {
            case 0:
                current_state.member_size_ = 1;
                break;
            case 1:
                current_state.member_size_ = 2;
                break;
            case 2:
                current_state.member_size_ = 4;
                break;
            case 3:
                current_state.member_size_ = 8;
                break;
            default:
                break;
        }
        current_state.header_serialized_ = XCdrHeaderSelection::SHORT_HEADER;
    }
    else if (4 == lc)
    {
        uint32_t size = 0;
        deserialize(size);
        current_state.member_size_ = size;
        current_state.header_serialized_ = XCdrHeaderSelection::LONG_HEADER;
    }
    else
    {
        uint32_t size = 0;
        deserialize(size);
        current_state.member_size_ = 4 + (size * (5 == lc ? 1 : (6 == lc ? 4 : 8)));
        current_state.header_serialized_ = XCdrHeaderSelection::SHORT_HEADER;
        offset_ -= sizeof(uint32_t);
    }
}

Cdr& Cdr::xcdr1_begin_serialize_type(
        Cdr::state& current_state,
        EncodingAlgorithmFlag type_encoding)
{
    static_cast<void>(current_state);
    assert(current_state == Cdr::state(*this));
    assert(EncodingAlgorithmFlag::PLAIN_CDR == current_encoding_ ||
            EncodingAlgorithmFlag::PL_CDR == current_encoding_);
    assert(offset_ == m_cdrBuffer.begin() ? current_encoding_ == type_encoding : true);
    current_state.previous_enconding_ = current_encoding_;
    current_encoding_ = type_encoding;
    return *this;
}

Cdr& Cdr::xcdr1_end_serialize_type(
        const Cdr::state& current_state)
{
    assert(EncodingAlgorithmFlag::PLAIN_CDR == current_encoding_ ||
            EncodingAlgorithmFlag::PL_CDR == current_encoding_);

    if (EncodingAlgorithmFlag::PL_CDR == current_encoding_)
    {
        makeAlign(alignment(4));
        serialize(PID_SENTINEL);
        serialize(PID_SENTINEL_LENGTH);
    }

    current_encoding_ = current_state.previous_enconding_;

    return *this;
}

Cdr& Cdr::xcdr2_begin_serialize_type(
        Cdr::state& current_state,
        EncodingAlgorithmFlag type_encoding)
{
    static_cast<void>(current_state);
    assert(current_state == Cdr::state(*this));
    assert(EncodingAlgorithmFlag::PLAIN_CDR2 == current_encoding_ ||
            EncodingAlgorithmFlag::DELIMIT_CDR2 == current_encoding_ ||
            EncodingAlgorithmFlag::PL_CDR2 == current_encoding_);
    assert(offset_ == m_cdrBuffer.begin() ? current_encoding_ == type_encoding : true);
    if (EncodingAlgorithmFlag::PLAIN_CDR2 != type_encoding)
    {
        uint32_t dheader {0};
        serialize(dheader);
    }
    serialized_member_size_ = NO_SERIALIZED_MEMBER_SIZE; // Avoid error when serializen arrays, sequences, etc..
    current_state.previous_enconding_ = current_encoding_;
    current_encoding_ = type_encoding;
    return *this;
}

Cdr& Cdr::xcdr2_end_serialize_type(
        const Cdr::state& current_state)
{
    assert(EncodingAlgorithmFlag::PLAIN_CDR2 == current_encoding_ ||
            EncodingAlgorithmFlag::DELIMIT_CDR2 == current_encoding_ ||
            EncodingAlgorithmFlag::PL_CDR2 == current_encoding_);
    if (EncodingAlgorithmFlag::PLAIN_CDR2 != current_encoding_)
    {
        auto last_offset = offset_;
        set_state(current_state);
        const size_t member_serialized_size = last_offset - offset_ /*DHEADER ->*/ - 4 - alignment(4);
        serialize(static_cast<uint32_t>(member_serialized_size));
        jump(member_serialized_size);
        serialized_member_size_ = SERIALIZED_MEMBER_SIZE;
    }
    current_encoding_ = current_state.previous_enconding_;
    return *this;
}

Cdr& Cdr::xcdr1_deserialize_type(
        EncodingAlgorithmFlag type_encoding,
        std::function<bool (Cdr&, const MemberId&)> functor)
{
    assert(EncodingAlgorithmFlag::PLAIN_CDR == type_encoding ||
            EncodingAlgorithmFlag::PL_CDR == type_encoding);
    assert(offset_ == m_cdrBuffer.begin() ? current_encoding_ == type_encoding : true);
    Cdr::state current_state(*this);

    if (EncodingAlgorithmFlag::PL_CDR == type_encoding)
    {
        while (xcdr1_deserialize_member_header(next_member_id_, current_state))
        {
            bool deser_value = functor(*this, next_member_id_);

            if (!deser_value)
            {
                if (next_member_id_.must_understand)
                {
                    assert(false); // TODO throw exception
                }
                else
                {
                    jump(current_state.member_size_);
                }
            }

            assert(current_state.member_size_ == offset_ - origin_); // TODO throw exception.
            // Reset state to POP(origin=0) because before serializing member the algorithm did PUSH(origin=0).
            auto last_offset = offset_;
            set_state(current_state);
            offset_ = last_offset;
            m_lastDataSize = 0;
        }
    }
    else
    {
        next_member_id_ = MemberId(0);

        while (offset_ != end_ && functor(*this, next_member_id_)) // TODO Tests with appendables with different length
        {
            ++next_member_id_.id;
        }

        next_member_id_ = current_state.next_member_id_;
    }

    return *this;
}

Cdr& Cdr::xcdr2_deserialize_type(
        EncodingAlgorithmFlag type_encoding,
        std::function<bool (Cdr&, const MemberId&)> functor)
{
    assert(EncodingAlgorithmFlag::PLAIN_CDR2 == type_encoding ||
            EncodingAlgorithmFlag::DELIMIT_CDR2 == type_encoding ||
            EncodingAlgorithmFlag::PL_CDR2 == type_encoding);
    assert(offset_ == m_cdrBuffer.begin() ? current_encoding_ == type_encoding : true);


    if (EncodingAlgorithmFlag::PLAIN_CDR2 != type_encoding)
    {
        uint32_t dheader {0};
        deserialize(dheader);

        Cdr::state current_state(*this);

        if (EncodingAlgorithmFlag::PL_CDR2 == type_encoding)
        {
            while (offset_ - current_state.offset_ != dheader)
            {
                assert(offset_ - current_state.offset_ < dheader); // TODO throw exception
                auto offset = offset_;
                (void)offset;

                xcdr2_deserialize_member_header(next_member_id_, current_state);
                bool deser_value = functor(*this, next_member_id_);
                if (!deser_value)
                {
                    if (next_member_id_.must_understand)
                    {
                        assert(false); // TODO Throw exception
                    }
                    else
                    {
                        jump(current_state.member_size_);
                    }
                }

                assert((0 == current_state.member_size_ &&
                        0 == offset_ - offset) ||
                        (0 < current_state.member_size_ &&
                        current_state.member_size_ == offset_ - offset -
                        alignment_on_state(current_state.origin_, offset, sizeof(uint32_t)) -
                        (XCdrHeaderSelection::SHORT_HEADER == current_state.header_serialized_ ? 4 : 8)));

            }

            next_member_id_ = current_state.next_member_id_;
        }
        else
        {
            next_member_id_ = MemberId(0);

            while (offset_ - current_state.offset_ < dheader && functor(*this, next_member_id_))
            {
                ++next_member_id_.id;
            }
            size_t jump_size = dheader - (offset_ - current_state.offset_);
            jump(jump_size);

            next_member_id_ = current_state.next_member_id_;
        }
    }
    else
    {
        Cdr::state current_state(*this);
        next_member_id_ = MemberId(0);

        while (offset_ != end_ && functor(*this, next_member_id_))
        {
            ++next_member_id_.id;
        }

        next_member_id_ = current_state.next_member_id_;
    }

    return *this;
}

} // namespace fastcdr
} // namespace eprosima
