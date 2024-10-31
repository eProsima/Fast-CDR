// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <tuple>
#include <vector>

#include <fastcdr/Cdr.h>
#include <fastcdr/CdrSizeCalculator.hpp>

using namespace eprosima::fastcdr;

using XCdrStreamValues =
        std::array<std::vector<unsigned char>,
                1 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS>;

using XCdrPlStreamValues =
        std::array<std::vector<unsigned char>,
                1 + Cdr::XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT>;

class XCdrv1Test : public ::testing::TestWithParam<std::tuple<EncodingAlgorithmFlag, CdrVersion>>
{
};

class XCdrv1PLTest : public ::testing::TestWithParam< Cdr::XCdrHeaderSelection>
{
};

template<class _T>
void xcdrv1_serialize_the_value(
        const XCdrPlStreamValues& expected_streams,
        Cdr::XCdrHeaderSelection header_selection,
        _T value)
{
    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = EncodingAlgorithmFlag::PL_CDR;
    Cdr::Endianness endianness = Cdr::Endianness::LITTLE_ENDIANNESS;
    uint8_t tested_stream = 0 + static_cast<uint8_t>(header_selection);
    size_t total_size = expected_streams[tested_stream].size() + 4; // Extra needed space for temp LongMemberHeader
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(total_size, sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), total_size);
    Cdr cdr(fast_buffer, endianness, CdrVersion::XCDRv1);
    //}

    //{
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr.serialize_member(MemberId(1), value, header_selection);
    cdr.end_serialize_type(enc_state);
    cdr.set_dds_cdr_options({0, 0});
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    _T dec_value {0};
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&dec_value](Cdr& cdr_inner, const MemberId& mid) -> bool
            {
                bool ret_value = true;
                switch (mid.id)
                {
                    case 1:
                        cdr_inner.deserialize_member(dec_value);
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    ASSERT_EQ(value, dec_value);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

/*!
 * @test Test the decoder can get the XCDR version from the encapsulation.
 * @code{.idl}
 * struct AutoSelectStruct
 * {
 *     @id(0)
 *     unsigned short value1;
 *     @id(1)
 *     unsigned long value2;
 * };
 * @endcode
 */
TEST_P(XCdrv1Test, auto_selection_on_decode)
{
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = Cdr::Endianness::LITTLE_ENDIANNESS;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(100, sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), 100);

    //{ Encode an ushort and an ulong.
    Cdr cdr(fast_buffer, endianness, CdrVersion::XCDRv1);
    const uint16_t us {0x01FC};
    const uint32_t ul {0x01FC1FCD};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(0) << us;
    cdr << MemberId(1) << ul;
    cdr.end_serialize_type(enc_state);
    cdr.set_dds_cdr_options({0, 0});
    //}

    //{ Decode an ushort and an ulong.
    Cdr dcdr(fast_buffer, endianness, std::get<1>(GetParam()));
    uint16_t dus{0};
    uint32_t dul{0};
    dcdr.read_encapsulation();
    ASSERT_EQ(dcdr.get_encoding_flag(), encoding);
    ASSERT_EQ(dcdr.endianness(), endianness);
    dcdr.deserialize_type(encoding, [&](Cdr& dcdr_inner, const MemberId& mid) -> bool
            {
                bool ret_value = true;
                switch (mid.id)
                {
                    case 0:
                        dcdr_inner >> dus;
                        break;
                    case 1:
                        dcdr_inner >> dul;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    ASSERT_EQ(us, dus);
    ASSERT_EQ(ul, dul);
}

/*!
 * @test Test the encoder doesn't execute a POP(origin=0) when serializing a MMEMBER
 * @code{.idl}
 * struct AutoSelectStruct
 * {
 *     @id(0)
 *     @optional
 *     octet value1;
 *     @id(1)
 *     long long value2;
 * };
 * @endcode
 */
TEST_P(XCdrv1Test, push_origin_zero)
{
    constexpr uint8_t cval {0x1C};
    const optional<uint8_t> uc {cval};
    constexpr int64_t longlong_value {static_cast<int64_t>(0xCDCDCDCDCDCDCDDCll)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        cval,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        cval,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        cval,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval, // LongLong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        cval,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival, // LongLong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = Cdr::Endianness::LITTLE_ENDIANNESS;
    uint8_t tested_stream = 0 + encoding + endianness;

    //{ Calculate encoded size.
    CdrSizeCalculator calculator(CdrVersion::XCDRv1);
    size_t current_alignment = 0;
    size_t calculated_size = calculator.begin_calculate_type_serialized_size(encoding, current_alignment);
    calculated_size += calculator.calculate_member_serialized_size(MemberId(0), uc, current_alignment);
    calculated_size += calculator.calculate_member_serialized_size(MemberId(1), longlong_value, current_alignment);
    calculated_size += calculator.end_calculate_type_serialized_size(encoding, current_alignment);
    calculated_size += 4; // Encapsulation
    //}

    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(100, sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), 100);

    //{ Encode an ushort and an ulong.
    Cdr cdr(fast_buffer, endianness, CdrVersion::XCDRv1);
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(0) << uc;
    cdr << MemberId(1) << longlong_value;
    cdr.end_serialize_type(enc_state);
    cdr.set_dds_cdr_options({0, 0});
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
    ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decode an optional octet and a long long.
    optional<uint8_t> duc{0};
    int64_t dlonglong_value{0};
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid) -> bool
            {
                bool ret_value = true;
                switch (mid.id)
                {
                    case 0:
                        cdr_inner >> duc;
                        break;
                    case 1:
                        cdr_inner >> dlonglong_value;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    ASSERT_EQ(uc, duc);
    ASSERT_EQ(longlong_value, dlonglong_value);
}

/*!
 * @test Test serialization of an octet member changing the XCdrHeaderSelection
 * @code{.idl}
 * struct PLOctetStruct
 * {
 *     @id(1)
 *     octet value1;
 * };
 * @endcode
 */
TEST_P(XCdrv1PLTest, pl_octet_member)
{
    constexpr unsigned char octet_value = 0xCD;

    //{ Defining expected XCDR streams
    XCdrPlStreamValues expected_streams;
    expected_streams[0 + Cdr::XCdrHeaderSelection::SHORT_HEADER] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + Cdr::XCdrHeaderSelection::LONG_HEADER] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0x01, 0x00, 0x00, 0x00, // Member ID and flags
        0x01, 0x00, 0x00, 0x00, // Size
        octet_value,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + Cdr::XCdrHeaderSelection::AUTO_WITH_SHORT_HEADER_BY_DEFAULT] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + Cdr::XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    //}

    Cdr::XCdrHeaderSelection header_selection {GetParam()};
    xcdrv1_serialize_the_value(expected_streams, header_selection, octet_value);
}

/*!
 * @test Test serialization of a long member changing the XCdrHeaderSelection
 * @code{.idl}
 * struct PLLongStruct
 * {
 *     @id(1)
 *     long value1;
 * };
 * @endcode
 */
TEST_P(XCdrv1PLTest, pl_long_member)
{
    constexpr int32_t long_value {static_cast<int32_t>(0xDCCDCDCD)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrPlStreamValues expected_streams;
    expected_streams[0 + Cdr::XCdrHeaderSelection::SHORT_HEADER] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, ival, ival, fval, // Long
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + Cdr::XCdrHeaderSelection::LONG_HEADER] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0x01, 0x00, 0x00, 0x00, // Member ID and flags
        0x04, 0x00, 0x00, 0x00, // Size
        ival, ival, ival, fval, // Long
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + Cdr::XCdrHeaderSelection::AUTO_WITH_SHORT_HEADER_BY_DEFAULT] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, ival, ival, fval, // Long
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + Cdr::XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, ival, ival, fval, // Long
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    //}

    Cdr::XCdrHeaderSelection header_selection {GetParam()};
    xcdrv1_serialize_the_value(expected_streams, header_selection, long_value);
}

/*!
 * @test Test an exception is thrown when the user sets XCdrHeaderSelection::SHORT_HEADER but a long header is needed.
 * @code{.idl}
 * struct PLExceptionStruct
 * {
 *     @id(16383)
 *     long value;
 * };
 * @endcode
 */
TEST(XCdrv1PLTest, exception_long_header)
{
    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = EncodingAlgorithmFlag::PL_CDR;
    Cdr::Endianness endianness = Cdr::Endianness::LITTLE_ENDIANNESS;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(10, sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), 10);
    Cdr cdr(fast_buffer, endianness, CdrVersion::XCDRv1);
    //}

    //{
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    int32_t value {1};
    EXPECT_THROW(cdr.serialize_member(MemberId(16383), value, Cdr::XCdrHeaderSelection::SHORT_HEADER),
            exception::BadParamException);
    //}
}

INSTANTIATE_TEST_SUITE_P(
    XCdrTest,
    XCdrv1Test,
    ::testing::Values(
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR, CdrVersion::XCDRv1),
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR, CdrVersion::XCDRv2),
        std::make_tuple(EncodingAlgorithmFlag::PL_CDR, CdrVersion::XCDRv1),
        std::make_tuple(EncodingAlgorithmFlag::PL_CDR, CdrVersion::XCDRv2)
        ));

INSTANTIATE_TEST_SUITE_P(
    XCdrTest,
    XCdrv1PLTest,
    ::testing::Values(
        Cdr::XCdrHeaderSelection::SHORT_HEADER,
        Cdr::XCdrHeaderSelection::LONG_HEADER,
        Cdr::XCdrHeaderSelection::AUTO_WITH_SHORT_HEADER_BY_DEFAULT,
        Cdr::XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT
        ));
