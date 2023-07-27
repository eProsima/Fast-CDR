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
        // Expects 'value3' was not deserialized.
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

TEST_P(XCdrAppendableTest, more_serialized_elements)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival,             // UShort
        ival,                   // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        ival, 0x00, 0x00, 0x00, // ULong
        ival, 0x00,             // UShort
        ival,                   // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x7,  // DHEADER
        0x00, 0x00, 0x00, ival, // ULong
        0x00, ival,             // UShort
        ival,                   // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
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

TEST_P(XCdrAppendableTest, inner_more_serialized_elements)
{
    constexpr uint8_t ival {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
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
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x08, 0x00, 0x00, // Encapsulation
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
        0x00, 0x09, 0x00, 0x00, // Encapsulation
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
    ASSERT_EQ(value, dvalue2);
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

