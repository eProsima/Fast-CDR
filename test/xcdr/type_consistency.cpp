// Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <gtest/gtest.h>

#include <fastcdr/Cdr.h>
#include <fastcdr/CdrEncoding.hpp>
#include <fastcdr/FastBuffer.h>
#include <fastcdr/cdr/fixed_size_string.hpp>
#include <fastcdr/exceptions/BadParamException.h>
#include <fastcdr/exceptions/NotEnoughMemoryException.h>
#include "utility.hpp"

using namespace eprosima::fastcdr;
using eprosima::fastcdr::exception::BadParamException;

namespace {

constexpr size_t TEST_BUFFER_LENGTH = 1024;

//! Serializes @p value with the encoding and endianness currently under test.
template<typename SerializedT>
void serialize_into(
        char* buffer,
        size_t buffer_size,
        const SerializedT& value,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness)
{
    FastBuffer fast_buffer(buffer, buffer_size);
    Cdr cdr_ser(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_ser.set_encoding_flag(encoding);
    cdr_ser << value;
}

//! Serializes @p value followed by @p trailer, so tests can check where the stream is left.
template<typename SerializedT, typename TrailerT>
void serialize_into_with_trailer(
        char* buffer,
        size_t buffer_size,
        const SerializedT& value,
        const TrailerT& trailer,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness)
{
    FastBuffer fast_buffer(buffer, buffer_size);
    Cdr cdr_ser(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_ser.set_encoding_flag(encoding);
    cdr_ser << value << trailer;
}

} // namespace

/*!
 * The try-construct policies are exercised against every supported CDR version. XCDRv1 and XCDRv2
 * take different code paths when discarding the part of a collection that does not fit in the
 * destination: XCDRv2 skips it using the DHEADER, while XCDRv1 has no DHEADER and must consume the
 * remainder element by element.
 */
class XCdrTypeConsistencyTest : public ::testing::TestWithParam<std::tuple<EncodingAlgorithmFlag,
            Cdr::Endianness>>
{
};


/*!
 * Element type used to check the policies on a sequence of non-primitive values holding a @c bool .
 * Discarding such an element goes through @c deserialize(bool&) , which takes a dedicated path when
 * the value is consumed without being stored. The second member of a different type makes an
 * incorrect skip visible as misaligned data rather than as a plausible value.
 */
struct InnerBoolStruct
{
    InnerBoolStruct() = default;

    InnerBoolStruct(
            bool b,
            uint16_t v)
        : value1(b)
        , value2(v)
    {
    }

    bool operator ==(
            const InnerBoolStruct& other) const
    {
        return value1 == other.value1 && value2 == other.value2;
    }

    bool value1 {false};

    uint16_t value2 {0};
};

namespace eprosima {
namespace fastcdr {

template<>
void serialize(
        Cdr& cdr,
        const InnerBoolStruct& data)
{
    Cdr::state current_status(cdr);
    cdr.begin_serialize_type(current_status, cdr.get_encoding_flag());
    cdr << MemberId(0) << data.value1;
    cdr << MemberId(1) << data.value2;
    cdr.end_serialize_type(current_status);
}

template<>
void deserialize(
        Cdr& cdr,
        InnerBoolStruct& data)
{
    cdr.deserialize_type(cdr.get_encoding_flag(), [&data](Cdr& cdr_inner, const MemberId& mid) -> bool
            {
                bool ret_value = true;

                switch (mid.id)
                {
                    case 0:
                        cdr_inner >> data.value1;
                        break;
                    case 1:
                        cdr_inner >> data.value2;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
}

} // namespace fastcdr
} // namespace eprosima

/*!
 * Element type used to check that discarding a mutable structure consumes exactly its bytes. Skipping it requires
 * the member headers (member id and size) and the length of the string member, which are only known by decoding
 * them, so it fails if any of those values is not stored while the element is discarded.
 */
struct InnerMutableStruct
{
    InnerMutableStruct() = default;

    InnerMutableStruct(
            const std::string& t,
            int32_t n)
        : text(t)
        , number(n)
    {
    }

    bool operator ==(
            const InnerMutableStruct& other) const
    {
        return text == other.text && number == other.number;
    }

    std::string text;

    int32_t number {0};
};

namespace eprosima {
namespace fastcdr {

template<>
void serialize(
        Cdr& cdr,
        const InnerMutableStruct& data)
{
    Cdr::state current_status(cdr);
    cdr.begin_serialize_type(current_status, cdr.get_cdr_version() == CdrVersion::XCDRv1
                             ? EncodingAlgorithmFlag::PL_CDR
                             : EncodingAlgorithmFlag::PL_CDR2);
    cdr << MemberId(0) << data.text;
    cdr << MemberId(1) << data.number;
    cdr.end_serialize_type(current_status);
}

template<>
void deserialize(
        Cdr& cdr,
        InnerMutableStruct& data)
{
    cdr.deserialize_type(cdr.get_cdr_version() == CdrVersion::XCDRv1
                         ? EncodingAlgorithmFlag::PL_CDR
                         : EncodingAlgorithmFlag::PL_CDR2,
            [&data](Cdr& cdr_inner, const MemberId& mid) -> bool
            {
                bool ret_value = true;

                switch (mid.id)
                {
                    case 0:
                        cdr_inner >> data.text;
                        break;
                    case 1:
                        cdr_inner >> data.number;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
}

} // namespace fastcdr
} // namespace eprosima

/*!
 * Element type used to check that discarding a structure with an optional member consumes exactly its bytes.
 * Whether the optional value follows is only known by decoding the presence flag (XCDRv2) or the member header size
 * (XCDRv1), so skipping fails if that value is not stored while the element is discarded. Elements with and without
 * the optional value are both needed: each one fails under a different wrong default.
 */
struct InnerOptionalStruct
{
    InnerOptionalStruct() = default;

    InnerOptionalStruct(
            const optional<int32_t>& o,
            uint16_t v)
        : value1(o)
        , value2(v)
    {
    }

    bool operator ==(
            const InnerOptionalStruct& other) const
    {
        return value1 == other.value1 && value2 == other.value2;
    }

    optional<int32_t> value1;

    uint16_t value2 {0};
};

namespace eprosima {
namespace fastcdr {

template<>
void serialize(
        Cdr& cdr,
        const InnerOptionalStruct& data)
{
    Cdr::state current_status(cdr);
    cdr.begin_serialize_type(current_status, cdr.get_encoding_flag());
    cdr << MemberId(0) << data.value1;
    cdr << MemberId(1) << data.value2;
    cdr.end_serialize_type(current_status);
}

template<>
void deserialize(
        Cdr& cdr,
        InnerOptionalStruct& data)
{
    cdr.deserialize_type(cdr.get_encoding_flag(), [&data](Cdr& cdr_inner, const MemberId& mid) -> bool
            {
                bool ret_value = true;

                switch (mid.id)
                {
                    case 0:
                        cdr_inner >> data.value1;
                        break;
                    case 1:
                        cdr_inner >> data.value2;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
}

} // namespace fastcdr
} // namespace eprosima

// =====================================================================================
// std::string
// =====================================================================================

/*!
 * @brief Checks that a std::string shorter than the bound is deserialized unchanged.
 *
 * The serialized string is well inside the bound, so no try-construct policy applies. This is the control case the
 * three policy cases for std::string are compared against. The case is repeated for XCDRv1 and XCDRv2, in big and
 * little endian.
 *
 * @test XCdrTypeConsistencyTest.StdString_Fit
 * @pre A 5 character std::string ("hello") is serialized with the encoding and endianness under test.
 * @post Deserializing with @c FAIL and a bound of 10 does not throw and yields the original string.
 */
TEST_P(XCdrTypeConsistencyTest, StdString_Fit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::string output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

/*!
 * @brief Checks that a std::string filling the bound exactly is accepted.
 *
 * CDR serializes a narrow string with a terminating null character counted in its length field, whereas the bound
 * counts only the characters of the value. A string of exactly @c max_length characters must therefore be accepted
 * instead of being treated as too long. Reading the trailer afterwards also proves the whole serialized string,
 * terminator included, was consumed. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdString_ExactFit
 * @pre A 5 character std::string ("hello") followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c FAIL and a bound of 5 does not throw, yields the original string, and leaves the
 * stream positioned on the trailer, which is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdString_ExactFit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello"};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::string output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 5));
    EXPECT_EQ(output, input);

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

/*!
 * @brief Checks that @c FAIL rejects a std::string longer than the bound.
 *
 * @c FAIL is the policy that refuses to build a value that does not fit, reporting the inconsistency to the caller
 * instead of silently altering the data. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdString_Fail
 * @pre An 11 character std::string ("hello world") is serialized.
 * @post Deserializing with @c FAIL and a bound of 5 throws @c BadParamException .
 */
TEST_P(XCdrTypeConsistencyTest, StdString_Fail)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::string output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 5), BadParamException);
}

/*!
 * @brief Checks that @c DEFAULT_VALUE discards a std::string longer than the bound.
 *
 * The destination is pre-filled before the call so that an empty result proves the value was actively reset,
 * rather than merely left untouched. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdString_DefaultValue
 * @pre An 11 character std::string is serialized and the destination holds "prefilled".
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 5 does not throw and leaves the destination empty.
 */
TEST_P(XCdrTypeConsistencyTest, StdString_DefaultValue)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::string output {"prefilled"};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 5));
    EXPECT_TRUE(output.empty());
}

/*!
 * @brief Checks that @c TRIM truncates a std::string longer than the bound.
 *
 * @c TRIM keeps as much of the received value as the destination can hold, discarding the excess characters. The
 * case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdString_Trim
 * @pre An 11 character std::string ("hello world") is serialized.
 * @post Deserializing with @c TRIM and a bound of 5 does not throw and yields "hello".
 */
TEST_P(XCdrTypeConsistencyTest, StdString_Trim)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::string output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 5));
    EXPECT_EQ(output, std::string("hello"));
}


// =====================================================================================
// std::wstring
// =====================================================================================

/*!
 * @brief Checks that a std::wstring shorter than the bound is deserialized unchanged.
 *
 * The serialized wide string is well inside the bound, so no try-construct policy applies. This is the control
 * case for the std::wstring policies. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdWstring_Fit
 * @pre A 5 character std::wstring (L"hello") is serialized with the encoding and endianness under test.
 * @post Deserializing with @c FAIL and a bound of 10 does not throw and yields the original wide string.
 */
TEST_P(XCdrTypeConsistencyTest, StdWstring_Fit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::wstring output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

/*!
 * @brief Checks that a std::wstring filling the bound exactly is accepted.
 *
 * Unlike narrow strings, wide strings are serialized without a terminating null character, so the length field
 * already matches the number of characters of the value. The case guards that boundary and, through the trailer,
 * that the whole wide string was consumed. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdWstring_ExactFit
 * @pre A 5 character std::wstring (L"hello") followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c FAIL and a bound of 5 does not throw, yields the original wide string, and leaves
 * the stream positioned on the trailer, which is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdWstring_ExactFit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello"};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::wstring output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 5));
    EXPECT_EQ(output, input);

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

/*!
 * @brief Checks that @c FAIL rejects a std::wstring longer than the bound.
 *
 * @c FAIL reports the inconsistency to the caller instead of silently altering the received wide string. The case
 * is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdWstring_Fail
 * @pre An 11 character std::wstring (L"hello world") is serialized.
 * @post Deserializing with @c FAIL and a bound of 5 throws @c BadParamException .
 */
TEST_P(XCdrTypeConsistencyTest, StdWstring_Fail)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::wstring output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 5), BadParamException);
}

/*!
 * @brief Checks that @c DEFAULT_VALUE discards a std::wstring longer than the bound.
 *
 * The destination is pre-filled before the call so that an empty result proves the value was actively reset. The
 * case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdWstring_DefaultValue
 * @pre An 11 character std::wstring is serialized and the destination holds L"prefilled".
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 5 does not throw and leaves the destination empty.
 */
TEST_P(XCdrTypeConsistencyTest, StdWstring_DefaultValue)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::wstring output {L"prefilled"};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 5));
    EXPECT_TRUE(output.empty());
}

/*!
 * @brief Checks that @c TRIM truncates a std::wstring longer than the bound.
 *
 * @c TRIM keeps as many wide characters as the destination can hold and discards the excess. The case is repeated
 * for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdWstring_Trim
 * @pre An 11 character std::wstring (L"hello world") is serialized.
 * @post Deserializing with @c TRIM and a bound of 5 does not throw and yields L"hello".
 */
TEST_P(XCdrTypeConsistencyTest, StdWstring_Trim)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::wstring output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 5));
    EXPECT_EQ(output, std::wstring(L"hello"));
}


// =====================================================================================
// char*&
// =====================================================================================

/*!
 * @brief Checks that a C string shorter than the bound is deserialized unchanged.
 *
 * The @c char*& overload allocates the returned buffer, which the caller owns and releases. The serialized string
 * is well inside the bound, so no policy applies. The case is repeated for XCDRv1 and XCDRv2, in big and little
 * endian.
 *
 * @test XCdrTypeConsistencyTest.CharPtr_Fit
 * @pre A 5 character string ("hello") is serialized with the encoding and endianness under test.
 * @post Deserializing with @c FAIL and a bound of 10 does not throw and yields a non-null, null-terminated
 * "hello", which the test frees.
 */
TEST_P(XCdrTypeConsistencyTest, CharPtr_Fit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    char* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 10));
    ASSERT_NE(output, nullptr);
    EXPECT_STREQ(output, "hello");
    free(output);
}

/*!
 * @brief Checks that a C string filling the bound exactly is accepted.
 *
 * The serialized length of a narrow string counts the terminating null character, while the bound counts only the
 * characters of the value, so a string of exactly @c max_length characters must be accepted. The trailer
 * additionally proves the terminator was consumed and not left in the stream. The case is repeated for XCDRv1 and
 * XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.CharPtr_ExactFit
 * @pre A 5 character string ("hello") followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c FAIL and a bound of 5 does not throw, yields "hello", and leaves the stream
 * positioned on the trailer, which is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, CharPtr_ExactFit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello"};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    char* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 5));
    ASSERT_NE(output, nullptr);
    EXPECT_STREQ(output, "hello");
    free(output);

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

/*!
 * @brief Checks that @c FAIL rejects a C string longer than the bound and allocates nothing.
 *
 * Besides reporting the inconsistency, the overload must not hand back a buffer the caller would have to release
 * after a failed call. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.CharPtr_Fail
 * @pre An 11 character string ("hello world") is serialized and the destination pointer is null.
 * @post Deserializing with @c FAIL and a bound of 5 throws @c BadParamException and the destination pointer is
 * left null.
 */
TEST_P(XCdrTypeConsistencyTest, CharPtr_Fail)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    char* output {nullptr};
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 5), BadParamException);
    EXPECT_EQ(output, nullptr);
}

/*!
 * @brief Checks that @c DEFAULT_VALUE discards a C string longer than the bound.
 *
 * The discarded value is an allocated empty string, as with the std::string overload, so the caller can use the
 * result without checking for a null pointer. The case is repeated for XCDRv1 and XCDRv2, in big and little
 * endian.
 *
 * @test XCdrTypeConsistencyTest.CharPtr_DefaultValue
 * @pre An 11 character string is serialized and the destination pointer is null.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 5 does not throw and yields an allocated empty string.
 */
TEST_P(XCdrTypeConsistencyTest, CharPtr_DefaultValue)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    char* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 5));
    ASSERT_NE(output, nullptr);
    EXPECT_STREQ(output, "");
    free(output);
}

/*!
 * @brief Checks that @c TRIM truncates a C string longer than the bound.
 *
 * The truncated result must still be null-terminated even though the characters kept do not include the terminator
 * that was serialized. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.CharPtr_Trim
 * @pre An 11 character string ("hello world") is serialized.
 * @post Deserializing with @c TRIM and a bound of 5 does not throw and yields a null-terminated "hello", which the
 * test frees.
 */
TEST_P(XCdrTypeConsistencyTest, CharPtr_Trim)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    char* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 5));
    ASSERT_NE(output, nullptr);
    EXPECT_STREQ(output, "hello");
    free(output);
}


// =====================================================================================
// wchar_t*&
// =====================================================================================

/*!
 * @brief Checks that a C wide string shorter than the bound is deserialized unchanged.
 *
 * The @c wchar_t*& overload allocates the returned buffer, which the caller owns and releases. The case is
 * repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.WcharPtr_Fit
 * @pre A 5 character wide string (L"hello") is serialized with the encoding and endianness under test.
 * @post Deserializing with @c FAIL and a bound of 10 does not throw and yields a non-null, null-terminated
 * L"hello", which the test frees.
 */
TEST_P(XCdrTypeConsistencyTest, WcharPtr_Fit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    wchar_t* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 10));
    ASSERT_NE(output, nullptr);
    EXPECT_EQ(std::wcscmp(output, L"hello"), 0);
    free(output);
}

/*!
 * @brief Checks that a C wide string filling the bound exactly is accepted.
 *
 * Wide strings carry no terminating null character on the wire, so the serialized length already matches the
 * number of characters of the value. The trailer proves the whole wide string was consumed. The case is repeated
 * for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.WcharPtr_ExactFit
 * @pre A 5 character wide string (L"hello") followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c FAIL and a bound of 5 does not throw, yields L"hello", and leaves the stream
 * positioned on the trailer, which is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, WcharPtr_ExactFit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello"};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    wchar_t* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 5));
    ASSERT_NE(output, nullptr);
    EXPECT_EQ(std::wcscmp(output, L"hello"), 0);
    free(output);

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

/*!
 * @brief Checks that @c FAIL rejects a C wide string longer than the bound and allocates nothing.
 *
 * The overload must not hand back a buffer the caller would have to release after a failed call. The case is
 * repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.WcharPtr_Fail
 * @pre An 11 character wide string (L"hello world") is serialized and the destination pointer is null.
 * @post Deserializing with @c FAIL and a bound of 5 throws @c BadParamException and the destination pointer is
 * left null.
 */
TEST_P(XCdrTypeConsistencyTest, WcharPtr_Fail)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    wchar_t* output {nullptr};
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 5), BadParamException);
    EXPECT_EQ(output, nullptr);
}

/*!
 * @brief Checks that @c DEFAULT_VALUE discards a C wide string longer than the bound.
 *
 * The discarded value is an allocated empty wide string, as with the std::wstring overload, so the caller can use
 * the result without checking for a null pointer. The case is repeated for XCDRv1 and XCDRv2, in big and little
 * endian.
 *
 * @test XCdrTypeConsistencyTest.WcharPtr_DefaultValue
 * @pre An 11 character wide string is serialized and the destination pointer is null.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 5 does not throw and yields an allocated empty wide
 * string.
 */
TEST_P(XCdrTypeConsistencyTest, WcharPtr_DefaultValue)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    wchar_t* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 5));
    ASSERT_NE(output, nullptr);
    EXPECT_EQ(std::wcscmp(output, L""), 0);
    free(output);
}

/*!
 * @brief Checks that @c TRIM truncates a C wide string longer than the bound.
 *
 * The truncated result must still be null-terminated even though no terminator was received. The case is repeated
 * for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.WcharPtr_Trim
 * @pre An 11 character wide string (L"hello world") is serialized.
 * @post Deserializing with @c TRIM and a bound of 5 does not throw and yields a null-terminated L"hello", which
 * the test frees.
 */
TEST_P(XCdrTypeConsistencyTest, WcharPtr_Trim)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    wchar_t* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 5));
    ASSERT_NE(output, nullptr);
    EXPECT_EQ(std::wcscmp(output, L"hello"), 0);
    free(output);
}


// =====================================================================================
// fixed_string<MAX>
// =====================================================================================

/*!
 * @brief Checks that a string shorter than the fixed_string capacity is deserialized unchanged.
 *
 * For @c fixed_string the bound is the @c MAX_CHARS template argument rather than an explicit argument, so the
 * policy is the only extra parameter. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.FixedString_Fit
 * @pre A 2 character string ("hi") is serialized and the destination is a @c fixed_string<8> .
 * @post Deserializing with @c FAIL does not throw and yields "hi".
 */
TEST_P(XCdrTypeConsistencyTest, FixedString_Fit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hi"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    fixed_string<8> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL));
    EXPECT_STREQ(output.c_str(), "hi");
}

/*!
 * @brief Checks that a string filling the fixed_string capacity exactly is accepted.
 *
 * The serialized length of a narrow string counts the terminating null character, while @c MAX_CHARS counts only
 * the characters the destination can hold, so a string of exactly @c MAX_CHARS characters must be accepted rather
 * than reported as too long. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.FixedString_ExactFit
 * @pre A 5 character string ("hello") is serialized and the destination is a @c fixed_string<5> .
 * @post Deserializing with @c FAIL does not throw and yields "hello".
 */
TEST_P(XCdrTypeConsistencyTest, FixedString_ExactFit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    fixed_string<5> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL));
    EXPECT_STREQ(output.c_str(), "hello");
}

/*!
 * @brief Checks that the overload without a policy accepts a string filling the fixed_string exactly.
 *
 * The overload that takes no policy behaves as @c FAIL, so it rejects strings longer than the destination. A
 * string that fills the destination exactly is a valid value and must still be accepted; this case pins that
 * boundary so it cannot be lost while adjusting the rejection of longer strings. The case is repeated for XCDRv1
 * and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.FixedString_ExactFitWithoutPolicy
 * @pre A 5 character string ("hello") is serialized and the destination is a @c fixed_string<5> .
 * @post Deserializing without a policy does not throw and yields "hello".
 */
TEST_P(XCdrTypeConsistencyTest, FixedString_ExactFitWithoutPolicy)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    fixed_string<5> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output));
    EXPECT_STREQ(output.c_str(), "hello");
}

/*!
 * @brief Checks that @c FAIL rejects a string longer than the fixed_string capacity.
 *
 * @c FAIL reports the inconsistency instead of silently truncating, which is the behaviour bounded strings rely on
 * to detect mismatched types. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.FixedString_Fail
 * @pre A 29 character string is serialized and the destination is a @c fixed_string<8> .
 * @post Deserializing with @c FAIL throws @c BadParamException .
 */
TEST_P(XCdrTypeConsistencyTest, FixedString_Fail)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"too long for the fixed string"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    fixed_string<8> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL), BadParamException);
}

/*!
 * @brief Checks that @c DEFAULT_VALUE discards a string longer than the fixed_string capacity.
 *
 * The destination is pre-filled before the call so that an empty result proves the value was actively reset. The
 * case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.FixedString_DefaultValue
 * @pre A 29 character string is serialized and the destination is a @c fixed_string<8> holding "prefill".
 * @post Deserializing with @c DEFAULT_VALUE does not throw and leaves the destination empty.
 */
TEST_P(XCdrTypeConsistencyTest, FixedString_DefaultValue)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"too long for the fixed string"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    fixed_string<8> output {"prefill"};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE));
    EXPECT_STREQ(output.c_str(), "");
}

/*!
 * @brief Checks that @c TRIM truncates a string longer than the fixed_string capacity.
 *
 * @c TRIM is the only way to obtain the legacy truncating behaviour, since the overload without a policy now
 * reports an error instead. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.FixedString_Trim
 * @pre A 12 character string ("abcdefghijkl") is serialized and the destination is a @c fixed_string<8> .
 * @post Deserializing with @c TRIM does not throw and yields "abcdefgh".
 */
TEST_P(XCdrTypeConsistencyTest, FixedString_Trim)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"abcdefghijkl"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    fixed_string<8> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM));
    EXPECT_STREQ(output.c_str(), "abcdefgh");
}


// =====================================================================================
// std::vector<primitive>
// =====================================================================================

/*!
 * @brief Checks that a sequence of primitives shorter than the bound is deserialized unchanged.
 *
 * For collections the bound is a number of elements. The sequence is well inside it, so no policy applies. The
 * case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorPrimitive_Fit
 * @pre A 3 element std::vector<int32_t> is serialized with the encoding and endianness under test.
 * @post Deserializing with @c FAIL and a bound of 10 does not throw and yields the original sequence.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorPrimitive_Fit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<int32_t> input {1, 2, 3};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<int32_t> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

/*!
 * @brief Checks that @c FAIL rejects a sequence of primitives longer than the bound.
 *
 * @c FAIL reports the inconsistency rather than delivering a partial sequence. The case is repeated for XCDRv1 and
 * XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorPrimitive_Fail
 * @pre A 5 element std::vector<int32_t> is serialized.
 * @post Deserializing with @c FAIL and a bound of 3 throws @c BadParamException .
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorPrimitive_Fail)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<int32_t> input {1, 2, 3, 4, 5};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<int32_t> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 3), BadParamException);
}

/*!
 * @brief Checks that @c DEFAULT_VALUE discards a sequence of primitives longer than the bound.
 *
 * The surplus elements still have to be consumed from the stream even though none of them is stored. The
 * destination is pre-filled so that an empty result proves it was actively cleared. The case is repeated for
 * XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorPrimitive_DefaultValue
 * @pre A 5 element std::vector<int32_t> is serialized and the destination holds one element.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 3 does not throw and leaves the destination empty.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorPrimitive_DefaultValue)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<int32_t> input {1, 2, 3, 4, 5};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<int32_t> output {99};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 3));
    EXPECT_TRUE(output.empty());
}

/*!
 * @brief Checks that @c TRIM keeps the leading elements of a sequence of primitives longer than the bound.
 *
 * The elements beyond the bound are consumed from the stream but not stored. The case is repeated for XCDRv1 and
 * XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorPrimitive_Trim
 * @pre A 5 element std::vector<int32_t> ({1, 2, 3, 4, 5}) is serialized.
 * @post Deserializing with @c TRIM and a bound of 3 does not throw and yields {1, 2, 3}.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorPrimitive_Trim)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<int32_t> input {1, 2, 3, 4, 5};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<int32_t> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 3));
    const std::vector<int32_t> expected {1, 2, 3};
    EXPECT_EQ(output, expected);
}


// =====================================================================================
// std::vector<non-primitive>
// =====================================================================================

/*!
 * @brief Checks that a sequence of non-primitives shorter than the bound is deserialized unchanged.
 *
 * Sequences of non-primitive elements take a different code path from primitive ones, and under XCDRv2 they
 * additionally carry a DHEADER. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorNonPrimitive_Fit
 * @pre A 2 element std::vector<std::string> is serialized with the encoding and endianness under test.
 * @post Deserializing with @c FAIL and a bound of 10 does not throw and yields the original sequence.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorNonPrimitive_Fit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"alpha", "beta"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<std::string> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

/*!
 * @brief Checks that @c FAIL rejects a sequence of non-primitives longer than the bound.
 *
 * @c FAIL reports the inconsistency rather than delivering a partial sequence. The case is repeated for XCDRv1 and
 * XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorNonPrimitive_Fail
 * @pre A 4 element std::vector<std::string> is serialized.
 * @post Deserializing with @c FAIL and a bound of 2 throws @c BadParamException .
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorNonPrimitive_Fail)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"alpha", "beta", "gamma", "delta"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<std::string> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 2), BadParamException);
}

/*!
 * @brief Checks that @c DEFAULT_VALUE discards a sequence of non-primitives longer than the bound.
 *
 * The surplus elements must be consumed without being written to the destination, which has already been emptied.
 * XCDRv2 skips them using the DHEADER, while XCDRv1 has to read each one and throw it away. The case is repeated
 * for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorNonPrimitive_DefaultValue
 * @pre A 4 element std::vector<std::string> is serialized and the destination holds one element.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 2 does not throw and leaves the destination empty.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorNonPrimitive_DefaultValue)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"alpha", "beta", "gamma", "delta"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<std::string> output {"x"};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());
}

/*!
 * @brief Checks that @c TRIM keeps the leading elements of a sequence of non-primitives longer than the bound.
 *
 * The elements beyond the bound are consumed without being written to the destination, which is shorter than the
 * received sequence. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorNonPrimitive_Trim
 * @pre A 4 element std::vector<std::string> ({"alpha", "beta", "gamma", "delta"}) is serialized.
 * @post Deserializing with @c TRIM and a bound of 2 does not throw and yields {"alpha", "beta"}.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorNonPrimitive_Trim)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"alpha", "beta", "gamma", "delta"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<std::string> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2));
    const std::vector<std::string> expected {"alpha", "beta"};
    EXPECT_EQ(output, expected);
}

/*!
 * @brief Checks that discarding a whole sequence of strings leaves the stream just past it.
 *
 * Skipping a string requires its length prefix, so the discarded elements must be read rather than only stepped
 * over. A trailer serialized after the sequence makes a wrong stream position visible. The case is repeated for
 * XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorNonPrimitive_DefaultValueConsumesWholeSequence
 * @pre A 4 element std::vector<std::string> followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 2 does not throw, leaves the destination empty, and the
 * trailer is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorNonPrimitive_DefaultValueConsumesWholeSequence)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"alpha", "beta", "gamma", "delta"};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<std::string> output {"x"};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

/*!
 * @brief Checks that truncating a sequence of strings leaves the stream just past it.
 *
 * The strings dropped must be consumed by exactly the bytes they occupy, which is observable only by reading what
 * was serialized next. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorNonPrimitive_TrimConsumesWholeSequence
 * @pre A 4 element std::vector<std::string> followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c TRIM and a bound of 2 does not throw, yields {"alpha", "beta"}, and the trailer is
 * read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorNonPrimitive_TrimConsumesWholeSequence)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"alpha", "beta", "gamma", "delta"};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<std::string> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2));
    const std::vector<std::string> expected {"alpha", "beta"};
    EXPECT_EQ(output, expected);

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}


// =====================================================================================
// std::vector<bool>
// =====================================================================================

/*!
 * @brief Checks that a sequence of booleans shorter than the bound is deserialized unchanged.
 *
 * @c std::vector<bool> is a bit-packed specialization and is handled by a dedicated code path, so it is covered
 * separately from the other sequences. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorBool_Fit
 * @pre A 3 element std::vector<bool> is serialized with the encoding and endianness under test.
 * @post Deserializing with @c FAIL and a bound of 10 does not throw and yields the original sequence.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorBool_Fit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<bool> input {true, false, true};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<bool> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

/*!
 * @brief Checks that @c FAIL rejects a sequence of booleans longer than the bound.
 *
 * @c FAIL reports the inconsistency rather than delivering a partial sequence. The case is repeated for XCDRv1 and
 * XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorBool_Fail
 * @pre A 5 element std::vector<bool> is serialized.
 * @post Deserializing with @c FAIL and a bound of 3 throws @c BadParamException .
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorBool_Fail)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<bool> input {true, false, true, true, false};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<bool> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 3), BadParamException);
}

/*!
 * @brief Checks that @c DEFAULT_VALUE discards a sequence of booleans longer than the bound.
 *
 * The destination is pre-filled so that an empty result proves it was actively cleared. The case is repeated for
 * XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorBool_DefaultValue
 * @pre A 5 element std::vector<bool> is serialized and the destination holds one element.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 3 does not throw and leaves the destination empty.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorBool_DefaultValue)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<bool> input {true, false, true, true, false};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<bool> output {true};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 3));
    EXPECT_TRUE(output.empty());
}

/*!
 * @brief Checks that @c TRIM keeps the leading elements of a sequence of booleans longer than the bound.
 *
 * The booleans beyond the bound are consumed from the stream but not stored. The case is repeated for XCDRv1 and
 * XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorBool_Trim
 * @pre A 5 element std::vector<bool> ({true, false, true, true, false}) is serialized.
 * @post Deserializing with @c TRIM and a bound of 3 does not throw and yields {true, false, true}.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorBool_Trim)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<bool> input {true, false, true, true, false};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<bool> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 3));
    const std::vector<bool> expected {true, false, true};
    EXPECT_EQ(output, expected);
}


// =====================================================================================
// std::map<primitive, primitive>
// =====================================================================================

/*!
 * @brief Checks that a map of primitives smaller than the bound is deserialized unchanged.
 *
 * For maps the bound is a number of entries. The map is well inside it, so no policy applies. The case is repeated
 * for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdMapPrimitive_Fit
 * @pre A 2 entry std::map<int32_t, int32_t> is serialized with the encoding and endianness under test.
 * @post Deserializing with @c FAIL and a bound of 10 does not throw and yields the original map.
 */
TEST_P(XCdrTypeConsistencyTest, StdMapPrimitive_Fit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, int32_t> input {{1, 10}, {2, 20}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::map<int32_t, int32_t> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

/*!
 * @brief Checks that @c FAIL rejects a map of primitives larger than the bound.
 *
 * @c FAIL reports the inconsistency rather than delivering a partial map. The case is repeated for XCDRv1 and
 * XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdMapPrimitive_Fail
 * @pre A 4 entry std::map<int32_t, int32_t> is serialized.
 * @post Deserializing with @c FAIL and a bound of 2 throws @c BadParamException .
 */
TEST_P(XCdrTypeConsistencyTest, StdMapPrimitive_Fail)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, int32_t> input {{1, 10}, {2, 20}, {3, 30}, {4, 40}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::map<int32_t, int32_t> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 2), BadParamException);
}

/*!
 * @brief Checks that @c DEFAULT_VALUE discards a map of primitives larger than the bound.
 *
 * Both the key and the value of every surplus entry have to be consumed from the stream even though none of them
 * is stored. The destination is pre-filled so that an empty result proves it was actively cleared. The case is
 * repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdMapPrimitive_DefaultValue
 * @pre A 4 entry std::map<int32_t, int32_t> is serialized and the destination holds one entry.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 2 does not throw and leaves the destination empty.
 */
TEST_P(XCdrTypeConsistencyTest, StdMapPrimitive_DefaultValue)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, int32_t> input {{1, 10}, {2, 20}, {3, 30}, {4, 40}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::map<int32_t, int32_t> output {{99, 99}};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());
}

/*!
 * @brief Checks that @c TRIM keeps the leading entries of a map of primitives larger than the bound.
 *
 * Entries are read in serialization order, so the entries kept are the first ones received. The case is repeated
 * for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdMapPrimitive_Trim
 * @pre A 4 entry std::map<int32_t, int32_t> ({1, 10}, {2, 20}, {3, 30}, {4, 40}) is serialized.
 * @post Deserializing with @c TRIM and a bound of 2 does not throw and yields {{1, 10}, {2, 20}}.
 */
TEST_P(XCdrTypeConsistencyTest, StdMapPrimitive_Trim)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, int32_t> input {{1, 10}, {2, 20}, {3, 30}, {4, 40}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::map<int32_t, int32_t> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2));
    const std::map<int32_t, int32_t> expected {{1, 10}, {2, 20}};
    EXPECT_EQ(output, expected);
}


// =====================================================================================
// std::map<primitive, non-primitive>
// =====================================================================================

/*!
 * @brief Checks that a map with non-primitive values smaller than the bound is deserialized unchanged.
 *
 * Maps with non-primitive values take a different code path from fully primitive ones, and under XCDRv2 they
 * additionally carry a DHEADER. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdMapNonPrimitive_Fit
 * @pre A 2 entry std::map<int32_t, std::string> is serialized with the encoding and endianness under test.
 * @post Deserializing with @c FAIL and a bound of 10 does not throw and yields the original map.
 */
TEST_P(XCdrTypeConsistencyTest, StdMapNonPrimitive_Fit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "one"}, {2, "two"}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::map<int32_t, std::string> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

/*!
 * @brief Checks that @c FAIL rejects a map with non-primitive values larger than the bound.
 *
 * @c FAIL reports the inconsistency rather than delivering a partial map. The case is repeated for XCDRv1 and
 * XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdMapNonPrimitive_Fail
 * @pre A 4 entry std::map<int32_t, std::string> is serialized.
 * @post Deserializing with @c FAIL and a bound of 2 throws @c BadParamException .
 */
TEST_P(XCdrTypeConsistencyTest, StdMapNonPrimitive_Fail)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::map<int32_t, std::string> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 2), BadParamException);
}

/*!
 * @brief Checks that @c DEFAULT_VALUE discards a map with non-primitive values larger than the bound.
 *
 * The surplus entries must be consumed without being stored. XCDRv2 skips them using the DHEADER, while XCDRv1 has
 * to read each key and value and throw them away. The case is repeated for XCDRv1 and XCDRv2, in big and little
 * endian.
 *
 * @test XCdrTypeConsistencyTest.StdMapNonPrimitive_DefaultValue
 * @pre A 4 entry std::map<int32_t, std::string> is serialized and the destination holds one entry.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 2 does not throw and leaves the destination empty.
 */
TEST_P(XCdrTypeConsistencyTest, StdMapNonPrimitive_DefaultValue)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::map<int32_t, std::string> output {{99, "prefill"}};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());
}

/*!
 * @brief Checks that @c TRIM keeps the leading entries of a map with non-primitive values larger than the bound.
 *
 * Entries are read in serialization order, so the entries kept are the first ones received. The case is repeated
 * for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdMapNonPrimitive_Trim
 * @pre A 4 entry std::map<int32_t, std::string> ({1, "one"} .. {4, "four"}) is serialized.
 * @post Deserializing with @c TRIM and a bound of 2 does not throw and yields {{1, "one"}, {2, "two"}}.
 */
TEST_P(XCdrTypeConsistencyTest, StdMapNonPrimitive_Trim)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::map<int32_t, std::string> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2));
    const std::map<int32_t, std::string> expected {{1, "one"}, {2, "two"}};
    EXPECT_EQ(output, expected);
}

/*!
 * @brief Checks that discarding a whole map with string values leaves the stream just past it.
 *
 * Skipping a string value requires its length prefix, so the discarded entries must be read rather than only
 * stepped over. A trailer serialized after the map makes a wrong stream position visible. The case is repeated
 * for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdMapNonPrimitive_DefaultValueConsumesWholeMap
 * @pre A 4 entry std::map<int32_t, std::string> followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 2 does not throw, leaves the destination empty, and the
 * trailer is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdMapNonPrimitive_DefaultValueConsumesWholeMap)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::map<int32_t, std::string> output {{0, "x"}};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

/*!
 * @brief Checks that truncating a map with string values leaves the stream just past it.
 *
 * The entries dropped must be consumed by exactly the bytes they occupy, which is observable only by reading what
 * was serialized next. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdMapNonPrimitive_TrimConsumesWholeMap
 * @pre A 4 entry std::map<int32_t, std::string> followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c TRIM and a bound of 2 does not throw, yields {{1, "one"}, {2, "two"}}, and the
 * trailer is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdMapNonPrimitive_TrimConsumesWholeMap)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::map<int32_t, std::string> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2));
    const std::map<int32_t, std::string> expected {{1, "one"}, {2, "two"}};
    EXPECT_EQ(output, expected);

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}


// =====================================================================================
// std::vector<struct with a bool member>
// =====================================================================================

/*!
 * @brief Checks that a sequence of structures shorter than the bound is deserialized unchanged.
 *
 * @c InnerBoolStruct holds a @c bool and a @c uint16_t , so this group also covers the boolean and the alignment
 * handling inside a non-primitive element. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorBoolStruct_Fit
 * @pre A 2 element std::vector<InnerBoolStruct> is serialized with the encoding and endianness under test.
 * @post Deserializing with @c FAIL and a bound of 10 does not throw and yields the original sequence.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorBoolStruct_Fit)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<InnerBoolStruct> input {{true, 11}, {false, 22}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<InnerBoolStruct> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

/*!
 * @brief Checks that @c FAIL rejects a sequence of structures longer than the bound.
 *
 * @c FAIL reports the inconsistency rather than delivering a partial sequence. The case is repeated for XCDRv1 and
 * XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorBoolStruct_Fail
 * @pre A 4 element std::vector<InnerBoolStruct> is serialized.
 * @post Deserializing with @c FAIL and a bound of 2 throws @c BadParamException .
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorBoolStruct_Fail)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<InnerBoolStruct> input {{true, 11}, {false, 22}, {true, 33}, {false, 44}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<InnerBoolStruct> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 2), BadParamException);
}

/*!
 * @brief Checks that @c DEFAULT_VALUE discards a sequence of structures longer than the bound.
 *
 * Discarding an element reads its @c bool member without storing it, which is a dedicated path in the boolean
 * decoder: an ordinary read validates the byte and rejects anything other than 0 or 1, whereas a discarded one
 * only has to advance the stream. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorBoolStruct_DefaultValue
 * @pre A 4 element std::vector<InnerBoolStruct> is serialized and the destination holds one element.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 2 does not throw and leaves the destination empty.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorBoolStruct_DefaultValue)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<InnerBoolStruct> input {{true, 11}, {false, 22}, {true, 33}, {false, 44}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<InnerBoolStruct> output {{true, 99}};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());
}

/*!
 * @brief Checks that @c TRIM keeps the leading elements of a sequence of structures longer than the bound.
 *
 * The elements beyond the bound are discarded, which exercises reading their @c bool member without storing it.
 * The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorBoolStruct_Trim
 * @pre A 4 element std::vector<InnerBoolStruct> is serialized.
 * @post Deserializing with @c TRIM and a bound of 2 does not throw and yields the first two elements unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorBoolStruct_Trim)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<InnerBoolStruct> input {{true, 11}, {false, 22}, {true, 33}, {false, 44}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<InnerBoolStruct> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2));
    const std::vector<InnerBoolStruct> expected {{true, 11}, {false, 22}};
    EXPECT_EQ(output, expected);
}

/*!
 * @brief Checks that discarding a whole sequence leaves the stream just past it.
 *
 * Inspecting only the destination cannot tell whether the discarded elements were consumed by the right number of
 * bytes, because an empty result is expected either way. Serializing a known trailer after the sequence turns a
 * wrong stream position into a visible failure. The case is repeated for XCDRv1 and XCDRv2, in big and little
 * endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorBoolStruct_DefaultValueConsumesWholeSequence
 * @pre A 4 element std::vector<InnerBoolStruct> followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 2 does not throw, leaves the destination empty, and the
 * trailer is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorBoolStruct_DefaultValueConsumesWholeSequence)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<InnerBoolStruct> input {{true, 11}, {false, 22}, {true, 33}, {false, 44}};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<InnerBoolStruct> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

/*!
 * @brief Checks that truncating a sequence leaves the stream just past it.
 *
 * The elements kept are verified by the other @c TRIM cases; what this one adds is that the elements dropped were
 * consumed by exactly the bytes they occupy, which is observable only by reading what was serialized next. The
 * case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorBoolStruct_TrimConsumesWholeSequence
 * @pre A 4 element std::vector<InnerBoolStruct> followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c TRIM and a bound of 2 does not throw, yields the first two elements, and the trailer
 * is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorBoolStruct_TrimConsumesWholeSequence)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<InnerBoolStruct> input {{true, 11}, {false, 22}, {true, 33}, {false, 44}};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<InnerBoolStruct> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2));
    const std::vector<InnerBoolStruct> expected {{true, 11}, {false, 22}};
    EXPECT_EQ(output, expected);

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

// =====================================================================================
// Discarded elements whose size is only known by decoding them
// =====================================================================================

/*!
 * @brief Checks that discarding a whole sequence of sequences leaves the stream just past it.
 *
 * Each discarded element starts with its own length, which must be decoded to know how many bytes it occupies.
 * A trailer serialized after the sequence makes a wrong stream position visible.
 * The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorNestedSequence_DefaultValueConsumesWholeSequence
 * @pre A 4 element std::vector<std::vector<int32_t>> followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 2 does not throw, leaves the destination empty, and the
 * trailer is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorNestedSequence_DefaultValueConsumesWholeSequence)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::vector<int32_t>> input {{1, 2}, {3}, {4, 5, 6}, {7, 8, 9, 10}};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<std::vector<int32_t>> output {{0}};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

/*!
 * @brief Checks that truncating a sequence of sequences leaves the stream just past it.
 *
 * Each discarded element starts with its own length, which must be decoded to know how many bytes it occupies.
 * A trailer serialized after the sequence makes a wrong stream position visible.
 * The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorNestedSequence_TrimConsumesWholeSequence
 * @pre A 4 element std::vector<std::vector<int32_t>> followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c TRIM and a bound of 2 does not throw, yields the first two elements, and the trailer
 * is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorNestedSequence_TrimConsumesWholeSequence)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::vector<int32_t>> input {{1, 2}, {3}, {4, 5, 6}, {7, 8, 9, 10}};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<std::vector<int32_t>> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2));
    const std::vector<std::vector<int32_t>> expected {{1, 2}, {3}};
    EXPECT_EQ(output, expected);

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

/*!
 * @brief Checks that discarding a whole sequence of mutable structures leaves the stream just past it.
 *
 * Discarded elements are mutable structures holding a string, so skipping them requires decoding their member
 * headers and the string length. A trailer serialized after the sequence makes a wrong stream position visible.
 * The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorMutableStruct_DefaultValueConsumesWholeSequence
 * @pre A 4 element std::vector<InnerMutableStruct> followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 2 does not throw, leaves the destination empty, and the
 * trailer is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorMutableStruct_DefaultValueConsumesWholeSequence)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<InnerMutableStruct> input {{"alpha", 1}, {"beta", 2}, {"gamma", 3}, {"delta", 4}};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<InnerMutableStruct> output {{"x", 0}};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

/*!
 * @brief Checks that truncating a sequence of mutable structures leaves the stream just past it.
 *
 * Discarded elements are mutable structures holding a string, so skipping them requires decoding their member
 * headers and the string length. A trailer serialized after the sequence makes a wrong stream position visible.
 * The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorMutableStruct_TrimConsumesWholeSequence
 * @pre A 4 element std::vector<InnerMutableStruct> followed by a uint32_t trailer are serialized.
 * @post Deserializing with @c TRIM and a bound of 2 does not throw, yields {{"alpha", 1}, {"beta", 2}}, and the
 * trailer is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorMutableStruct_TrimConsumesWholeSequence)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<InnerMutableStruct> input {{"alpha", 1}, {"beta", 2}, {"gamma", 3}, {"delta", 4}};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<InnerMutableStruct> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2));
    const std::vector<InnerMutableStruct> expected {{"alpha", 1}, {"beta", 2}};
    EXPECT_EQ(output, expected);

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

/*!
 * @brief Checks that discarding a whole sequence of structures with an optional member leaves the stream just past
 * it.
 *
 * The discarded elements alternate between having and not having the optional value, which is only known by
 * decoding the presence flag (XCDRv2) or the member header size (XCDRv1). A trailer serialized after the sequence
 * makes a wrong stream position visible. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorOptionalStruct_DefaultValueConsumesWholeSequence
 * @pre A 4 element std::vector<InnerOptionalStruct>, two of them without the optional value, followed by a uint32_t
 * trailer are serialized.
 * @post Deserializing with @c DEFAULT_VALUE and a bound of 2 does not throw, leaves the destination empty, and the
 * trailer is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorOptionalStruct_DefaultValueConsumesWholeSequence)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<InnerOptionalStruct> input {{optional<int32_t>{}, 1}, {7, 2}, {optional<int32_t>{}, 3}, {9, 4}};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<InnerOptionalStruct> output {{5, 0}};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

/*!
 * @brief Checks that truncating a sequence of structures with an optional member leaves the stream just past it.
 *
 * The discarded elements are one without and one with the optional value, which is only known by decoding the
 * presence flag (XCDRv2) or the member header size (XCDRv1). A trailer serialized after the sequence makes a wrong
 * stream position visible. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorOptionalStruct_TrimConsumesWholeSequence
 * @pre A 4 element std::vector<InnerOptionalStruct>, two of them without the optional value, followed by a uint32_t
 * trailer are serialized.
 * @post Deserializing with @c TRIM and a bound of 2 does not throw, yields the first two elements, and the trailer
 * is read back unchanged.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorOptionalStruct_TrimConsumesWholeSequence)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<InnerOptionalStruct> input {{optional<int32_t>{}, 1}, {7, 2}, {optional<int32_t>{}, 3}, {9, 4}};
    const uint32_t trailer {0xCAFEBABEu};
    serialize_into_with_trailer(buffer, sizeof(buffer), input, trailer, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<InnerOptionalStruct> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2));
    const std::vector<InnerOptionalStruct> expected {{optional<int32_t>{}, 1}, {7, 2}};
    EXPECT_EQ(output, expected);

    uint32_t read_trailer {0};
    ASSERT_NO_THROW(cdr_des.deserialize(read_trailer));
    EXPECT_EQ(read_trailer, trailer);
}

// =====================================================================================
// FAIL leaves the stream and the destination unchanged
// =====================================================================================

/*!
 * @brief Checks that @c FAIL leaves the stream where it was before the call for a std::string.
 *
 * Reading the same value again with a policy that accepts it proves the stream was not advanced. The case is
 * repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdString_FailLeavesStreamUnchanged
 * @pre An 11 character string ("hello world") is serialized.
 * @post Deserializing with @c FAIL and a bound of 5 throws @c BadParamException , and deserializing again with
 * @c TRIM and a bound of 5 yields "hello".
 */
TEST_P(XCdrTypeConsistencyTest, StdString_FailLeavesStreamUnchanged)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::string output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 5), BadParamException);
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 5));
    EXPECT_EQ(output, "hello");
}

/*!
 * @brief Checks that @c FAIL leaves the stream where it was before the call for a sequence of non-primitives.
 *
 * Reading the same value again with a policy that accepts it proves the stream was not advanced. The case is
 * repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdVectorNonPrimitive_FailLeavesStreamUnchanged
 * @pre A 4 element std::vector<std::string> ({"alpha", "beta", "gamma", "delta"}) is serialized.
 * @post Deserializing with @c FAIL and a bound of 2 throws @c BadParamException , and deserializing again with
 * @c TRIM and a bound of 2 yields {"alpha", "beta"}.
 */
TEST_P(XCdrTypeConsistencyTest, StdVectorNonPrimitive_FailLeavesStreamUnchanged)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"alpha", "beta", "gamma", "delta"};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    std::vector<std::string> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 2), BadParamException);
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2));
    const std::vector<std::string> expected {"alpha", "beta"};
    EXPECT_EQ(output, expected);
}

/*!
 * @brief Checks that @c FAIL leaves both the stream and the destination unchanged for a map.
 *
 * The destination map is only replaced when the whole deserialization succeeds, and reading the same value again
 * with a policy that accepts it proves the stream was not advanced. The case is repeated for XCDRv1 and XCDRv2,
 * in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdMapNonPrimitive_FailLeavesStreamAndDestinationUnchanged
 * @pre A 4 entry std::map<int32_t, std::string> is serialized and the destination holds one entry.
 * @post Deserializing with @c FAIL and a bound of 2 throws @c BadParamException and keeps the destination entry,
 * and deserializing again with @c TRIM and a bound of 2 yields {{1, "one"}, {2, "two"}}.
 */
TEST_P(XCdrTypeConsistencyTest, StdMapNonPrimitive_FailLeavesStreamAndDestinationUnchanged)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    const std::map<int32_t, std::string> previous {{0, "x"}};
    std::map<int32_t, std::string> output {previous};
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::FAIL, 2), BadParamException);
    EXPECT_EQ(output, previous);

    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2));
    const std::map<int32_t, std::string> expected {{1, "one"}, {2, "two"}};
    EXPECT_EQ(output, expected);
}

/*!
 * @brief Checks that a map with primitive values is left unchanged when the deserialization fails midway.
 *
 * The buffer is cut inside the second entry, so the first entry is read before the failure. The destination
 * must not receive it. The case is repeated for XCDRv1 and XCDRv2, in big and little endian.
 *
 * @test XCdrTypeConsistencyTest.StdMapPrimitive_ErrorKeepsDestination
 * @pre A 2 entry std::map<int32_t, int32_t> is serialized, the buffer is cut inside the second entry, and the
 * destination holds one entry.
 * @post Deserializing with @c TRIM and a bound of 2 throws @c NotEnoughMemoryException and keeps the destination
 * entry.
 */
TEST_P(XCdrTypeConsistencyTest, StdMapPrimitive_ErrorKeepsDestination)
{
    const EncodingAlgorithmFlag encoding {std::get<0>(GetParam())};
    const Cdr::Endianness endianness {std::get<1>(GetParam())};

    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, int32_t> input {{1, 10}, {2, 20}};
    serialize_into(buffer, sizeof(buffer), input, encoding, endianness);

    // Length (4 bytes), first entry (8 bytes) and the key of the second entry (4 bytes).
    FastBuffer fast_buffer(buffer, 16);
    Cdr cdr_des(fast_buffer, endianness, get_version_from_algorithm(encoding));
    cdr_des.set_encoding_flag(encoding);

    const std::map<int32_t, int32_t> previous {{0, 0}};
    std::map<int32_t, int32_t> output {previous};
    EXPECT_THROW(cdr_des.deserialize(output, CdrTryConstructFlag::TRIM, 2),
            eprosima::fastcdr::exception::NotEnoughMemoryException);
    EXPECT_EQ(output, previous);
}


INSTANTIATE_TEST_SUITE_P(
    XCdrTest,
    XCdrTypeConsistencyTest,
    ::testing::Values(
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR, Cdr::Endianness::LITTLE_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR2, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR2, Cdr::Endianness::LITTLE_ENDIANNESS)
        ));


// =====================================================================================
// XCDRv2 DHEADER consistency
// =====================================================================================

namespace {

//! Overwrites the little endian DHEADER at the start of @p buffer .
void patch_dheader(
        char* buffer,
        uint32_t dheader)
{
    buffer[0] = static_cast<char>(dheader & 0xFF);
    buffer[1] = static_cast<char>((dheader >> 8) & 0xFF);
    buffer[2] = static_cast<char>((dheader >> 16) & 0xFF);
    buffer[3] = static_cast<char>((dheader >> 24) & 0xFF);
}

//! Deserializes @p buffer as XCDRv2 little endian into @p output with @p flag and @p max_length .
template<typename T>
void deserialize_xcdrv2(
        char* buffer,
        size_t buffer_size,
        T& output,
        CdrTryConstructFlag flag,
        uint32_t max_length)
{
    FastBuffer fast_buffer(buffer, buffer_size);
    Cdr cdr_des(fast_buffer, Cdr::Endianness::LITTLE_ENDIANNESS, CdrVersion::XCDRv2);
    cdr_des.set_encoding_flag(EncodingAlgorithmFlag::PLAIN_CDR2);
    cdr_des.deserialize(output, flag, max_length);
}

} // namespace

/*!
 * @brief Checks that a sequence whose DHEADER is too small for the elements it announces is rejected.
 *
 * Layout of {"a", "b"}: DHEADER at 0, length at 4, "a" at 8 (length) and 12 (characters), "b" at 16 and 20. A
 * DHEADER of 10 covers the length and the first string only, so the second element cannot be read from it. It
 * must not be silently left default-constructed.
 *
 * @test XCdrTypeConsistencyDheaderTest.StdVectorNonPrimitive_DheaderTooSmall
 * @pre A 2 element std::vector<std::string> is serialized with XCDRv2 and its DHEADER is patched to 10.
 * @post Deserializing with @c TRIM and a bound of 2 throws @c BadParamException .
 */
TEST(XCdrTypeConsistencyDheaderTest, StdVectorNonPrimitive_DheaderTooSmall)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"a", "b"};
    serialize_into(buffer, sizeof(buffer), input, EncodingAlgorithmFlag::PLAIN_CDR2,
            Cdr::Endianness::LITTLE_ENDIANNESS);
    patch_dheader(buffer, 10);

    std::vector<std::string> output;
    EXPECT_THROW(deserialize_xcdrv2(buffer, sizeof(buffer), output, CdrTryConstructFlag::TRIM, 2),
            BadParamException);
}

/*!
 * @brief Checks that a truncated sequence whose kept elements overrun the DHEADER is rejected.
 *
 * With the layout of {"a", "b"} (see StdVectorNonPrimitive_DheaderTooSmall), a DHEADER of 6 ends inside the first
 * string, which is the element kept when trimming to one.
 *
 * @test XCdrTypeConsistencyDheaderTest.StdVectorNonPrimitive_TrimOverrunsDheader
 * @pre A 2 element std::vector<std::string> is serialized with XCDRv2 and its DHEADER is patched to 6.
 * @post Deserializing with @c TRIM and a bound of 1 throws @c BadParamException .
 */
TEST(XCdrTypeConsistencyDheaderTest, StdVectorNonPrimitive_TrimOverrunsDheader)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"a", "b"};
    serialize_into(buffer, sizeof(buffer), input, EncodingAlgorithmFlag::PLAIN_CDR2,
            Cdr::Endianness::LITTLE_ENDIANNESS);
    patch_dheader(buffer, 6);

    std::vector<std::string> output;
    EXPECT_THROW(deserialize_xcdrv2(buffer, sizeof(buffer), output, CdrTryConstructFlag::TRIM, 1),
            BadParamException);
}

/*!
 * @brief Checks that a map whose DHEADER is too small for the entries it announces is rejected.
 *
 * Layout of {{1, "a"}, {2, "b"}}: DHEADER at 0, length at 4, first key at 8 and value at 12 (length) and 16
 * (characters), second key at 20. A DHEADER of 14 covers the length and the first entry only.
 *
 * @test XCdrTypeConsistencyDheaderTest.StdMapNonPrimitive_DheaderTooSmall
 * @pre A 2 entry std::map<int32_t, std::string> is serialized with XCDRv2 and its DHEADER is patched to 14.
 * @post Deserializing with @c TRIM and a bound of 2 throws @c BadParamException .
 */
TEST(XCdrTypeConsistencyDheaderTest, StdMapNonPrimitive_DheaderTooSmall)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "a"}, {2, "b"}};
    serialize_into(buffer, sizeof(buffer), input, EncodingAlgorithmFlag::PLAIN_CDR2,
            Cdr::Endianness::LITTLE_ENDIANNESS);
    patch_dheader(buffer, 14);

    std::map<int32_t, std::string> output;
    EXPECT_THROW(deserialize_xcdrv2(buffer, sizeof(buffer), output, CdrTryConstructFlag::TRIM, 2),
            BadParamException);
}

/*!
 * @brief Checks that a truncated map whose kept entries overrun the DHEADER is rejected.
 *
 * With the layout of {{1, "a"}, {2, "b"}} (see StdMapNonPrimitive_DheaderTooSmall), a DHEADER of 6 ends inside
 * the first entry, which is the entry kept when trimming to one.
 *
 * @test XCdrTypeConsistencyDheaderTest.StdMapNonPrimitive_TrimOverrunsDheader
 * @pre A 2 entry std::map<int32_t, std::string> is serialized with XCDRv2 and its DHEADER is patched to 6.
 * @post Deserializing with @c TRIM and a bound of 1 throws @c BadParamException .
 */
TEST(XCdrTypeConsistencyDheaderTest, StdMapNonPrimitive_TrimOverrunsDheader)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "a"}, {2, "b"}};
    serialize_into(buffer, sizeof(buffer), input, EncodingAlgorithmFlag::PLAIN_CDR2,
            Cdr::Endianness::LITTLE_ENDIANNESS);
    patch_dheader(buffer, 6);

    std::map<int32_t, std::string> output;
    EXPECT_THROW(deserialize_xcdrv2(buffer, sizeof(buffer), output, CdrTryConstructFlag::TRIM, 1),
            BadParamException);
}
