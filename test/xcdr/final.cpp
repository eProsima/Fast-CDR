// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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


class XCdrFinalTest : public ::testing::TestWithParam< std::tuple<EncodingAlgorithmFlag, Cdr::Endianness>>
{
};

struct FiInnerStructure
{
public:

    FiInnerStructure() = default;

    FiInnerStructure(
            eprosima::fastcdr::EncodingAlgorithmFlag e1,
            eprosima::fastcdr::EncodingAlgorithmFlag e2
            )
        : enc_xcdrv1(e1)
        , enc_xcdrv2(e2)
    {
    }

    FiInnerStructure(
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
            const FiInnerStructure& other) const
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
        const FiInnerStructure& data)
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
        FiInnerStructure& data)
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
 * @test Test an inner appendable structure inside a final structure.
 * @code{.idl}
 * @appendable
 * struct InnerAppendableStructure
 * {
 *     @id(0)
 *     unsigned long value1;
 *     @id(1) @optional
 *     unsigned long value2;
 * };
 *
 * @final
 * struct FinalWithInnerAppendableStruct
 * {
 *     @id(1)
 *     unsigned long value1;
 *     @id(2)
 *     InnerAppendableStructure value2;
 * };
 * @endcode
 */
TEST_P(XCdrFinalTest, inner_appendable_structure)
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
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x00, 0x00, 0x05, // DHEADER
        0x00, 0x00, 0x00, ival, // ULong
        0x00,                   // Optional not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x03, // Encapsulation
        ival, 0x00, 0x00, 0x00, // ULong
        0x05, 0x00, 0x00, 0x00, // DHEADER
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
    FiInnerStructure value2 {eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR,
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
    FiInnerStructure dvalue2 {eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR,
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
 * @test Test an inner mutable structure inside a final structure.
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
 * @final
 * struct FinalWithInnerMutableStruct
 * {
 *     @id(1)
 *     unsigned long value1;
 *     @id(2)
 *     InnerMutableStructure value2;
 * };
 * @endcode
 */
TEST_P(XCdrFinalTest, inner_mutable_structure)
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
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, ival, // ULong
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, ival, // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
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
    FiInnerStructure value2 {eprosima::fastcdr::EncodingAlgorithmFlag::PL_CDR,
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
    FiInnerStructure dvalue2 {eprosima::fastcdr::EncodingAlgorithmFlag::PL_CDR,
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
    XCdrFinalTest,
    ::testing::Values(
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR, Cdr::Endianness::LITTLE_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR2, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR2, Cdr::Endianness::LITTLE_ENDIANNESS)
        ));

