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

#include <gtest/gtest.h>

#include <fastcdr/Cdr.h>
#include <fastcdr/CdrSizeCalculator.hpp>

#include "../xcdr/utility.hpp"

using namespace eprosima::fastcdr;

using XCdrStreamValues =
        std::array<std::vector<uint8_t>,
                1 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS>;

class CdrArrayAsSTDVectorTest : public ::testing::TestWithParam< std::tuple<EncodingAlgorithmFlag, Cdr::Endianness>>
{
};

struct InnerBasicTypesShortStruct
{
    InnerBasicTypesShortStruct() = default;

    InnerBasicTypesShortStruct(
            uint16_t v)
    {
        value1 = v;
        value2 = v;
    }

    bool operator ==(
            const InnerBasicTypesShortStruct& other) const
    {
        return value1 == other.value1 && value2 == other.value2;
    }

    uint16_t value1 {0};

    uint16_t value2 {0};
};

namespace eprosima {
namespace fastcdr {

template<>
size_t calculate_serialized_size(
        eprosima::fastcdr::CdrSizeCalculator& calculator,
        const InnerBasicTypesShortStruct& data,
        size_t& current_alignment)
{
    eprosima::fastcdr::EncodingAlgorithmFlag previous_encoding = calculator.get_encoding();
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(previous_encoding, current_alignment)};


    calculated_size += calculator.calculate_member_serialized_size(eprosima::fastcdr::MemberId(
                        3), data.value1, current_alignment);

    calculated_size += calculator.calculate_member_serialized_size(eprosima::fastcdr::MemberId(
                        0x3FFF), data.value2, current_alignment);


    calculated_size += calculator.end_calculate_type_serialized_size(previous_encoding, current_alignment);

    return calculated_size;
}

template<>
void serialize(
        Cdr& cdr,
        const InnerBasicTypesShortStruct& data)

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
        InnerBasicTypesShortStruct& data)
{
    cdr.deserialize_type(cdr.get_encoding_flag(), [&data](Cdr& cdr_inner, const MemberId& mid) -> bool
            {
                bool ret_value = true;
                if (EncodingAlgorithmFlag::PL_CDR == cdr_inner.get_encoding_flag() ||
                EncodingAlgorithmFlag::PL_CDR2 == cdr_inner.get_encoding_flag())
                {
                    switch (mid.id)
                    {
                        case 3:
                            cdr_inner >> data.value1;
                            break;
                        case 0x3FFF:
                            cdr_inner >> data.value2;
                            break;
                        default:
                            ret_value = false;
                            break;
                    }

                }
                else
                {
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
                }

                return ret_value;
            });
}

} // namespace fastcdr
} // namespace eprosima

template<class _T>
void serialize_array(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _T value)
{
    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding), encoding);
    size_t current_alignment {0};
    size_t calculated_size {calculator.calculate_array_serialized_size(value, current_alignment)};
    calculated_size += 4; // Encapsulation
    //}

    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode the value.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.serialize_array(value);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
    ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding the value.
    _T dvalue(value.size());
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_array(dvalue);
    ASSERT_EQ(value, dvalue);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

/*!
 * @test Test an array of unsigned long type stored in a std::vector.
 */
TEST_P(CdrArrayAsSTDVectorTest, array_ulong_as_std_vector)
{
    const std::vector<uint32_t> array_value {0xCDCDCDDC, 0xCDCDCDDC};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_array(expected_streams, encoding, endianness, array_value);
}

/*!
 * @test Test an array of struct type stored in a std::vector.
 * @code{.idl}
 * struct InnerBasicTypesShortStruct
 * {
 *     @id(3)
 *     unsigned short value1;
 *     @id(16383)
 *     unsigned short value2;
 * };
 *
 * InnerBasicTypesShortStruct var_structarray[2];
 *
 * @endcode
 */
TEST_P(CdrArrayAsSTDVectorTest, array_struct)
{
    const std::vector<InnerBasicTypesShortStruct> array_value {{{0xCDDC}, {0xDCCD}}};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, fval, ival, fval, // Array element 0
        fval, ival, fval, ival  // Array element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival, fval, ival, // Array element 0
        ival, fval, ival, fval  // Array element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x03, 0x00, 0x02, // ShortMemberHeader
        ival, fval,
        0x00, 0x00,             // Alignment
        0x3F, 0x01, 0x00, 0x08, // LongMemberHeader
        0x00, 0x00, 0x3F, 0xFF, // LongMemberHeader
        0x00, 0x00, 0x00, 0x02, // LongMemberHeader
        ival, fval,
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x00, 0x03, 0x00, 0x02, // ShortMemberHeader
        fval, ival,
        0x00, 0x00,             // Alignment
        0x3F, 0x01, 0x00, 0x08, // LongMemberHeader
        0x00, 0x00, 0x3F, 0xFF, // LongMemberHeader
        0x00, 0x00, 0x00, 0x02, // LongMemberHeader
        fval, ival,
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x03, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,
        0x00, 0x00,             // Alignment
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0xFF, 0x3F, 0x00, 0x00, // LongMemberHeader
        0x02, 0x00, 0x00, 0x00, // LongMemberHeader
        fval, ival,
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x03, 0x00, 0x02, 0x00, // ShortMemberHeader
        ival, fval,
        0x00, 0x00,             // Alignment
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0xFF, 0x3F, 0x00, 0x00, // LongMemberHeader
        0x02, 0x00, 0x00, 0x00, // LongMemberHeader
        ival, fval,
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        ival, fval, ival, fval, // Array element 0
        fval, ival, fval, ival  // Array element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, fval, ival, // Array element 0
        ival, fval, ival, fval  // Array element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval, ival, fval, // Array element 0
        0x00, 0x00, 0x00, 0x04, // DHEADER
        fval, ival, fval, ival  // Array element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, fval, ival, // Array element 0
        0x04, 0x00, 0x00, 0x00, // DHEADER
        ival, fval, ival, fval  // Array element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x26, // DHEADER
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        ival, fval,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        ival, fval,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        fval, ival,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        fval, ival
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x26, 0x00, 0x00, 0x00, // DHEADER
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,
        0x00, 0x00,             // Alignment
        0xFF, 0x3F, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,
        0x00, 0x00,             // Alignment
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        ival, fval,
        0x00, 0x00,             // Alignment
        0xFF, 0x3F, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        ival, fval
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_array(expected_streams, encoding, endianness, array_value);
}

INSTANTIATE_TEST_SUITE_P(
    CdrTest,
    CdrArrayAsSTDVectorTest,
    ::testing::Values(
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR, Cdr::Endianness::LITTLE_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PL_CDR, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PL_CDR, Cdr::Endianness::LITTLE_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR2, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR2, Cdr::Endianness::LITTLE_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::DELIMIT_CDR2, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::DELIMIT_CDR2, Cdr::Endianness::LITTLE_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PL_CDR2, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(EncodingAlgorithmFlag::PL_CDR2, Cdr::Endianness::LITTLE_ENDIANNESS)
        ));
