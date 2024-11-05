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


class XCdrAppendableTest : public ::testing::TestWithParam< std::tuple<EncodingAlgorithmFlag, Cdr::Endianness>>
{
};

struct AppendableExtraElement
{
public:

    AppendableExtraElement() = default;

    AppendableExtraElement(
            uint8_t value)
    {
        value1 = value;
        value2 = value;
        value3 = value;
    }

    bool operator ==(
            const AppendableExtraElement& other) const
    {
        // 'value3' is not deserialized.
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
        const AppendableExtraElement& data)
{
    Cdr::state current_status(cdr);
    cdr.begin_serialize_type(current_status, cdr.get_encoding_flag());
    cdr << MemberId(3) << data.value1;
    cdr << MemberId(0x3FFF) << data.value2;
    cdr << MemberId(16) << data.value3;
    cdr.end_serialize_type(current_status);
}

template<>
void deserialize(
        Cdr& cdr,
        AppendableExtraElement& data)
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

struct AppendableLessElement
{
public:

    AppendableLessElement() = default;

    AppendableLessElement(
            uint8_t value)
    {
        value1 = value;
        value2 = value;
        value3 = value;
    }

    bool operator ==(
            const AppendableLessElement& other) const
    {
        // 'value3' is not serialized.
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
        const AppendableLessElement& data)
{
    Cdr::state current_status(cdr);
    cdr.begin_serialize_type(current_status, cdr.get_encoding_flag());
    cdr << MemberId(3) << data.value1;
    cdr << MemberId(0x3FFF) << data.value2;
    cdr.end_serialize_type(current_status);
}

template<>
void deserialize(
        Cdr& cdr,
        AppendableLessElement& data)
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
                    case 2:
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

struct MutableElement
{
public:

    MutableElement() = default;

    MutableElement(
            uint8_t value)
    {
        value1 = value;
        value2 = value;
    }

    bool operator ==(
            const MutableElement& other) const
    {
        return value1 == other.value1 && value2 == other.value2;
    }

    uint32_t value1 {0};

    uint16_t value2 {0};
};

namespace eprosima {
namespace fastcdr {

template<>
void serialize(
        Cdr& cdr,
        const MutableElement& data)
{
    Cdr::state current_status(cdr);
    cdr.begin_serialize_type(current_status,
            CdrVersion::XCDRv2 ==
            cdr.get_cdr_version() ? EncodingAlgorithmFlag::PL_CDR2 : EncodingAlgorithmFlag::PL_CDR);
    cdr << MemberId(0) << data.value1;
    cdr << MemberId(1) << data.value2;
    cdr.end_serialize_type(current_status);
}

template<>
void deserialize(
        Cdr& cdr,
        MutableElement& data)
{
    cdr.deserialize_type( CdrVersion::XCDRv2 ==
            cdr.get_cdr_version() ? EncodingAlgorithmFlag::PL_CDR2 : EncodingAlgorithmFlag::PL_CDR,
            [&data](Cdr& cdr_inner, const MemberId& mid) -> bool
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

struct ApInnerStructure
{
public:

    ApInnerStructure() = default;

    ApInnerStructure(
            eprosima::fastcdr::EncodingAlgorithmFlag e1,
            eprosima::fastcdr::EncodingAlgorithmFlag e2
            )
        : enc_xcdrv1(e1)
        , enc_xcdrv2(e2)
    {
    }

    ApInnerStructure(
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
            const ApInnerStructure& other) const
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
        const ApInnerStructure& data)
{
    Cdr::state current_status(cdr);
    cdr.begin_serialize_type(current_status, cdr.get_cdr_version() == eprosima::fastcdr::CdrVersion::XCDRv1
                             ? data.enc_xcdrv1
                             : data.enc_xcdrv2);
    cdr << MemberId(0) << data.value1;
    cdr << MemberId(1) << data.value2;
    cdr.end_serialize_type(current_status);
}

template<>
void deserialize(
        Cdr& cdr,
        ApInnerStructure& data)
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
 * @test Test an appendable structure where the encoded version has more members that the decoded one.
 * @code{.idl}
 * @appendable
 * struct AppendableStruct // Encoded version
 * {
 *     @id(1)
 *     unsigned long value1;
 *     @id(2)
 *     unsigned short value2;
 *     @id(3)
 *     octet value3;
 * };
 *
 * @appendable
 * struct AppendableStruct // Decoded version
 * {
 *     @id(1)
 *     unsigned long value1;
 *     @id(2)
 *     unsigned short value2;
 * };
 * @endcode
 */
TEST_P(XCdrAppendableTest, more_serialized_elements)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival,             // UShort
        ival,                   // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00,             // UShort
        ival,                   // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x7,  // DHEADER
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival,             // UShort
        ival,                   // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x07, 0x00, 0x00, 0x00, // DHEADER
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00,             // UShort
        ival,                   // Octet
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
    cdr << MemberId(1) << value1;
    cdr << MemberId(2) << value2;
    cdr << MemberId(3) << value3;
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
                    case 0:
                        cdr_inner >> dvalue1;
                        break;
                    case 1:
                        cdr_inner >> dvalue2;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    Cdr::state dec_state_end(cdr);
    if (CdrVersion::XCDRv2 == get_version_from_algorithm(encoding))
    {
        ASSERT_EQ(enc_state_end, dec_state_end);
    }
    else
    {
        ASSERT_FALSE(enc_state_end == dec_state_end);
    }
    ASSERT_EQ(value1, dvalue1);
    ASSERT_EQ(value2, dvalue2);
    //}
}

/*!
 * @test Test an appendable structure where the encoded version has less members that the decoded one.
 * @code{.idl}
 * @appendable
 * struct AppendableStruct // Encoded version
 * {
 *     @id(1)
 *     unsigned long value1;
 *     @id(2)
 *     unsigned short value2;
 * };
 *
 * @appendable
 * struct AppendableStruct // Decoded version
 * {
 *     @id(1)
 *     unsigned long value1;
 *     @id(2)
 *     unsigned short value2;
 *     @id(3)
 *     octet value3;
 * };
 * @endcode
 */
TEST_P(XCdrAppendableTest, less_serialized_elements)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x6,  // DHEADER
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00              // UShort
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
                    case 0:
                        cdr_inner >> dvalue1;
                        break;
                    case 1:
                        cdr_inner >> dvalue2;
                        break;
                    case 2:
                        cdr_inner >> dvalue3;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    ASSERT_EQ(value1, dvalue1);
    ASSERT_EQ(value2, dvalue2);
    ASSERT_EQ(0, dvalue3);
    //}
}

/*!
 * @test Test an appendable structure where the encoded version has less members that the decoded one.
 * The decoded buffer takes into account the extra padding in order to reach the next 4-byte aligned offset.
 * @code{.idl}
 * @appendable
 * struct AppendableStruct // Encoded version
 * {
 *     @id(1)
 *     unsigned long value1;
 *     @id(2)
 *     unsigned short value2;
 * };
 *
 * @appendable
 * struct AppendableStruct // Decoded version
 * {
 *     @id(1)
 *     unsigned long value1;
 *     @id(2)
 *     unsigned short value2;
 *     @id(3)
 *     octet value3;
 * };
 * @endcode
 */
TEST_P(XCdrAppendableTest, less_serialized_elements_extra_padding)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x6,  // DHEADER
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    auto alignment {((expected_streams[tested_stream].size() + 3u) & ~3u) - expected_streams[tested_stream].size()};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size() + alignment);
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode
    uint32_t value1 { ival };
    uint16_t value2 { ival };
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
                    case 0:
                        cdr_inner >> dvalue1;
                        break;
                    case 1:
                        cdr_inner >> dvalue2;
                        break;
                    case 2:
                        cdr_inner >> dvalue3;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    ASSERT_EQ(value1, dvalue1);
    ASSERT_EQ(value2, dvalue2);
    ASSERT_EQ(0, dvalue3);
    //}
}

/*!
 * @test Test an inner appendable structure where the encoded version has more members that the decoded one.
 * @code{.idl}
 * @appendable
 * struct AppendableExtraElement // Encoded version
 * {
 *     @id(3)
 *     unsigned long value1;
 *     @id(16383)
 *     unsigned short value2;
 *     @id(16)
 *     octet value3;
 * };
 *
 * @appendable
 * struct AppendableExtraElement // Decoded version
 * {
 *     @id(3)
 *     unsigned long value1;
 *     @id(16383)
 *     unsigned short value2;
 * };
 *
 * @appendable
 * struct AppendableInnerStruct
 * {
 *     @id(1)
 *     AppendableExtraElement value1;
 *     @id(2)
 *     AppendableExtraElement value2;
 * };
 * @endcode
 */
TEST_P(XCdrAppendableTest, inner_more_serialized_elements)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival,             // UShort
        ival,                   // Octet
        0x00,                   // Alignment
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival,             // UShort
        ival                    // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00,             // UShort
        ival,                   // Octet
        0x00,                   // Alignment
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00,             // UShort
        ival                    // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x17, // DHEADER
        0x00, 0x00, 0x00, 0x07, // DHEADER
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival,             // UShort
        ival,                   // Octet
        0x00,                   // Alignment
        0x00, 0x00, 0x00, 0x07, // DHEADER
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival,             // UShort
        ival                    // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x17, 0x00, 0x00, 0x00, // DHEADER
        0x07, 0x00, 0x00, 0x00, // DHEADER
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00,             // UShort
        ival,                   // Octet
        0x00,                   // Alignment
        0x07, 0x00, 0x00, 0x00, // DHEADER
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00,             // UShort
        ival                    // Octet
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
    AppendableExtraElement value { ival };
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
    AppendableExtraElement dvalue1, dvalue2;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
            {
                bool ret_value = true;

                switch (mid.id)
                {
                    case 0:
                        cdr_inner >> dvalue1;
                        break;
                    case 1:
                        cdr_inner >> dvalue2;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    Cdr::state dec_state_end(cdr);
    if (CdrVersion::XCDRv2 == get_version_from_algorithm(encoding))
    {
        ASSERT_EQ(enc_state_end, dec_state_end);
    }
    else
    {
        ASSERT_FALSE(enc_state_end == dec_state_end);
    }
    ASSERT_EQ(value, dvalue1);
    ASSERT_EQ(value, dvalue2); // This works on XCdrv1 because the octet is jumped as alignment.
    //}
}

/*!
 * @test Test an inner appendable structure where the encoded version has less members that the decoded one.
 * @code{.idl}
 * @appendable
 * struct AppendableLessElement // Encoded version
 * {
 *     @id(3)
 *     unsigned long value1;
 *     @id(16383)
 *     unsigned short value2;
 * };
 *
 * @appendable
 * struct AppendableLessElement // Decoded version
 * {
 *     @id(3)
 *     unsigned long value1;
 *     @id(16383)
 *     unsigned short value2;
 *     @id(16)
 *     octet value3;
 * };
 *
 * @appendable
 * struct AppendableInnerStruct
 * {
 *     @id(1)
 *     AppendableLessElement value1;
 *     @id(2)
 *     AppendableLessElement value2;
 * };
 * @endcode
 */
TEST_P(XCdrAppendableTest, inner_less_serialized_elements)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00,             // UShort
        0x00, 0x00,             // Alignment
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x16, // DHEADER
        0x00, 0x00, 0x00, 0x06, // DHEADER
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x06, // DHEADER
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x16, 0x00, 0x00, 0x00, // DHEADER
        0x06, 0x00, 0x00, 0x00, // DHEADER
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00,             // UShort
        0x00, 0x00,             // Alignment
        0x06, 0x00, 0x00, 0x00, // DHEADER
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00              // UShort
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
    AppendableLessElement value { ival };
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
    AppendableLessElement dvalue1, dvalue2;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
            {
                bool ret_value = true;

                switch (mid.id)
                {
                    case 0:
                        cdr_inner >> dvalue1;
                        break;
                    case 1:
                        cdr_inner >> dvalue2;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    ASSERT_EQ(value, dvalue1);
    ASSERT_EQ(value, dvalue2); // This works on XCdrv1 because the octet is jumped as alignment.
    //}
}

/*!
 * @test Test an inner mutable structure.
 * Regresion test for redmine ticket #20044.
 * @code{.idl}
 * @mutable
 * struct MutableElement
 * {
 *     unsigned long value1;
 *     unsigned short value2;
 * };
 *
 * @appendable
 * struct AppendableWithMutable // Encoded version
 * {
 *     MutableElement value1;
 *     unsigned short value2;
 * };
 * @endcode
 */
TEST_P(XCdrAppendableTest, inner_mutable)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        0x00, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x00, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, 0x00, 0x00, 0x00, // ULong
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        ival, 0x00,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        ival, 0x00              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, ival, // ULong
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, ival,             // UShort
        0x00, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        ival, 0x00, 0x00, 0x00, // ULong
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        ival, 0x00,             // UShort
        ival, 0x00              // UShort
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
    MutableElement value { ival };
    uint16_t value2 { ival };
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(0) << value;
    cdr << MemberId(1) << value2;
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
    MutableElement dvalue;
    uint16_t dvalue2 { 0 };
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
            {
                bool ret_value = true;

                switch (mid.id)
                {
                    case 0:
                        cdr_inner >> dvalue;
                        break;
                    case 1:
                        cdr_inner >> dvalue2;
                        break;
                    default:
                        ret_value = false;
                        break;
                }

                return ret_value;
            });
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    ASSERT_EQ(value, dvalue);
    ASSERT_EQ(value2, dvalue2);
    //}
}

/*!
 * @test Test an inner final structure inside a appendable structure.
 * @code{.idl}
 * @final
 * struct InnerFinalStructure
 * {
 *     @id(0)
 *     unsigned long value1;
 *     @id(1) @optional
 *     unsigned long value2;
 * };
 *
 * @appendable
 * struct AppendableWithInnerFinalStruct
 * {
 *     @id(1)
 *     unsigned long value1;
 *     @id(2)
 *     InnerFinalStructure value2;
 * };
 * @endcode
 */
TEST_P(XCdrAppendableTest, inner_final_structure)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x01, 0x00, 0x00, // ShortMemberHeader (optional)
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00, 0x00, 0x00, // ULong
        0x01, 0x00, 0x00, 0x00, // ShortMemberHeader (optional)
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x09, // DHEADER
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x00, 0x00, ival, // ULong
        0x00,                   // Optional not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x03, // Encapsulation
        0x09, 0x00, 0x00, 0x00, // DHEADER
        ival, 0x00, 0x00, 0x00, // ULong
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
    ApInnerStructure value2 {eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR,
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
    ApInnerStructure dvalue2 {eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR,
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
                    case 0:
                        cdr_inner >> dvalue1;
                        break;
                    case 1:
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
 * @test Test an inner mutable structure inside a appendable structure.
 * @code{.idl}
 * @mutable
 * struct InnerMutableStructure
 * {
 *     @id(0)
 *     unsigned long value1;
 *     @id(1) @optional
 *     unsigned long value2;
 * };
 *
 * @appendable
 * struct AppendableWithInnerMutableStruct
 * {
 *     @id(1)
 *     unsigned long value1;
 *     @id(2)
 *     InnerMutableStructure value2;
 * };
 * @endcode
 */
TEST_P(XCdrAppendableTest, inner_mutable_structure)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, ival, // ULong
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        ival, 0x00, 0x00, 0x00, // ULong
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        ival, 0x00, 0x00, 0x00, // ULong
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, ival, // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        ival, 0x00, 0x00, 0x00, // ULong
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        ival, 0x00, 0x00, 0x00, // ULong
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
    ApInnerStructure value2 {eprosima::fastcdr::EncodingAlgorithmFlag::PL_CDR,
                             eprosima::fastcdr::EncodingAlgorithmFlag::PL_CDR2, ival};
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
    ApInnerStructure dvalue2 {eprosima::fastcdr::EncodingAlgorithmFlag::PL_CDR,
                              eprosima::fastcdr::EncodingAlgorithmFlag::PL_CDR2, ival};
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
            {
                bool ret_value {true};

                switch (mid.id)
                {
                    case 0:
                        cdr_inner >> dvalue1;
                        break;
                    case 1:
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
    XCdrAppendableTest,
    ::testing::Values(
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR, Cdr::Endianness::LITTLE_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::DELIMIT_CDR2, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::DELIMIT_CDR2, Cdr::Endianness::LITTLE_ENDIANNESS)
        ));

