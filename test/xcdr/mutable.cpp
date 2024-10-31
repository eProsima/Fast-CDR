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

#include <array>
#include <memory>
#include <tuple>
#include <vector>

#include <gtest/gtest.h>

#include <fastcdr/Cdr.h>
#include "utility.hpp"

using namespace eprosima::fastcdr;

using XCdrStreamValues =
        std::array<std::vector<uint8_t>,
                1 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS>;


class XCdrMutableTest : public ::testing::TestWithParam< std::tuple<EncodingAlgorithmFlag, Cdr::Endianness>>
{
};

struct MutableExtraElement
{
public:

    MutableExtraElement() = default;

    MutableExtraElement(
            uint8_t value)
    {
        value1 = value;
        value2 = value;
        value3 = value;
    }

    bool operator ==(
            const MutableExtraElement& other) const
    {
        return value1 == other.value1 && value2 == other.value2 && value3 != other.value3;
    }

    //! First being serialized.
    uint32_t value1 {0};

    //! Third being serialized.
    uint16_t value2 {0};

    //! Second being serialized. Not know by deserialization.
    uint8_t value3 {0};
};

namespace eprosima {
namespace fastcdr {

template<>
void serialize(
        Cdr& cdr,
        const MutableExtraElement& data)
{
    Cdr::state current_status(cdr);
    cdr.begin_serialize_type(current_status, cdr.get_encoding_flag());
    cdr << MemberId(3) << data.value1;
    cdr << MemberId(0x3FFF) << data.value3;
    cdr << MemberId(16) << data.value2;
    cdr.end_serialize_type(current_status);
}

template<>
void deserialize(
        Cdr& cdr,
        MutableExtraElement& data)
{
    cdr.deserialize_type(cdr.get_encoding_flag(), [&data](Cdr& cdr_inner, const MemberId& mid) -> bool
            {
                bool ret_value = true;
                switch (mid.id)
                {
                    case 16:
                        cdr_inner >> data.value2;
                        break;
                    case 3:
                        cdr_inner >> data.value1;
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

struct MutableLessElement
{
public:

    MutableLessElement() = default;

    MutableLessElement(
            uint8_t value)
    {
        value1 = value;
        value2 = value;
        value3 = value;
    }

    bool operator ==(
            const MutableLessElement& other) const
    {
        return value1 == other.value1 && value2 == other.value2 && value3 != other.value3;
    }

    //! First being serialized.
    uint32_t value1 {0};

    //! Third being serialized.
    uint16_t value2 {0};

    //! Second being serialized. Not know by deserialization.
    uint8_t value3 {0};
};

namespace eprosima {
namespace fastcdr {

template<>
void serialize(
        Cdr& cdr,
        const MutableLessElement& data)
{
    Cdr::state current_status(cdr);
    cdr.begin_serialize_type(current_status, cdr.get_encoding_flag());
    cdr << MemberId(3) << data.value1;
    cdr << MemberId(16) << data.value2;
    cdr.end_serialize_type(current_status);
}

template<>
void deserialize(
        Cdr& cdr,
        MutableLessElement& data)
{
    cdr.deserialize_type(cdr.get_encoding_flag(), [&data](Cdr& cdr_inner, const MemberId& mid) -> bool
            {
                bool ret_value = true;
                switch (mid.id)
                {
                    case 16:
                        cdr_inner >> data.value2;
                        break;
                    case 3:
                        cdr_inner >> data.value1;
                        break;
                    case 0x3FFF:
                        cdr_inner >> data.value3;
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

struct MuInnerStructure
{
public:

    MuInnerStructure() = default;

    MuInnerStructure(
            eprosima::fastcdr::EncodingAlgorithmFlag e1,
            eprosima::fastcdr::EncodingAlgorithmFlag e2
            )
        : enc_xcdrv1(e1)
        , enc_xcdrv2(e2)
    {
    }

    MuInnerStructure(
            eprosima::fastcdr::EncodingAlgorithmFlag e1,
            eprosima::fastcdr::EncodingAlgorithmFlag e2,
            uint8_t value
            )
        : value1(value)
        , enc_xcdrv1(e1)
        , enc_xcdrv2(e2)
    {
    }

    bool operator ==(
            const MuInnerStructure& other) const
    {
        return value1 == other.value1 &&
               value2.has_value() == other.value2.has_value() &&
               (!value2.has_value() || value2.value() == other.value2.value());
    }

    //! First being serialized.
    uint32_t value1 {0};

    //! Second being serialized.
    eprosima::fastcdr::optional<uint32_t> value2;

    eprosima::fastcdr::EncodingAlgorithmFlag enc_xcdrv1 {eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR};

    eprosima::fastcdr::EncodingAlgorithmFlag enc_xcdrv2 {eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR2};
};

namespace eprosima {
namespace fastcdr {

template<>
void serialize(
        Cdr& cdr,
        const MuInnerStructure& data)
{
    Cdr::state current_status(cdr);
    cdr.begin_serialize_type(current_status, cdr.get_cdr_version() == eprosima::fastcdr::CdrVersion::XCDRv1
                             ? data.enc_xcdrv1
                             : data.enc_xcdrv2);
    cdr << MemberId(3) << data.value1;
    cdr << MemberId(16) << data.value2;
    cdr.end_serialize_type(current_status);
}

template<>
void deserialize(
        Cdr& cdr,
        MuInnerStructure& data)
{
    cdr.deserialize_type(cdr.get_cdr_version() == eprosima::fastcdr::CdrVersion::XCDRv1
                         ? data.enc_xcdrv1
                         : data.enc_xcdrv2,
            [&data](Cdr& cdr_inner, const MemberId& mid) -> bool
            {
                bool ret_value {true};
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
 * @test Test a mutable structure where the encoded version has more members that the decoded one.
 * @code{.idl}
 * @mutable
 * struct MutableStruct // Encoded version
 * {
 *     @id(3)
 *     unsigned long value1;
 *     @id(16383)
 *     octet value3;
 *     @id(16)
 *     unsigned short value2;
 * };
 *
 * @mutable
 * struct MutableStruct // Decoded version
 * {
 *     @id(16)
 *     unsigned short value2;
 *     @id(3)
 *     unsigned long value1;
 * };
 * @endcode
 */
TEST_P(XCdrMutableTest, unordered_and_more_serialized_elements)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x03, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, ival, // ULong
        0x3F, 0x01, 0x00, 0x08, // LongMemberHeader
        0x00, 0x00, 0x3F, 0xFF, // LongMemberHeader
        0x00, 0x00, 0x00, 0x01, // LongMemberHeader
        ival,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x10, 0x00, 0x02, // ShortMemberHeader
        0x00, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x03, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, 0x00, 0x00, 0x00, // ULong
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0xFF, 0x3F, 0x00, 0x00, // LongMemberHeader
        0x01, 0x00, 0x00, 0x00, // LongMemberHeader
        ival,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x02, 0x00, // ShortMemberHeader
        ival, 0x00,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x16, // DHEADER
        0x20, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        ival,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x00, ival,             // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x16, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        ival, 0x00, 0x00, 0x00, // ULong
        0xFF, 0x3F, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        ival,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        ival, 0x00,             // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode
    uint32_t value1 { ival };
    uint16_t value2 { ival };
    uint8_t value3 { ival };
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(3) << value1;
    cdr << MemberId(0x3FFF) << value3;
    cdr << MemberId(16) << value2;
    cdr.end_serialize_type(enc_state);
    cdr.set_dds_cdr_options({0, 0});
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(),
            expected_streams[tested_stream].size()));
    //}

    //{ Decoding
    uint32_t dvalue1 {0};
    uint16_t dvalue2 {0};
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
            {
                bool ret_value = true;

                switch (mid.id)
                {
                    case 16:
                        cdr_inner >> dvalue2;
                        break;
                    case 3:
                        cdr_inner >> dvalue1;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    ASSERT_EQ(value1, dvalue1);
    ASSERT_EQ(value2, dvalue2);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

/*!
 * @test Test a mutable structure where the encoded version has less members that the decoded one.
 * @code{.idl}
 * @mutable
 * struct MutableStruct // Encoded version
 * {
 *     @id(3)
 *     unsigned long value1;
 *     @id(16)
 *     unsigned short value2;
 * };
 *
 * @mutable
 * struct MutableStruct // Decoded version
 * {
 *     @id(16)
 *     unsigned short value2;
 *     @id(16383)
 *     octet value3;
 *     @id(3)
 *     unsigned long value1;
 * };
 * @endcode
 */
TEST_P(XCdrMutableTest, unordered_and_less_serialized_elements)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x03, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x10, 0x00, 0x02, // ShortMemberHeader
        0x00, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x03, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, 0x00, 0x00, 0x00, // ULong
        0x10, 0x00, 0x02, 0x00, // ShortMemberHeader
        ival, 0x00,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x20, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, ival, // ULong
        0x10, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x00, ival,             // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        ival, 0x00, 0x00, 0x00, // ULong
        0x10, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        ival, 0x00,             // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode
    uint32_t value1 { ival };
    uint16_t value2 { ival };
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(3) << value1;
    cdr << MemberId(16) << value2;
    cdr.end_serialize_type(enc_state);
    cdr.set_dds_cdr_options({0, 0});
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(),
            expected_streams[tested_stream].size()));
    //}

    //{ Decoding
    uint32_t dvalue1 {0};
    uint16_t dvalue2 {0};
    uint8_t dvalue3 {0};
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
            {
                bool ret_value = true;

                switch (mid.id)
                {
                    case 16:
                        cdr_inner >> dvalue2;
                        break;
                    case 0x3FFF:
                        cdr_inner >> dvalue3;
                        break;
                    case 3:
                        cdr_inner >> dvalue1;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    ASSERT_EQ(value1, dvalue1);
    ASSERT_EQ(value2, dvalue2);
    ASSERT_EQ(0, dvalue3);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}


/*!
 * @test Test an inner mutable structure where the encoded version has more members that the decoded one.
 * @code{.idl}
 * @mutable
 * struct MutableExtraElement // Encoded version
 * {
 *     @id(3)
 *     unsigned long value1;
 *     @id(16383)
 *     octet value3;
 *     @id(16)
 *     unsigned short value2;
 * };
 *
 * @mutable
 * struct MutableExtraElement // Decoded version
 * {
 *     @id(16)
 *     unsigned short value2;
 *     @id(3)
 *     unsigned long value1;
 * };
 *
 * @mutable
 * struct MutableInnerStruct
 * {
 *     @id(1)
 *     MutableExtraElement value1;
 *     @id(2)
 *     MutableExtraElement value2;
 * };
 * @endcode
 */
TEST_P(XCdrMutableTest, inner_unordered_and_more_serialized_elements)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x24, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, ival, // ULong
        0x3F, 0x01, 0x00, 0x08, // LongMemberHeader
        0x00, 0x00, 0x3F, 0xFF, // LongMemberHeader
        0x00, 0x00, 0x00, 0x01, // LongMemberHeader
        ival,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x10, 0x00, 0x02, // ShortMemberHeader
        0x00, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x00, 0x02, 0x00, 0x24, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, ival, // ULong
        0x3F, 0x01, 0x00, 0x08, // LongMemberHeader
        0x00, 0x00, 0x3F, 0xFF, // LongMemberHeader
        0x00, 0x00, 0x00, 0x01, // LongMemberHeader
        ival,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x10, 0x00, 0x02, // ShortMemberHeader
        0x00, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x24, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, 0x00, 0x00, 0x00, // ULong
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0xFF, 0x3F, 0x00, 0x00, // LongMemberHeader
        0x01, 0x00, 0x00, 0x00, // LongMemberHeader
        ival,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x02, 0x00, // ShortMemberHeader
        ival, 0x00,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x02, 0x00, 0x24, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, 0x00, 0x00, 0x00, // ULong
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0xFF, 0x3F, 0x00, 0x00, // LongMemberHeader
        0x01, 0x00, 0x00, 0x00, // LongMemberHeader
        ival,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x02, 0x00, // ShortMemberHeader
        ival, 0x00,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x3E, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x16, // NEXTINT + DHEADER
        0x20, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        ival,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x00, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x50, 0x00, 0x00, 0x02, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x16, // NEXTINT + DHEADER
        0x20, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        ival,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x00, ival,             // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x3E, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x16, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
        0x03, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        ival, 0x00, 0x00, 0x00, // ULong
        0xFF, 0x3F, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        ival,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        ival, 0x00,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x16, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
        0x03, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        ival, 0x00, 0x00, 0x00, // ULong
        0xFF, 0x3F, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        ival,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        ival, 0x00,             // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode
    MutableExtraElement value { ival };
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(1) << value;
    cdr << MemberId(2) << value;
    cdr.end_serialize_type(enc_state);
    cdr.set_dds_cdr_options({0, 0});
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(),
            expected_streams[tested_stream].size()));
    //}

    //{ Decoding
    MutableExtraElement dvalue1, dvalue2;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
            {
                bool ret_value = true;

                switch (mid.id)
                {
                    case 1:
                        cdr_inner >> dvalue1;
                        break;
                    case 2:
                        cdr_inner >> dvalue2;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    ASSERT_EQ(value, dvalue1);
    ASSERT_EQ(value, dvalue2);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

/*!
 * @test Test an inner mutable structure where the encoded version has more members that the decoded one.
 * @code{.idl}
 * @mutable
 * struct MutableLessElement // Encoded version
 * {
 *     @id(3)
 *     unsigned long value1;
 *     @id(16)
 *     unsigned short value2;
 * };
 *
 * @mutable
 * struct MutableLessElement // Decoded version
 * {
 *     @id(16)
 *     unsigned short value2;
 *     @id(3)
 *     unsigned long value1;
 *     @id(16383)
 *     octet value3;
 * };
 *
 * @mutable
 * struct MutableInnerStruct
 * {
 *     @id(1)
 *     MutableLessElement value1;
 *     @id(2)
 *     MutableLessElement value2;
 * };
 * @endcode
 */
TEST_P(XCdrMutableTest, inner_unordered_and_less_serialized_elements)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x14, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x10, 0x00, 0x02, // ShortMemberHeader
        0x00, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x00, 0x02, 0x00, 0x14, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x10, 0x00, 0x02, // ShortMemberHeader
        0x00, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x14, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, 0x00, 0x00, 0x00, // ULong
        0x10, 0x00, 0x02, 0x00, // ShortMemberHeader
        ival, 0x00,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x02, 0x00, 0x14, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, 0x00, 0x00, 0x00, // ULong
        0x10, 0x00, 0x02, 0x00, // ShortMemberHeader
        ival, 0x00,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x2E, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0E, // NEXTINT + DHEADER
        0x20, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, ival, // ULong
        0x10, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x00, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x50, 0x00, 0x00, 0x02, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0E, // NEXTINT + DHEADER
        0x20, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, ival, // ULong
        0x10, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x00, ival,             // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x2E, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x0E, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
        0x03, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        ival, 0x00, 0x00, 0x00, // ULong
        0x10, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        ival, 0x00,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x0E, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
        0x03, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        ival, 0x00, 0x00, 0x00, // ULong
        0x10, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        ival, 0x00,             // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode
    MutableLessElement value { ival };
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(1) << value;
    cdr << MemberId(2) << value;
    cdr.end_serialize_type(enc_state);
    cdr.set_dds_cdr_options({0, 0});
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(),
            expected_streams[tested_stream].size()));
    //}

    //{ Decoding
    MutableLessElement dvalue1, dvalue2;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
            {
                bool ret_value = true;

                switch (mid.id)
                {
                    case 1:
                        cdr_inner >> dvalue1;
                        break;
                    case 2:
                        cdr_inner >> dvalue2;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    ASSERT_EQ(value, dvalue1);
    ASSERT_EQ(value, dvalue2);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

/*!
 * @test Test an inner final structure inside a mutable structure.
 * @code{.idl}
 * @final
 * struct InnerFinalStructure
 * {
 *     @id(3)
 *     unsigned long value1;
 *     @id(16) @optional
 *     unsigned long value2;
 * };
 *
 * @mutable
 * struct MutableWithInnerFinalStruct
 * {
 *     @id(1)
 *     unsigned long value1;
 *     @id(2)
 *     InnerFinalStructure value2;
 * };
 * @endcode
 */
TEST_P(XCdrMutableTest, inner_final_structure)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x02, 0x00, 0x08, // ShortMemberHeader
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x10, 0x00, 0x00, // ShortMemberHeader (optional)
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, 0x00, 0x00, 0x00, // ULong
        0x02, 0x00, 0x08, 0x00, // ShortMemberHeader
        ival, 0x00, 0x00, 0x00, // ULong
        0x10, 0x00, 0x00, 0x00, // ShortMemberHeader
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x15, // DHEADER
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, ival, // ULong
        0x40, 0x00, 0x00, 0x02, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x05, // NEXTINT
        0x00, 0x00, 0x00, ival, // ULong
        0x00,                   // Optional not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x15, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        ival, 0x00, 0x00, 0x00, // ULong
        0x02, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x05, 0x00, 0x00, 0x00, // NEXTINT
        ival, 0x00, 0x00, 0x00, // ULong
        0x00,                   // Optional not present
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode
    uint32_t value1 {ival};
    MuInnerStructure value2 {eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR,
                             eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR2, ival};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(1) << value1;
    cdr << MemberId(2) << value2;
    cdr.end_serialize_type(enc_state);
    cdr.set_dds_cdr_options({0, 0});
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(),
            expected_streams[tested_stream].size()));
    //}

    //{ Decoding
    uint32_t dvalue1 {0};
    MuInnerStructure dvalue2 {eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR,
                              eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR2, ival};
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
            {
                bool ret_value {true};

                switch (mid.id)
                {
                    case 1:
                        cdr_inner >> dvalue1;
                        break;
                    case 2:
                        cdr_inner >> dvalue2;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    ASSERT_EQ(value1, dvalue1);
    ASSERT_EQ(value2, dvalue2);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

/*!
 * @test Test an inner appendable structure inside a mutable structure.
 * @code{.idl}
 * @appendable
 * struct InnerAppendableStructure
 * {
 *     @id(3)
 *     unsigned long value1;
 *     @id(16) @optional
 *     unsigned long value2;
 * };
 *
 * @mutable
 * struct MutableWithInnerAppendableStruct
 * {
 *     @id(1)
 *     unsigned long value1;
 *     @id(2)
 *     InnerAppendableStructure value2;
 * };
 * @endcode
 */
TEST_P(XCdrMutableTest, inner_appendable_structure)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x02, 0x00, 0x08, // ShortMemberHeader
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x10, 0x00, 0x00, // ShortMemberHeader (optional)
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, 0x00, 0x00, 0x00, // ULong
        0x02, 0x00, 0x08, 0x00, // ShortMemberHeader
        ival, 0x00, 0x00, 0x00, // ULong
        0x10, 0x00, 0x00, 0x00, // ShortMemberHeader
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x15, // DHEADER
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, ival, // ULong
        0x50, 0x00, 0x00, 0x02, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x05, // NEXTINT + DHEADER
        0x00, 0x00, 0x00, ival, // ULong
        0x00,                   // Optional not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x15, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        ival, 0x00, 0x00, 0x00, // ULong
        0x02, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x05, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
        ival, 0x00, 0x00, 0x00, // ULong
        0x00,                   // Optional not present
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode
    uint32_t value1 {ival};
    MuInnerStructure value2 {eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR,
                             eprosima::fastcdr::EncodingAlgorithmFlag::DELIMIT_CDR2, ival};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(1) << value1;
    cdr << MemberId(2) << value2;
    cdr.end_serialize_type(enc_state);
    cdr.set_dds_cdr_options({0, 0});
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(),
            expected_streams[tested_stream].size()));
    //}

    //{ Decoding
    uint32_t dvalue1 {0};
    MuInnerStructure dvalue2 {eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR,
                              eprosima::fastcdr::EncodingAlgorithmFlag::DELIMIT_CDR2, ival};
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
            {
                bool ret_value {true};

                switch (mid.id)
                {
                    case 1:
                        cdr_inner >> dvalue1;
                        break;
                    case 2:
                        cdr_inner >> dvalue2;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    ASSERT_EQ(value1, dvalue1);
    ASSERT_EQ(value2, dvalue2);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

INSTANTIATE_TEST_SUITE_P(
    XCdrTest,
    XCdrMutableTest,
    ::testing::Values(
        std::make_tuple(EncodingAlgorithmFlag::PL_CDR, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PL_CDR, Cdr::Endianness::LITTLE_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PL_CDR2, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PL_CDR2, Cdr::Endianness::LITTLE_ENDIANNESS)
        ));

