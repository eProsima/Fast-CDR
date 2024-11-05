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
#include <fastcdr/CdrSizeCalculator.hpp>
#include <fastcdr/cdr/fixed_size_string.hpp>
#include "utility.hpp"

using namespace eprosima::fastcdr;

using XCdrStreamValues =
        std::array<std::vector<uint8_t>,
                1 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS>;

enum Enum32 : uint32_t
{
    ENUM32_VAL_1 = 0,
    ENUM32_VAL_2 = 1
};

enum Enum16 : uint16_t
{
    ENUM16_VAL_1 = 0,
    ENUM16_VAL_2 = 1
};

enum Enum8 : uint8_t
{
    ENUM8_VAL_1 = 0,
    ENUM8_VAL_2 = 1
};

class XCdrBasicTypesTest : public ::testing::TestWithParam< std::tuple<EncodingAlgorithmFlag, Cdr::Endianness>>
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

struct InnerBasicTypesStruct
{
public:

    InnerBasicTypesStruct() = default;

    InnerBasicTypesStruct(
            uint16_t v)
    {
        value1 = v;
        value2 = v;
    }

    bool operator ==(
            const InnerBasicTypesStruct& other) const
    {
        return value1 == other.value1 && value2 == other.value2;
    }

    InnerBasicTypesShortStruct value1;

    uint16_t value2 {0};
};

namespace eprosima {
namespace fastcdr {

template<>
size_t calculate_serialized_size(
        eprosima::fastcdr::CdrSizeCalculator& calculator,
        const InnerBasicTypesStruct& data,
        size_t& current_alignment)
{
    eprosima::fastcdr::EncodingAlgorithmFlag previous_encoding = calculator.get_encoding();
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(previous_encoding, current_alignment)};


    calculated_size += calculator.calculate_member_serialized_size(eprosima::fastcdr::MemberId(
                        0), data.value1, current_alignment);

    calculated_size += calculator.calculate_member_serialized_size(eprosima::fastcdr::MemberId(
                        1), data.value2, current_alignment);


    calculated_size += calculator.end_calculate_type_serialized_size(previous_encoding, current_alignment);

    return calculated_size;
}

template<>
void serialize(
        Cdr& cdr,
        const InnerBasicTypesStruct& data)
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
        InnerBasicTypesStruct& data)
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

template<class _T>
void serialize(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _T value,
        bool low_api)
{
    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding));
    size_t current_alignment {0};
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(encoding, current_alignment)};
    calculated_size += calculator.calculate_member_serialized_size(MemberId(1), value, current_alignment);
    calculated_size += calculator.end_calculate_type_serialized_size(encoding, current_alignment);
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
    cdr.begin_serialize_type(enc_state, encoding);
    if (low_api)
    {
        cdr.serialize_member(MemberId(1), value);
    }
    else
    {
        cdr << MemberId(1) << value;
    }
    cdr.end_serialize_type(enc_state);
    cdr.set_dds_cdr_options({0, 0});
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
    ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding the value.
    _T dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId&)->bool
            {
                bool ret_value = true;

                if (low_api)
                {
                    cdr_inner.deserialize_member(dvalue);
                }
                else
                {
                    cdr_inner >> dvalue;
                }

                if (EncodingAlgorithmFlag::PL_CDR != encoding && EncodingAlgorithmFlag::PL_CDR2 != encoding)
                {
                    ret_value = false;
                }

                return ret_value;
            });
    ASSERT_EQ(value, dvalue);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

template<class _Align, class _T>
void align_serialize(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _Align align_value,
        _T value,
        bool low_api)
{
    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding));
    size_t current_alignment {0};
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(encoding, current_alignment)};
    calculated_size += calculator.calculate_member_serialized_size(MemberId(0), align_value, current_alignment);
    calculated_size += calculator.calculate_member_serialized_size(MemberId(1), value, current_alignment);
    calculated_size += calculator.end_calculate_type_serialized_size(encoding, current_alignment);
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
    cdr.begin_serialize_type(enc_state, encoding);
    if (low_api)
    {
        cdr.serialize_member(MemberId(0), align_value);
        cdr.serialize_member(MemberId(1), value);
    }
    else
    {
        cdr << MemberId(0) << align_value << MemberId(1) << value;
    }
    cdr.end_serialize_type(enc_state);
    cdr.set_dds_cdr_options({0, 0});
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
    ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding the value.
    _Align dalign_value {0};
    _T dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
            {
                bool ret_value = true;
                if (low_api)
                {
                    switch (mid.id)
                    {
                        case 0:
                            cdr_inner.deserialize_member(dalign_value);
                            break;
                        case 1:
                            cdr_inner.deserialize_member(dvalue);
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
                            cdr_inner >> dalign_value;
                            break;
                        case 1:
                            cdr_inner >> dvalue;
                            break;
                        default:
                            ret_value = false;
                            break;
                    }
                }

                return ret_value;
            });
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_EQ(value, dvalue);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

template<class _Align>
void longdouble_align_serialize(
        const XCdrStreamValues& expected_streams_begin,
        const XCdrStreamValues& expected_streams_end,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _Align align_value,
        long double value,
        bool low_api)
{
    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding));
    size_t current_alignment {0};
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(encoding, current_alignment)};
    calculated_size += calculator.calculate_member_serialized_size(MemberId(0), align_value, current_alignment);
    calculated_size += calculator.calculate_member_serialized_size(MemberId(1), value, current_alignment);
    calculated_size += calculator.end_calculate_type_serialized_size(encoding, current_alignment);
    calculated_size += 4; // Encapsulation
    //}

    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    size_t total_size = expected_streams_begin[tested_stream].size() + 16 +
            expected_streams_end[tested_stream].size();
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(total_size, sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), total_size);
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode the value.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    if (low_api)
    {
        cdr.serialize_member(MemberId(0), align_value);
        cdr.serialize_member(MemberId(1), value);
    }
    else
    {
        cdr << MemberId(0) << align_value << MemberId(1) << value;
    }
    cdr.end_serialize_type(enc_state);
    cdr.set_dds_cdr_options({0, 0});
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.get_serialized_data_length(), total_size);
    ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams_begin[tested_stream].data(),
            expected_streams_begin[tested_stream].size()));
    ASSERT_EQ(0,
            memcmp(buffer.get() + cdr.get_serialized_data_length() - expected_streams_end[tested_stream].size(),
            expected_streams_end[tested_stream].data(),
            expected_streams_end[tested_stream].size()));
    //}

    //{ Decoding the value.
    _Align dalign_value {0};
    long double dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
            {
                bool ret_value = true;
                if (low_api)
                {
                    switch (mid.id)
                    {
                        case 0:
                            cdr_inner.deserialize_member(dalign_value);
                            break;
                        case 1:
                            cdr_inner.deserialize_member(dvalue);
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
                            cdr_inner >> dalign_value;
                            break;
                        case 1:
                            cdr_inner >> dvalue;
                            break;
                        default:
                            ret_value = false;
                            break;
                    }
                }

                return ret_value;
            });
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_EQ(value, dvalue);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

/*!
 * @test Test a structure with a field of short type.
 * @code{.idl}
 * struct ShortStruct
 * {
 *     short var_short;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, short)
{
    constexpr int16_t short_value {0x7DDC};
    constexpr uint8_t ival {0x7D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, short_value, true);

    serialize(expected_streams, encoding, endianness, short_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type.
 * @code{.idl}
 * struct UShortStruct
 * {
 *     unsigned short var_ushort;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, ushort)
{
    constexpr uint16_t ushort_value {static_cast<uint16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, ushort_value, true);

    serialize(expected_streams, encoding, endianness, ushort_value, false);
}

/*!
 * @test Test a structure with a field of long type.
 * @code{.idl}
 * struct LongStruct
 * {
 *     long var_long;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, long)
{
    constexpr int32_t long_value {static_cast<int32_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval, // Long
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // Long
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, long_value, true);

    serialize(expected_streams, encoding, endianness, long_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type.
 * @code{.idl}
 * struct ULongStruct
 * {
 *     unsigned long var_ulong;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, ulong)
{
    constexpr uint32_t ulong_value {0x1D1D1DDC};
    constexpr uint8_t ival {0x1D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval, // ULong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, ulong_value, true);

    serialize(expected_streams, encoding, endianness, ulong_value, false);
}

/*!
 * @test Test a structure with a field of long long type.
 * @code{.idl}
 * struct LongLongStruct
 * {
 *     long long var_longlong;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, longlong)
{
    constexpr int64_t longlong_value {static_cast<int64_t>(0xCDCDCDCDCDCDCDDCll)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval, // LongLong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival, // LongLong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, longlong_value, true);

    serialize(expected_streams, encoding, endianness, longlong_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long long type.
 * @code{.idl}
 * struct ULongLongStruct
 * {
 *     unsigned long long var_ulonglong;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, ulonglong)
{
    constexpr uint64_t ulonglong_value {0x1D1D1D1D1D1D1DDCull};
    constexpr uint8_t ival {0x1D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval, // ULongLong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival, // ULongLong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, ulonglong_value, true);

    serialize(expected_streams, encoding, endianness, ulonglong_value, false);
}

/*!
 * @test Test a structure with a field of float type.
 * @code{.idl}
 * struct FloatStruct
 * {
 *     float var_float;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, float)
{
    constexpr float float_value {13.0};
    constexpr uint8_t ival {0x41};
    constexpr uint8_t fval {0x50};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Float
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival, // Float
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, float_value, true);

    serialize(expected_streams, encoding, endianness, float_value, false);
}

/*!
 * @test Test a structure with a field of double type.
 * @code{.idl}
 * struct DoubleStruct
 * {
 *     double var_double;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, double)
{
    constexpr double double_value {13.0};
    constexpr uint8_t ival {0x40};
    constexpr uint8_t fval {0x2a};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00, // Double
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival, // Double
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, double_value, true);

    serialize(expected_streams, encoding, endianness, double_value, false);
}

/*!
 * @test Test a structure with a field of long double type.
 * @code{.idl}
 * struct LongDoubleStruct
 * {
 *     long double var_longdouble;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, longdouble)
{
    constexpr long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams_begin, expected_streams_end;
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x18, // DHEADER
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x18, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // Size
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding));
    size_t current_alignment {0};
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(encoding, current_alignment)};
    calculated_size += calculator.calculate_member_serialized_size(MemberId(1), longdouble_value, current_alignment);
    calculated_size += calculator.end_calculate_type_serialized_size(encoding, current_alignment);
    calculated_size += 4; // Encapsulation
    //}

    {
        //{ Prepare buffer
        uint8_t tested_stream = 0 + encoding + endianness;
        size_t total_size = expected_streams_begin[tested_stream].size() + 16 +
                expected_streams_end[tested_stream].size();
        auto buffer =
                std::unique_ptr<char, void (*)(
            void*)>{reinterpret_cast<char*>(calloc(total_size, sizeof(char))), free};
        FastBuffer fast_buffer(buffer.get(), total_size);
        Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
        //}

        //{ Encode the value.
        long double value {longdouble_value};
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        cdr.serialize_member(MemberId(1), value);
        cdr.end_serialize_type(enc_state);
        cdr.set_dds_cdr_options({0, 0});
        Cdr::state enc_state_end(cdr);
        //}

        //{ Test encoded content
        ASSERT_EQ(cdr.get_serialized_data_length(), total_size);
        ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
        ASSERT_EQ(0, memcmp(buffer.get(), expected_streams_begin[tested_stream].data(),
                expected_streams_begin[tested_stream].size()));
        ASSERT_EQ(0,
                memcmp(buffer.get() + cdr.get_serialized_data_length() - expected_streams_end[tested_stream].size(),
                expected_streams_end[tested_stream].data(),
                expected_streams_end[tested_stream].size()));
        //}

        //{ Decoding the value.
        long double dvalue;
        cdr.reset();
        cdr.read_encapsulation();
        ASSERT_EQ(cdr.get_encoding_flag(), encoding);
        ASSERT_EQ(cdr.endianness(), endianness);
        cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId&)->bool
                {
                    bool ret_value = true;

                    cdr_inner.deserialize_member(dvalue);

                    if (EncodingAlgorithmFlag::PL_CDR != encoding && EncodingAlgorithmFlag::PL_CDR2 != encoding)
                    {
                        ret_value = false;
                    }

                    return ret_value;
                });
        ASSERT_EQ(value, dvalue);
        Cdr::state dec_state_end(cdr);
        ASSERT_EQ(enc_state_end, dec_state_end);
        //}
    }

    {
        //{ Prepare buffer
        uint8_t tested_stream = 0 + encoding + endianness;
        size_t total_size = expected_streams_begin[tested_stream].size() + 16 +
                expected_streams_end[tested_stream].size();
        auto buffer =
                std::unique_ptr<char, void (*)(
            void*)>{reinterpret_cast<char*>(calloc(total_size, sizeof(char))), free};
        FastBuffer fast_buffer(buffer.get(), total_size);
        Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
        //}

        //{ Encode the value.
        long double value {longdouble_value};
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        cdr << MemberId(1) << value;
        cdr.end_serialize_type(enc_state);
        cdr.set_dds_cdr_options({0, 0});
        Cdr::state enc_state_end(cdr);
        //}

        //{ Test encoded content
        ASSERT_EQ(cdr.get_serialized_data_length(), total_size);
        ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
        ASSERT_EQ(0, memcmp(buffer.get(), expected_streams_begin[tested_stream].data(),
                expected_streams_begin[tested_stream].size()));
        ASSERT_EQ(0,
                memcmp(buffer.get() + cdr.get_serialized_data_length() - expected_streams_end[tested_stream].size(),
                expected_streams_end[tested_stream].data(),
                expected_streams_end[tested_stream].size()));
        //}

        //{ Decoding the value.
        long double dvalue;
        cdr.reset();
        cdr.read_encapsulation();
        ASSERT_EQ(cdr.get_encoding_flag(), encoding);
        ASSERT_EQ(cdr.endianness(), endianness);
        cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId&)->bool
                {
                    bool ret_value = true;

                    cdr_inner >> dvalue;

                    if (EncodingAlgorithmFlag::PL_CDR != encoding && EncodingAlgorithmFlag::PL_CDR2 != encoding)
                    {
                        ret_value = false;
                    }

                    return ret_value;
                });
        ASSERT_EQ(value, dvalue);
        Cdr::state dec_state_end(cdr);
        ASSERT_EQ(enc_state_end, dec_state_end);
        //}
    }

}

/*!
 * @test Test a structure with a field of boolean type.
 * @code{.idl}
 * struct BooleanStruct
 * {
 *     boolean var_boolean;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, boolean)
{
    constexpr bool boolean_value {true};
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x03, // Encapsulation
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x03, // Encapsulation
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value,                // Boolean
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value,                // Boolean
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x03, // Encapsulation
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x03, // Encapsulation
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // DHEADER
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x03, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // DHEADER
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, boolean_value, true);

    serialize(expected_streams, encoding, endianness, boolean_value, false);
}

/*!
 * @test Test a structure with a field of octet type.
 * @code{.idl}
 * struct OctetStruct
 * {
 *     octet var_octet;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, octet)
{
    constexpr uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x03, // Encapsulation
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x03, // Encapsulation
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x03, // Encapsulation
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x03, // Encapsulation
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // DHEADER
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x03, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // DHEADER
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, octet_value, true);

    serialize(expected_streams, encoding, endianness, octet_value, false);
}

/*!
 * @test Test a structure with a field of char type.
 * @code{.idl}
 * struct CharStruct
 * {
 *     char var_char;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, char)
{
    constexpr char char_value {'}'};
    constexpr uint8_t valu {static_cast<uint8_t>(char_value)};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x03, // Encapsulation
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x03, // Encapsulation
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        valu,                   // Char
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        valu,                   // Char
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x03, // Encapsulation
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x03, // Encapsulation
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // DHEADER
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x03, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // DHEADER
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        valu                    // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, char_value, true);

    serialize(expected_streams, encoding, endianness, char_value, false);
}

/*!
 * @test Test a structure with a field of wchar type.
 * @code{.idl}
 * struct WCharStruct
 * {
 *     wchar var_wchar;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, wchar)
{
    constexpr wchar_t wchar_value {static_cast<wchar_t>(0x0000CDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        ival, fval              // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        fval, ival              // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Wchar
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Wchar
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        ival, fval              // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        fval, ival              // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        ival, fval              // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        fval, ival              // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Wchar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, wchar_value, true);

    serialize(expected_streams, encoding, endianness, wchar_value, false);
}

/*!
 * @test Test a structure with a field of string type.
 * @code{.idl}
 * struct StringStruct
 * {
 *     string var_string;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, string)
{
    const std::string string_value {"AB"};
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x07, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x07, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x01, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x07, // DHEADER
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x07, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x0B, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x01, // Encapsulation
        0x0B, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, string_value, true);

    serialize(expected_streams, encoding, endianness, string_value, false);
}

/*!
 * @test Test a structure with a field of wstring type.
 * @code{.idl}
 * struct WStringStruct
 * {
 *     wstring var_wstring;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, wstring)
{
    const std::wstring string_value {L"AB"};
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB,             // WString
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00,             // WString
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00,             // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00,             // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB,             // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, string_value, true);

    serialize(expected_streams, encoding, endianness, string_value, false);
}

/*!
 * @test Test a structure with a field of fixed string type.
 * @code{.idl}
 * struct FixedStringStruct
 * {
 *     string<20> var_string;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, fixed_string)
{
    const fixed_string<20> string_value {"AB"};
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x07, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x07, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x01, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x07, // DHEADER
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x07, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x0B, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x01, // Encapsulation
        0x0B, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, string_value, true);

    serialize(expected_streams, encoding, endianness, string_value, false);
}

/*!
 * @test Test a structure with a field of enum 32bits type.
 * @code{.idl}
 * enum Enum32
 * {
 *     ENUM32_VAL_1 = 0,
 *     ENUM32_VAL_2 = 1
 * };
 *
 * struct Enum32Struct
 * {
 *     Enum32 var_enum32;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, enum32)
{
    constexpr Enum32 enum_value {ENUM32_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01  // Enum
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00  // Enum
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x01, // Enum
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x01, 0x00, 0x00, 0x00, // Enum
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01  // Enum
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00  // Enum
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        0x00, 0x00, 0x00, 0x01  // Enum
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00  // Enum
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x01  // Enum
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x01, 0x00, 0x00, 0x00  // Enum
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, enum_value, true);

    serialize(expected_streams, encoding, endianness, enum_value, false);
}

/*!
 * @test Test a structure with a field of enum 16bits type.
 * @code{.idl}
 * @bit_bound(16)
 * enum Enum16
 * {
 *     ENUM16_VAL_1 = 0,
 *     ENUM16_VAL_2 = 1
 * };
 *
 * struct Enum16Struct
 * {
 *     Enum16 var_enum16;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, enum16)
{
    constexpr Enum16 enum_value {ENUM16_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        0x00, 0x01              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        0x01, 0x00              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        0x00, 0x01,             // UShort
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        0x01, 0x00,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        0x00, 0x01              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        0x01, 0x00              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        0x00, 0x01              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x01              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x01, 0x00              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, enum_value, true);

    serialize(expected_streams, encoding, endianness, enum_value, false);
}

/*!
 * @test Test a structure with a field of enum 8bits type.
 * @code{.idl}
 * @bit_bound(8)
 * enum Enum8
 * {
 *     ENUM8_VAL_1 = 0,
 *     ENUM8_VAL_2 = 1
 * };
 *
 * struct Enum8Struct
 * {
 *     Enum8 var_enum8;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, enum8)
{
    constexpr Enum8 enum_value {ENUM8_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x03, // Encapsulation
        0x01                    // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x03, // Encapsulation
        0x01                    // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        0x01,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        0x01,                   // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x03, // Encapsulation
        0x01                    // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x03, // Encapsulation
        0x01                    // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // DHEADER
        0x01                    // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x03, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // DHEADER
        0x01                    // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x01                    // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        0x01                    // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, enum_value, true);

    serialize(expected_streams, encoding, endianness, enum_value, false);
}

/*!
 * @test Test a structure with a field of array of unsigned long type.
 * @code{.idl}
 *
 * struct ULongArrayStruct
 * {
 *     unsigned long var_ulongarray[2];
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, array_ulong)
{
    const std::array<uint32_t, 2> array_value {0xCDCDCDDC, 0xCDCDCDDC};
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
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
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
        0x00, 0x00, 0x00, 0x08, // DHEADER
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, array_value, true);

    serialize(expected_streams, encoding, endianness, array_value, false);
}

/*!
 * @test Test a structure with a field of array of struct type.
 * @code{.idl}
 * struct InnerBasicTypesShortStruct
 * {
 *     @id(3)
 *     unsigned short value1;
 *     @id(16383)
 *     unsigned short value2;
 * };
 *
 * struct StructArrayStruct
 * {
 *     InnerBasicTypesShortStruct var_structarray[2];
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, array_struct)
{
    const std::array<InnerBasicTypesShortStruct, 2> array_value {{{0xCDDC}, {0xDCCD}}};
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
        0x00, 0x01, 0x00, 0x38, // ShortMemberHeader
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
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x38, 0x00, // ShortMemberHeader
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
        0x02, 0x3F, 0x00, 0x00  // Sentinel
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
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval, ival, fval, // Array element 0
        0x00, 0x00, 0x00, 0x04, // DHEADER
        fval, ival, fval, ival  // Array element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, fval, ival, // Array element 0
        0x04, 0x00, 0x00, 0x00, // DHEADER
        ival, fval, ival, fval  // Array element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x2E, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x26, // DHEADER + NEXTINT
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
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x2E, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x26, 0x00, 0x00, 0x00, // DHEADER + NEXTINT
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

    serialize(expected_streams, encoding, endianness, array_value, true);

    serialize(expected_streams, encoding, endianness, array_value, false);
}


/*!
 * @test Test a structure with a field of multi array of unsigned long type.
 * @code{.idl}
 *
 * struct ULongMultiArrayStruct
 * {
 *     unsigned long var_ulongarray[2][2];
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, multi_array_ulong)
{
    const std::array<std::array<uint32_t, 2>, 2>  array_value {{
        {0xCDCDCDDC, 0xCDCDCDDC},
        {0xCDCDCDDC, 0xCDCDCDDC}
    }};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x18, // DHEADER
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // NEXTINT
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x18, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // NEXTINT
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, array_value, true);

    serialize(expected_streams, encoding, endianness, array_value, false);
}

/*!
 * @test Test a structure with a field of multi array of struct type.
 * @code{.idl}
 * struct InnerBasicTypesShortStruct
 * {
 *     @id(3)
 *     unsigned short value1;
 *     @id(16383)
 *     unsigned short value2;
 * };
 *
 * struct StructMultiArrayStruct
 * {
 *     InnerBasicTypesShortStruct var_structarray[2][2];
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, multi_array_struct)
{
    const std::array<std::array<InnerBasicTypesShortStruct, 2>, 2> array_value {{
        {{ {0xCDDC}, {0xDCCD} }},
        {{ {0xCDDC}, {0xDCCD} }}
    }};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, fval, ival, fval, // Array element 0,0
        fval, ival, fval, ival, // Array element 0,1
        ival, fval, ival, fval, // Array element 1,0
        fval, ival, fval, ival  // Array element 1,1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival, fval, ival, // Array element 0,0
        ival, fval, ival, fval, // Array element 0,1
        fval, ival, fval, ival, // Array element 1,0
        ival, fval, ival, fval, // Array element 1,1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x70, // ShortMemberHeader
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
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x70, 0x00, // ShortMemberHeader
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
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        ival, fval, ival, fval, // Array element 0,0
        fval, ival, fval, ival, // Array element 0,1
        ival, fval, ival, fval, // Array element 1,0
        fval, ival, fval, ival  // Array element 1,1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, fval, ival, // Array element 0,0
        ival, fval, ival, fval, // Array element 0,1
        fval, ival, fval, ival, // Array element 1,0
        ival, fval, ival, fval  // Array element 1,1
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x24, // DHEADER
        0x00, 0x00, 0x00, 0x20, // DHEADER
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval, ival, fval, // Array element 0,0
        0x00, 0x00, 0x00, 0x04, // DHEADER
        fval, ival, fval, ival, // Array element 0,1
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval, ival, fval, // Array element 1,0
        0x00, 0x00, 0x00, 0x04, // DHEADER
        fval, ival, fval, ival  // Array element 1,1
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x24, 0x00, 0x00, 0x00, // DHEADER
        0x20, 0x00, 0x00, 0x00, // DHEADER
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, fval, ival, // Array element 0,0
        0x04, 0x00, 0x00, 0x00, // DHEADER
        ival, fval, ival, fval, // Array element 0,1
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, fval, ival, // Array element 1,0
        0x04, 0x00, 0x00, 0x00, // DHEADER
        ival, fval, ival, fval  // Array element 1,1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x56, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x4E, // DHEADER + NEXTINT
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
        fval, ival,
        0x00, 0x00,             // Alignment
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
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x56, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x4E, 0x00, 0x00, 0x00, // DHEADER + NEXTINT
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
        ival, fval,
        0x00, 0x00,             // Alignment
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

    serialize(expected_streams, encoding, endianness, array_value, true);

    serialize(expected_streams, encoding, endianness, array_value, false);
}

/*!
 * @test Test a structure with a field of sequence of unsigned long type.
 * @code{.idl}
 *
 * struct ULongSequenceStruct
 * {
 *     sequence<unsigned long, 2> var_ulongsequence;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, sequence_ulong)
{
    const std::vector<uint32_t> sequence_value {0xCDCDCDDC, 0xCDCDCDDC};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x0C, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x0C, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x60, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x02, // NEXTINT
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x60, // EMHEADER1(M) without NEXTINT
        0x02, 0x00, 0x00, 0x00, // NEXTINT
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, sequence_value, true);

    serialize(expected_streams, encoding, endianness, sequence_value, false);
}

/*!
 * @test Test a structure with a field of sequence of struct type.
 * @code{.idl}
 * struct InnerBasicTypesShortStruct
 * {
 *     @id(3)
 *     unsigned short value1;
 *     @id(16383)
 *     unsigned short value2;
 * };
 *
 * struct StructSequenceStruct
 * {
 *     sequence<InnerBasicTypesShortStruct, 2> var_structsequence;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, sequence_struct)
{
    const std::vector<InnerBasicTypesShortStruct> sequence_value {{{0xCDDC}, {0xDCCD}}};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, fval, ival, fval, // Sequence element 0
        fval, ival, fval, ival  // Sequence element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, fval, ival, // Sequence element 0
        ival, fval, ival, fval  // Sequence element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x3c, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // Sequence length
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
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x3c, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // Sequence length
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
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, fval, ival, fval, // Sequence element 0
        fval, ival, fval, ival  // Sequence element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, fval, ival, // Sequence element 0
        ival, fval, ival, fval  // Sequence element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x18, // DHEADER
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval, ival, fval, // Sequence element 0
        0x00, 0x00, 0x00, 0x04, // DHEADER
        fval, ival, fval, ival  // Sequence element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x18, 0x00, 0x00, 0x00, // DHEADER
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, fval, ival, // Sequence element 0
        0x04, 0x00, 0x00, 0x00, // DHEADER
        ival, fval, ival, fval  // Sequence element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x32, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x2A, // DHEADER + NEXTINT
        0x00, 0x00, 0x00, 0x02, // Sequence length
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
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x32, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x2A, 0x00, 0x00, 0x00, // DHEADER + NEXTINT
        0x02, 0x00, 0x00, 0x00, // Sequence length
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

    serialize(expected_streams, encoding, endianness, sequence_value, true);

    serialize(expected_streams, encoding, endianness, sequence_value, false);
}

/*!
 * @test Test a structure with a field of sequence of sequences of unsigned long type.
 * @code{.idl}
 *
 * struct ULongSequenceSequenceStruct
 * {
 *     sequence<sequence<unsigned long, 2>, 2> var_ulongsequencesequence;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, recursive_sequence_ulong)
{
    const std::vector<std::vector<uint32_t>> sequence_value {
        {0xCDCDCDDC, 0xCDCDCDDC},
        {0xCDCDCDDC, 0xCDCDCDDC},
    };
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x1C, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x1C, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x1C, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x1C, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x20, // DHEADER
        0x00, 0x00, 0x00, 0x1C, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x00, // DHEADER
        0x1C, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x24, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x1C, // NEXTINT & DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval, // ULong
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x24, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) without NEXTINT
        0x1C, 0x00, 0x00, 0x00, // NEXTINT & DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival, // ULong
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, sequence_value, true);

    serialize(expected_streams, encoding, endianness, sequence_value, false);
}

/*!
 * @test Test a structure with a field of sequence of sequences of struct type.
 * @code{.idl}
 * struct InnerBasicTypesShortStruct
 * {
 *     @id(3)
 *     unsigned short value1;
 *     @id(16383)
 *     unsigned short value2;
 * };
 *
 * struct StructSequenceStruct
 * {
 *     sequence<sequence<InnerBasicTypesShortStruct, 2>, 2> var_structsequence;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, recursive_sequence_struct)
{
    const std::vector<std::vector<InnerBasicTypesShortStruct>> sequence_value {
        {{0xCDDC}, {0xDCCD}},
        {{0xCDDC}, {0xDCCD}}
    };
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, fval, ival, fval, // Sequence element 0
        fval, ival, fval, ival, // Sequence element 1
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, fval, ival, fval, // Sequence element 0
        fval, ival, fval, ival  // Sequence element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, fval, ival, // Sequence element 0
        ival, fval, ival, fval, // Sequence element 1
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, fval, ival, // Sequence element 0
        ival, fval, ival, fval  // Sequence element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x7c, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x02, // Sequence length
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
        0x00, 0x00, 0x00, 0x02, // Sequence length
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
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x7c, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x02, 0x00, 0x00, 0x00, // Sequence length
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
        0x02, 0x00, 0x00, 0x00, // Sequence length
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
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x24, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, fval, ival, fval, // Sequence element 0
        fval, ival, fval, ival, // Sequence element 1
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
        ival, fval, ival, fval, // Sequence element 0
        fval, ival, fval, ival  // Sequence element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x24, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, fval, ival, // Sequence element 0
        ival, fval, ival, fval, // Sequence element 1
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
        fval, ival, fval, ival, // Sequence element 0
        ival, fval, ival, fval  // Sequence element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x38, // DHEADER
        0x00, 0x00, 0x00, 0x34, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval, ival, fval, // Sequence element 0
        0x00, 0x00, 0x00, 0x04, // DHEADER
        fval, ival, fval, ival, // Sequence element 1
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval, ival, fval, // Sequence element 0
        0x00, 0x00, 0x00, 0x04, // DHEADER
        fval, ival, fval, ival  // Sequence element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x38, 0x00, 0x00, 0x00, // DHEADER
        0x34, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, fval, ival, // Sequence element 0
        0x04, 0x00, 0x00, 0x00, // DHEADER
        ival, fval, ival, fval, // Sequence element 1
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, fval, ival, // Sequence element 0
        0x04, 0x00, 0x00, 0x00, // DHEADER
        ival, fval, ival, fval  // Sequence element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x6A, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x62, // DHEADER + NEXTINT
        0x00, 0x00, 0x00, 0x02, // Sequence length
        0x00, 0x00, 0x00, 0x2A, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
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
        fval, ival,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x2A, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Sequence length
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
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x6A, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x62, 0x00, 0x00, 0x00, // DHEADER + NEXTINT
        0x02, 0x00, 0x00, 0x00, // Sequence length
        0x2A, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
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
        ival, fval,
        0x00, 0x00,             // Alignment
        0x2A, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Sequence length
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

    serialize(expected_streams, encoding, endianness, sequence_value, true);

    serialize(expected_streams, encoding, endianness, sequence_value, false);
}

/*!
 * @test Test a structure with a field of map of unsigned long type.
 * @code{.idl}
 *
 * struct ULongMapStruct
 * {
 *     map<unsigned short, unsigned long> var_ulongmap;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, map_ulong)
{
    const std::map<uint16_t, uint32_t> map_value {{static_cast<uint16_t>(0xCDDC), 0xCDCDCDDCu},
        {static_cast<uint16_t>(0xDCCD), 0xCDCDCDDCu}};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // Map length
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval, // ULong
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // Map length
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULong
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x14, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // Map length
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval, // ULong
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval, // ULong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x14, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // Map length
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULong
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // Map length
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval, // ULong
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // Map length
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULong
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Map length
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval, // ULong
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Map length
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULong
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x1c, // DHEADER
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x14, // NEXTINT
        0x00, 0x00, 0x00, 0x02, // Map length
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval, // ULong
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x1c, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) without NEXTINT
        0x14, 0x00, 0x00, 0x00, // NEXTINT
        0x02, 0x00, 0x00, 0x00, // Map length
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULong
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, map_value, true);

    serialize(expected_streams, encoding, endianness, map_value, false);
}

/*!
 * @test Test a structure with a field of map of struct type.
 * @code{.idl}
 * struct InnerBasicTypesShortStruct
 * {
 *     @id(3)
 *     unsigned short value1;
 *     @id(16383)
 *     unsigned short value2;
 * };
 *
 * struct StructMapStruct
 * {
 *     map<unsigned short, InnerBasicTypesShortStruct> var_structmap;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, map_struct)
{
    const std::map<uint16_t, InnerBasicTypesShortStruct> map_value {
        {static_cast<uint16_t>(0xCDDC), {0xCDDC}},
        {static_cast<uint16_t>(0xDCCD), {0xDCCD}}};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // Map length
        ival, fval,             // UShort
        ival, fval, ival, fval, // Map element 0
        fval, ival,             // UShort
        fval, ival, fval, ival  // Map element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // Map length
        fval, ival,             // UShort
        fval, ival, fval, ival, // Map element 0
        ival, fval,             // UShort
        ival, fval, ival, fval  // Map element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x44, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // Map length
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        0x00, 0x03, 0x00, 0x02, // ShortMemberHeader
        ival, fval,
        0x00, 0x00,             // Alignment
        0x3F, 0x01, 0x00, 0x08, // LongMemberHeader
        0x00, 0x00, 0x3F, 0xFF, // LongMemberHeader
        0x00, 0x00, 0x00, 0x02, // LongMemberHeader
        ival, fval,
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x00, 0x03, 0x00, 0x02, // ShortMemberHeader
        fval, ival,
        0x00, 0x00,             // Alignment
        0x3F, 0x01, 0x00, 0x08, // LongMemberHeader
        0x00, 0x00, 0x3F, 0xFF, // LongMemberHeader
        0x00, 0x00, 0x00, 0x02, // LongMemberHeader
        fval, ival,
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x44, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // Map length
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x03, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,
        0x00, 0x00,             // Alignment
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0xFF, 0x3F, 0x00, 0x00, // LongMemberHeader
        0x02, 0x00, 0x00, 0x00, // LongMemberHeader
        fval, ival,
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        0x03, 0x00, 0x02, 0x00, // ShortMemberHeader
        ival, fval,
        0x00, 0x00,             // Alignment
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0xFF, 0x3F, 0x00, 0x00, // LongMemberHeader
        0x02, 0x00, 0x00, 0x00, // LongMemberHeader
        ival, fval,
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Map length
        ival, fval,             // UShort
        ival, fval, ival, fval, // Map element 0
        fval, ival,             // UShort
        fval, ival, fval, ival  // Map element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Map length
        fval, ival,             // UShort
        fval, ival, fval, ival, // Map element 0
        ival, fval,             // UShort
        ival, fval, ival, fval  // Map element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x20, // DHEADER
        0x00, 0x00, 0x00, 0x1c, // DHEADER
        0x00, 0x00, 0x00, 0x02, // Map length
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval, ival, fval, // Map element 0
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x04, // DHEADER
        fval, ival, fval, ival  // Map element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x00, // DHEADER
        0x1c, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // Map length
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, fval, ival, // Map element 0
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        0x04, 0x00, 0x00, 0x00, // DHEADER
        ival, fval, ival, fval  // Map element 1
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x36, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x2e, // DHEADER + NEXTINT
        0x00, 0x00, 0x00, 0x02, // Map length
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        ival, fval,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        ival, fval,
        fval, ival,             // UShort
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        fval, ival,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        fval, ival
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x36, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x2e, 0x00, 0x00, 0x00, // DHEADER + NEXTINT
        0x02, 0x00, 0x00, 0x00, // Map length
        fval, ival,
        0x00, 0x00,             // Alignment
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,
        0x00, 0x00,             // Alignment
        0xFF, 0x3F, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,
        ival, fval,             // UShort
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

    serialize(expected_streams, encoding, endianness, map_value, true);

    serialize(expected_streams, encoding, endianness, map_value, false);
}

/*!
 * @test Test a structure with a field of bitset 7bits type.
 * @code{.idl}
 * bitset Bitset7
 * {
 *     bitfield<7> field;
 * };
 *
 * struct Bitset7Struct
 * {
 *     Bitset7 var_bitset7;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, bitset_8)
{
    constexpr std::bitset<7> bitset_value {0x08};
    constexpr uint8_t ival {0x08};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x03, // Encapsulation
        ival                    // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x03, // Encapsulation
        ival                    // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        ival,                   // Bitset
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        ival,                   // Bitset
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x03, // Encapsulation
        ival,                   // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x03, // Encapsulation
        ival                    // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // DHEADER
        ival,                   // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x03, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // DHEADER
        ival                    // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival,                   // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        ival                    // Bitset
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, bitset_value, true);

    serialize(expected_streams, encoding, endianness, bitset_value, false);
}

/*!
 * @test Test a structure with a field of bitset 14bits type.
 * @code{.idl}
 * bitset Bitset14
 * {
 *     bitfield<14> field;
 * };
 *
 * struct Bitset14Struct
 * {
 *     Bitset14 var_bitset14;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, bitset_16)
{
    constexpr std::bitset<14> bitset_value {0x0088};
    constexpr uint8_t ival {0x00};
    constexpr uint8_t fval {0x88};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        ival, fval              // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        fval, ival              // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Bitset
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Bitset
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        ival, fval              // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        fval, ival              // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        ival, fval              // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        fval, ival              // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Bitset
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, bitset_value, true);

    serialize(expected_streams, encoding, endianness, bitset_value, false);
}

/*!
 * @test Test a structure with a field of bitset 30bits type.
 * @code{.idl}
 * bitset Bitset30
 * {
 *     bitfield<30> field;
 * };
 *
 * struct Bitset30Struct
 * {
 *     Bitset30 var_bitset30;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, bitset_32)
{
    constexpr std::bitset<30> bitset_value {0x0DDCDCDC};
    constexpr uint8_t ival {0x0D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, fval, fval, fval  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, fval, fval, ival  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, fval, fval, // Bitset
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, fval, fval, ival, // Bitset
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, fval, fval, fval  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, fval, fval, ival  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval, fval, fval  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, fval, fval, ival  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, fval, fval  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, fval, fval, ival  // Bitset
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, bitset_value, true);

    serialize(expected_streams, encoding, endianness, bitset_value, false);
}

/*!
 * @test Test a structure with a field of bitset 45bits type.
 * @code{.idl}
 * bitset Bitset45
 * {
 *     bitfield<45> field;
 * };
 *
 * struct Bitset45Struct
 * {
 *     Bitset45 var_bitset45;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, bitset_64)
{
    constexpr std::bitset<45> bitset_value {0x000000DCDCDCDCDCull};
    constexpr uint8_t ival {0x00};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval, // Bitset
        fval, fval, fval, fval  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, fval, fval, fval, // Bitset
        fval, ival, ival, ival  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, fval, // Bitset
        fval, fval, fval, fval, // Bitset
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, fval, fval, fval, // Bitset
        fval, ival, ival, ival, // Bitset
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval, // Bitset
        fval, fval, fval, fval  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, fval, fval, fval, // Bitset
        fval, ival, ival, ival  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        ival, ival, ival, fval, // Bitset
        fval, fval, fval, fval  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fval, fval, fval, fval, // Bitset
        fval, ival, ival, ival  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval, // Bitset
        fval, fval, fval, fval  // Bitset
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, fval, fval, fval, // Bitset
        fval, ival, ival, ival  // Bitset
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize(expected_streams, encoding, endianness, bitset_value, true);

    serialize(expected_streams, encoding, endianness, bitset_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of short type.
 * @code{.idl}
 * struct ShortAlign1Struct
 * {
 *     octet var_align;
 *     short var_short;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, short_align_1)
{
    const uint8_t align_value {0xAB};
    const int16_t short_value {0x7DDC};
    constexpr uint8_t ival {0x7D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        align_value,
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, short_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, short_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of short type.
 * @code{.idl}
 * struct ShortAlign2Struct
 * {
 *     unsigned short var_align;
 *     short var_short;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, short_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const int16_t short_value {0x7DDC};
    constexpr uint8_t ival {0x7D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        iava, fava,
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, short_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, short_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of short type.
 * @code{.idl}
 * struct ShortAlign4Struct
 * {
 *     unsigned long var_align;
 *     short var_short;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, short_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const int16_t short_value {0x7DDC};
    constexpr uint8_t ival {0x7D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        iava, iava, iava, fava,
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        fava, iava, iava, iava,
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        iava, iava, iava, fava,
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        fava, iava, iava, iava,
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        iava, iava, iava, fava,
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, short_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, short_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of unsigned short type.
 * @code{.idl}
 * struct UShortAlign1Struct
 * {
 *     octet var_align;
 *     unsigned short var_ushort;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, ushort_align_1)
{
    const uint8_t align_value {0xAB};
    const uint16_t ushort_value {static_cast<uint16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        align_value,
        0x00,                   // Alignment
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00,                   // Alignment
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, ushort_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, ushort_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of unsigned short type.
 * @code{.idl}
 * struct UShortAlign2Struct
 * {
 *     unsigned short var_align;
 *     unsigned short var_ushort;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, ushort_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const uint16_t ushort_value {static_cast<uint16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        iava, fava,
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, ushort_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, ushort_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of unsigned short type.
 * @code{.idl}
 * struct UShortAlign4Struct
 * {
 *     unsigned long var_align;
 *     unsigned short var_ushort;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, ushort_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const uint16_t ushort_value {static_cast<uint16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        iava, iava, iava, fava,
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        fava, iava, iava, iava,
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // UShort
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // UShort
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        iava, iava, iava, fava,
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        fava, iava, iava, iava,
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        iava, iava, iava, fava,
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, ushort_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, ushort_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of long type.
 * @code{.idl}
 * struct LongAlign1Struct
 * {
 *     octet var_align;
 *     long var_long;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, long_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr int32_t long_value {static_cast<int32_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval, // Long
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // Long
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, long_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, long_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of long type.
 * @code{.idl}
 * struct LongAlign2Struct
 * {
 *     unsigned short var_align;
 *     long var_long;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, long_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr int32_t long_value {static_cast<int32_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval, // Long
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // Long
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, long_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, long_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of long type.
 * @code{.idl}
 * struct LongAlign4Struct
 * {
 *     unsigned long var_align;
 *     long var_long;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, long_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr int32_t long_value {static_cast<int32_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval, // Long
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // Long
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        iava, iava, iava, fava,
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, long_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, long_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of unsigned long type.
 * @code{.idl}
 * struct ULongAlign1Struct
 * {
 *     octet var_align;
 *     unsigned long var_ulong;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, ulong_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr uint32_t ulong_value {0x1D1D1DDC};
    constexpr uint8_t ival {0x1D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval, // ULong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, ulong_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, ulong_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of unsigned long type.
 * @code{.idl}
 * struct ULongAlign2Struct
 * {
 *     unsigned short var_align;
 *     unsigned long var_ulong;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, ulong_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint32_t ulong_value {0x1D1D1DDC};
    constexpr uint8_t ival {0x1D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval, // ULong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, ulong_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, ulong_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of unsigned long type.
 * @code{.idl}
 * struct ULongAlign4Struct
 * {
 *     unsigned long var_align;
 *     unsigned long var_ulong;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, ulong_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint32_t ulong_value {0x1D1D1DDC};
    constexpr uint8_t ival {0x1D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval, // ULong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        iava, iava, iava, fava,
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, ulong_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, ulong_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of long long type.
 * @code{.idl}
 * struct LongLongAlign1Struct
 * {
 *     octet var_align;
 *     long long var_longlong;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, longlong_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr int64_t longlong_value {static_cast<int64_t>(0xCDCDCDCDCDCDCDDCll)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
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
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival, // LongLong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, longlong_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, longlong_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of long long type.
 * @code{.idl}
 * struct LongLongAlign2Struct
 * {
 *     unsigned short var_align;
 *     long long var_longlong;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, longlong_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr int64_t longlong_value {static_cast<int64_t>(0xCDCDCDCDCDCDCDDCll)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval, // LongLong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival, // LongLong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, longlong_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, longlong_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of long long type.
 * @code{.idl}
 * struct LongLongAlign4Struct
 * {
 *     unsigned long var_align;
 *     long long var_longlong;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, longlong_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr int64_t longlong_value {static_cast<int64_t>(0xCDCDCDCDCDCDCDDCll)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x00, // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x00, 0x00, 0x00, 0x00, // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval, // LongLong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival, // LongLong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        iava, iava, iava, fava,
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, longlong_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, longlong_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of unsigned long long type.
 * @code{.idl}
 * struct ULongLongAlign1Struct
 * {
 *     octet var_align;
 *     unsigned long long var_ulonglong;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, ulonglong_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr uint64_t ulonglong_value {0x1D1D1D1D1D1D1DDCull};
    constexpr uint8_t ival {0x1D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval, // ULongLong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival, // ULongLong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, ulonglong_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, ulonglong_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of unsigned long long type.
 * @code{.idl}
 * struct ULongLongAlign2Struct
 * {
 *     unsigned short var_align;
 *     unsigned long long var_ulonglong;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, ulonglong_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint64_t ulonglong_value {0x1D1D1D1D1D1D1DDCull};
    constexpr uint8_t ival {0x1D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval, // ULongLong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival, // ULongLong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, ulonglong_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, ulonglong_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of unsigned long long type.
 * @code{.idl}
 * struct ULongLongAlign4Struct
 * {
 *     unsigned long var_align;
 *     unsigned long long var_ulonglong;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, ulonglong_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint64_t ulonglong_value {0x1D1D1D1D1D1D1DDCull};
    constexpr uint8_t ival {0x1D};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x00, // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x00, 0x00, 0x00, 0x00, // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval, // ULongLong
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival, // ULongLong
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        iava, iava, iava, fava,
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, ulonglong_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, ulonglong_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of float type.
 * @code{.idl}
 * struct FloatAlign1Struct
 * {
 *     octet var_align;
 *     float var_float;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, float_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr float float_value {13.0};
    constexpr uint8_t ival {0x41};
    constexpr uint8_t fval {0x50};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Float
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival, // Float
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, float_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, float_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of float type.
 * @code{.idl}
 * struct FloatAlign2Struct
 * {
 *     unsigned short var_align;
 *     float var_float;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, float_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr float float_value {13.0};
    constexpr uint8_t ival {0x41};
    constexpr uint8_t fval {0x50};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Float
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival, // Float
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, float_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, float_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of float type.
 * @code{.idl}
 * struct FloatAlign4Struct
 * {
 *     unsigned long var_align;
 *     float var_float;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, float_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr float float_value {13.0};
    constexpr uint8_t ival {0x41};
    constexpr uint8_t fval {0x50};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Float
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival, // Float
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        iava, iava, iava, fava,
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, float_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, float_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of double type.
 * @code{.idl}
 * struct DoubleAlign1Struct
 * {
 *     octet var_align;
 *     double var_double;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, double_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr double double_value {13.0};
    constexpr uint8_t ival {0x40};
    constexpr uint8_t fval {0x2a};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00, // Double
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival, // Double
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, double_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, double_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of double type.
 * @code{.idl}
 * struct DoubleAlign2Struct
 * {
 *     unsigned short var_align;
 *     double var_double;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, double_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr double double_value {13.0};
    constexpr uint8_t ival {0x40};
    constexpr uint8_t fval {0x2a};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00, // Double
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival, // Double
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, double_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, double_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of double type.
 * @code{.idl}
 * struct DoubleAlign4Struct
 * {
 *     unsigned long var_align;
 *     double var_double;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, double_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr double double_value {13.0};
    constexpr uint8_t ival {0x40};
    constexpr uint8_t fval {0x2a};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x00, // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x00, 0x00, 0x00, 0x00, // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00, // Double
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival, // Double
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        iava, iava, iava, fava,
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, double_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, double_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of long double type.
 * @code{.idl}
 * struct LongDoubleAlign1Struct
 * {
 *     octet var_align;
 *     long double var_longdouble;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, longdouble_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams_begin, expected_streams_end;
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x20, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // Size
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    longdouble_align_serialize(expected_streams_begin, expected_streams_end, encoding, endianness,
            align_value, longdouble_value, true);

    longdouble_align_serialize(expected_streams_begin, expected_streams_end, encoding, endianness,
            align_value, longdouble_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of long double type.
 * @code{.idl}
 * struct LongDoubleAlign2Struct
 * {
 *     unsigned short var_align;
 *     long double var_longdouble;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, longdouble_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams_begin, expected_streams_end;
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x20, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // Size
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    longdouble_align_serialize(expected_streams_begin, expected_streams_end, encoding, endianness,
            align_value, longdouble_value, true);

    longdouble_align_serialize(expected_streams_begin, expected_streams_end, encoding, endianness,
            align_value, longdouble_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of long double type.
 * @code{.idl}
 * struct LongDoubleAlign4Struct
 * {
 *     unsigned long var_align;
 *     long double var_longdouble;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, longdouble_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams_begin, expected_streams_end;
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x00, // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x00, 0x00, 0x00, 0x00, // Alignment
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        iava, iava, iava, fava,
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x20, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // Size
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    longdouble_align_serialize(expected_streams_begin, expected_streams_end, encoding, endianness,
            align_value, longdouble_value, true);

    longdouble_align_serialize(expected_streams_begin, expected_streams_end, encoding, endianness,
            align_value, longdouble_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of boolean type.
 * @code{.idl}
 * struct BooleanAlign1Struct
 * {
 *     octet var_align;
 *     boolean var_boolean;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, boolean_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr bool boolean_value {true};
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        align_value,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        align_value,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value,                // Boolean
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value,                // Boolean
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        align_value,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        align_value,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        align_value,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x0D, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x0D, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, boolean_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, boolean_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of boolean type.
 * @code{.idl}
 * struct BooleanAlign2Struct
 * {
 *     unsigned short var_align;
 *     boolean var_boolean;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, boolean_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr bool boolean_value {true};
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        iava, fava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        fava, iava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value,                // Boolean
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value,                // Boolean
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x01, // Encapsulation
        iava, fava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x01, // Encapsulation
        fava, iava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // DHEADER
        iava, fava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x0D, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x0D, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, boolean_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, boolean_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of boolean type.
 * @code{.idl}
 * struct BooleanAlign4Struct
 * {
 *     unsigned long var_align;
 *     boolean var_boolean;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, boolean_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr bool boolean_value {true};
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x03, // Encapsulation
        iava, iava, iava, fava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x03, // Encapsulation
        fava, iava, iava, iava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value,                // Boolean
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value,                // Boolean
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x03, // Encapsulation
        iava, iava, iava, fava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x03, // Encapsulation
        fava, iava, iava, iava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        iava, iava, iava, fava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x03, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x0D, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x0D, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, boolean_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, boolean_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of octet type.
 * @code{.idl}
 * struct OctetAlign1Struct
 * {
 *     octet var_align;
 *     octet var_octet;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, octet_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        align_value,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        align_value,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        align_value,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        align_value,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        align_value,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x0D, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x0D, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, octet_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, octet_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of octet type.
 * @code{.idl}
 * struct OctetAlign2Struct
 * {
 *     unsigned short var_align;
 *     octet var_octet;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, octet_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        iava, fava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        fava, iava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value,             // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x01, // Encapsulation
        iava, fava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x01, // Encapsulation
        fava, iava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // DHEADER
        iava, fava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x0D, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x0D, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, octet_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, octet_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of octet type.
 * @code{.idl}
 * struct OctetAlign4Struct
 * {
 *     unsigned long var_align;
 *     octet var_octet;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, octet_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x03, // Encapsulation
        iava, iava, iava, fava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x03, // Encapsulation
        fava, iava, iava, iava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x03, // Encapsulation
        iava, iava, iava, fava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x03, // Encapsulation
        fava, iava, iava, iava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        iava, iava, iava, fava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x03, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x0D, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x0D, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, octet_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, octet_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of char type.
 * @code{.idl}
 * struct CharAlign1Struct
 * {
 *     octet var_align;
 *     char var_char;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, char_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr char char_value {'}'};
    constexpr uint8_t valu {static_cast<uint8_t>(char_value)};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        align_value,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        align_value,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        valu,                   // Char
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        valu,                   // Char
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        align_value,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        align_value,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        align_value,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x0D, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x0D, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        valu                    // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, char_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, char_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of char type.
 * @code{.idl}
 * struct CharAlign2Struct
 * {
 *     unsigned short var_align;
 *     char var_char;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, char_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr char char_value {'}'};
    constexpr uint8_t valu {static_cast<uint8_t>(char_value)};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        iava, fava,
        valu                     // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        fava, iava,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        valu,                   // Char
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        valu,                   // Char
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x01, // Encapsulation
        iava, fava,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x01, // Encapsulation
        fava, iava,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // DHEADER
        iava, fava,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x0D, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x0D, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        valu                    // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, char_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, char_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of char type.
 * @code{.idl}
 * struct CharAlign4Struct
 * {
 *     unsigned long var_align;
 *     char var_char;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, char_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr char char_value {'}'};
    constexpr uint8_t valu {static_cast<uint8_t>(char_value)};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x03, // Encapsulation
        iava, iava, iava, fava,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x03, // Encapsulation
        fava, iava, iava, iava,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        valu,                   // Char
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        valu,                   // Char
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x03, // Encapsulation
        iava, iava, iava, fava,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x03, // Encapsulation
        fava, iava, iava, iava,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        iava, iava, iava, fava,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x03, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x0D, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        valu                    // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x0D, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        valu                    // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, char_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, char_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of wchar type.
 * @code{.idl}
 * struct WCharAlign1Struct
 * {
 *     octet var_align;
 *     wchar var_wchar;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, wchar_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr wchar_t wchar_value {static_cast<wchar_t>(0x0000CDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        ival, fval              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        fval, ival              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // WChar
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // WChar
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        ival, fval              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        fval, ival              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        align_value,
        0x00,                   // Alignment
        ival, fval              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00,                   // Alignment
        fval, ival              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // WChar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, wchar_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, wchar_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of wchar type.
 * @code{.idl}
 * struct WCharAlign2Struct
 * {
 *     unsigned short var_align;
 *     wchar var_wchar;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, wchar_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr wchar_t wchar_value {static_cast<wchar_t>(0x0000CDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        ival, fval              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        fval, ival              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // WChar
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // WChar
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        ival, fval              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        fval, ival,             // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        iava, fava,
        ival, fval              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        fval, ival,             // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // WChar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, wchar_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, wchar_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of wchar type.
 * @code{.idl}
 * struct WCharAlign4Struct
 * {
 *     unsigned long var_align;
 *     wchar var_wchar;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, wchar_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr wchar_t wchar_value {static_cast<wchar_t>(0x0000CDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        iava, iava, iava, fava,
        ival, fval              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        fava, iava, iava, iava,
        fval, ival              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // WChar
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // WChar
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        iava, iava, iava, fava,
        ival, fval              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        fava, iava, iava, iava,
        fval, ival              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        iava, iava, iava, fava,
        ival, fval              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        fval, ival              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // WChar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, wchar_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, wchar_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of string type.
 * @code{.idl}
 * struct StringAlign1Struct
 * {
 *     octet var_align;
 *     string var_string;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, string_align_1)
{
    const uint8_t align_value {0xAB};
    const std::string string_value {"AB"};
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x07, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x07, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x01, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x01, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x0B, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x0B, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x13, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x01, // Encapsulation
        0x13, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, string_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, string_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of string type.
 * @code{.idl}
 * struct StringAlign2Struct
 * {
 *     unsigned short var_align;
 *     string var_string;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, string_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const std::string string_value {"AB"};
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x07, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x07, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x01, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x01, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x0B, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x0B, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x13, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x01, // Encapsulation
        0x13, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, string_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, string_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of string type.
 * @code{.idl}
 * struct StringAlign4Struct
 * {
 *     unsigned long var_align;
 *     string var_string;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, string_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const std::string string_value {"AB"};
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        fava, iava, iava, iava,
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x07, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x07, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x01, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x01, // Encapsulation
        fava, iava, iava, iava,
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x0B, // DHEADER
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x0B, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x13, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x01, // Encapsulation
        0x13, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, string_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, string_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of wstring type.
 * @code{.idl}
 * struct WStringAlign1Struct
 * {
 *     octet var_align;
 *     wstring var_wstring;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, wstring_align_1)
{
    const uint8_t align_value {0xAB};
    const std::wstring wstring_value {L"AB"};
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB,             // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB,             // WString
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00,             // WString
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA, // WString
        0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, wstring_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, wstring_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of wstring type.
 * @code{.idl}
 * struct WStringAlign2Struct
 * {
 *     unsigned short var_align;
 *     wstring var_wstring;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, wstring_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const std::wstring wstring_value {L"AB"};
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB,             // WString
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00,             // WString
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA, // WString
        0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, wstring_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, wstring_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of wstring type.
 * @code{.idl}
 * struct WStringAlign4Struct
 * {
 *     unsigned long var_align;
 *     wstring var_wstring;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, wstring_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const std::wstring wstring_value {L"AB"};
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB,             // WString
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00,             // WString
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0C, // DHEADER
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0C, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB              // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x14, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00              // WString
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, wstring_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, wstring_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of enum 32bits type.
 * @code{.idl}
 * enum Enum32
 * {
 *     ENUM32_VAL_1 = 0,
 *     ENUM32_VAL_2 = 1
 * };
 *
 * struct Enum32Align1Struct
 * {
 *     octet var_align;
 *     Enum32 var_enum32;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, enum32_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr Enum32 enum_value {ENUM32_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x01, // Enum32
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x01, 0x00, 0x00, 0x00, // Enum32
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of enum 32bits type.
 * @code{.idl}
 * enum Enum32
 * {
 *     ENUM32_VAL_1 = 0,
 *     ENUM32_VAL_2 = 1
 * };
 *
 * struct Enum32Align2Struct
 * {
 *     unsigned short var_align;
 *     Enum32 var_enum32;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, enum32_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr Enum32 enum_value {ENUM32_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x01, // Enum32
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x01, 0x00, 0x00, 0x00, // Enum32
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of enum 32bits type.
 * @code{.idl}
 * enum Enum32
 * {
 *     ENUM32_VAL_1 = 0,
 *     ENUM32_VAL_2 = 1
 * };
 *
 * struct Enum32Align4Struct
 * {
 *     unsigned long var_align;
 *     Enum32 var_enum32;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, enum32_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr Enum32 enum_value {ENUM32_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x01, // Enum32
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x01, 0x00, 0x00, 0x00, // Enum32
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of enum 16bits type.
 * @code{.idl}
 * enum Enum16
 * {
 *     ENUM16_VAL_1 = 0,
 *     ENUM16_VAL_2 = 1
 * };
 *
 * struct Enum16Align1Struct
 * {
 *     octet var_align;
 *     Enum16 var_enum16;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, enum16_align_1)
{
    const uint8_t align_value {0xAB};
    const Enum16 enum_value {ENUM16_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        0x01, 0x00              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        0x00, 0x01,             // Enum16
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        0x01, 0x00,             // Enum16
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00,                   // Alignment
        0x01, 0x00              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        align_value,
        0x00,                   // Alignment
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00,                   // Alignment
        0x01, 0x00              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x01, 0x00              // Enum16
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of enum 16bits type.
 * @code{.idl}
 * enum Enum16
 * {
 *     ENUM16_VAL_1 = 0,
 *     ENUM16_VAL_2 = 1
 * };
 *
 * struct Enum16Align2Struct
 * {
 *     unsigned short var_align;
 *     Enum16 var_enum16;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, enum16_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const Enum16 enum_value {ENUM16_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01, 0x00              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        0x00, 0x01,             // Enum16
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        0x01, 0x00,             // Enum16
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01, 0x00              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        iava, fava,
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x01, 0x00              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x01, 0x00              // Enum16
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of enum 16bits type.
 * @code{.idl}
 * enum Enum16
 * {
 *     ENUM16_VAL_1 = 0,
 *     ENUM16_VAL_2 = 1
 * };
 *
 * struct Enum16Align4Struct
 * {
 *     unsigned long var_align;
 *     Enum16 var_enum16;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, enum16_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const Enum16 enum_value {ENUM16_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        0x00, 0x01,             // Enum16
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        0x01, 0x00,             // Enum16
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        iava, iava, iava, fava,
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        0x01, 0x00              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x0E, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x0E, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x01, 0x00              // Enum16
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, false);
}

/*!
 * @test Test a structure with a field of octet type and another of enum 8bits type.
 * @code{.idl}
 * enum Enum8
 * {
 *     ENUM8_VAL_1 = 0,
 *     ENUM8_VAL_2 = 1
 * };
 *
 * struct Enum8Align1Struct
 * {
 *     octet var_align;
 *     Enum8 var_enum8;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, enum8_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr Enum8 enum_value {ENUM8_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x02, // Encapsulation
        align_value,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x02, // Encapsulation
        align_value,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        enum_value,             // Enum8
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        enum_value,             // Enum8
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x02, // Encapsulation
        align_value,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x02, // Encapsulation
        align_value,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        align_value,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x0D, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x0D, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        enum_value              // Enum8
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, false);
}

/*!
 * @test Test a structure with a field of unsigned short type and another of enum 8bits type.
 * @code{.idl}
 * enum Enum8
 * {
 *     ENUM8_VAL_1 = 0,
 *     ENUM8_VAL_2 = 1
 * };
 *
 * struct Enum8Align2Struct
 * {
 *     unsigned short var_align;
 *     Enum8 var_enum8;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, enum8_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr Enum8 enum_value {ENUM8_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        iava, fava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        fava, iava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        enum_value,             // Enum8
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        enum_value,             // Enum8
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x01, // Encapsulation
        iava, fava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x01, // Encapsulation
        fava, iava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // DHEADER
        iava, fava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x0D, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x0D, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        enum_value              // Enum8
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, false);
}

/*!
 * @test Test a structure with a field of unsigned long type and another of enum 8bits type.
 * @code{.idl}
 * enum Enum8
 * {
 *     ENUM8_VAL_1 = 0,
 *     ENUM8_VAL_2 = 1
 * };
 *
 * struct Enum8Align4Struct
 * {
 *     unsigned long var_align;
 *     Enum8 var_enum8;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, enum8_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint8_t enum_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x03, // Encapsulation
        iava, iava, iava, fava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x03, // Encapsulation
        fava, iava, iava, iava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        enum_value,             // Enum8
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        enum_value,             // Enum8
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x03, // Encapsulation
        iava, iava, iava, fava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x03, // Encapsulation
        fava, iava, iava, iava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        iava, iava, iava, fava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x03, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x0D, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x03, // Encapsulation
        0x0D, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        enum_value              // Enum8
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, true);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value, false);
}

/*!
 * @test Test a structure with strings
 * @code{.idl}
 * struct InnerStructStruct
 * {
 *     @id(1)
 *     string var_field1;
 *     @id(2)
 *     wstring var_field2;
 *     @id(3)
 *     string<20> var_field3;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, struct_with_strings)
{
    const std::string var_field1 {"AB"};
    const std::wstring var_field2 {L"AB"};
    const fixed_string<20> var_field3 {"AB"};
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB,             // WString
        0x00, 0x00, 0x00, 0x03, // Fixed String length
        valA, valB, 0x00        // Fixed String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00,             // WString
        0x03, 0x00, 0x00, 0x00, // Fixed String length
        valA, valB, 0x00        // Fixed String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x07, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x00, 0x02, 0x00, 0x08, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB,             // WString
        0x00, 0x03, 0x00, 0x07, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x03, // Fixed String length
        valA, valB, 0x00,       // Fixed String
        0x00,                   // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x07, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x02, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00,             // WString
        0x03, 0x00, 0x07, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x00, 0x00, // Fixed String length
        valA, valB, 0x00,       // Fixed String
        0x00,                   // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB,             // WString
        0x00, 0x00, 0x00, 0x03, // Fixed String length
        valA, valB, 0x00        // Fixed String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x01, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00,             // WString
        0x03, 0x00, 0x00, 0x00, // Fixed String length
        valA, valB, 0x00        // Fixed String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x17, // DHEADER
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB,             // WString
        0x00, 0x00, 0x00, 0x03, // Fixed String length
        valA, valB, 0x00        // Fixed String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x17, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00,             // WString
        0x03, 0x00, 0x00, 0x00, // Fixed String length
        valA, valB, 0x00        // Fixed String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x23, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x30, 0x00, 0x00, 0x02, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, valA,             // WString
        0x00, valB,             // WString
        0x50, 0x00, 0x00, 0x03, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x03, // Fixed String length
        valA, valB, 0x00        // Fixed String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x01, // Encapsulation
        0x23, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x02, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00,             // WString
        valB, 0x00,             // WString
        0x03, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x03, 0x00, 0x00, 0x00, // Fixed String length
        valA, valB, 0x00        // Fixed String
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding));
    size_t current_alignment {0};
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(encoding, current_alignment)};
    calculated_size += calculator.calculate_member_serialized_size(MemberId(1), var_field1, current_alignment);
    calculated_size += calculator.calculate_member_serialized_size(MemberId(2), var_field2, current_alignment);
    calculated_size += calculator.calculate_member_serialized_size(MemberId(3), var_field3, current_alignment);
    calculated_size += calculator.end_calculate_type_serialized_size(encoding, current_alignment);
    calculated_size += 4; // Encapsulation
    //}

    {
        //{ Prepare buffer
        uint8_t tested_stream = 0 + encoding + endianness;
        auto buffer =
                std::unique_ptr<char, void (*)(
            void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
        FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
        Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
        //}

        //{ Encode
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        cdr.serialize_member(MemberId(1), var_field1);
        cdr.serialize_member(MemberId(2), var_field2);
        cdr.serialize_member(MemberId(3), var_field3);
        cdr.end_serialize_type(enc_state);
        cdr.set_dds_cdr_options({0, 0});
        Cdr::state enc_state_end(cdr);
        //}

        //{ Test encoded content
        ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
        ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
        ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(),
                expected_streams[tested_stream].size()));
        //}

        //{ Decoding
        std::string dvar_field1;
        std::wstring dvar_field2;
        fixed_string<20> dvar_field3;
        cdr.reset();
        cdr.read_encapsulation();
        ASSERT_EQ(cdr.get_encoding_flag(), encoding);
        ASSERT_EQ(cdr.endianness(), endianness);
        cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
                {
                    bool ret_value = true;

                    if (EncodingAlgorithmFlag::PL_CDR == cdr_inner.get_encoding_flag() ||
                    EncodingAlgorithmFlag::PL_CDR2 == cdr_inner.get_encoding_flag())
                    {
                        switch (mid.id)
                        {
                            case 1:
                                cdr_inner.deserialize_member(dvar_field1);
                                break;
                            case 2:
                                cdr_inner.deserialize_member(dvar_field2);
                                break;
                            case 3:
                                cdr_inner.deserialize_member(dvar_field3);
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
                                cdr_inner.deserialize_member(dvar_field1);
                                break;
                            case 1:
                                cdr_inner.deserialize_member(dvar_field2);
                                break;
                            case 2:
                                cdr_inner.deserialize_member(dvar_field3);
                                break;
                            default:
                                ret_value = false;
                                break;
                        }
                    }

                    return ret_value;
                });
        ASSERT_EQ(var_field1, dvar_field1);
        ASSERT_EQ(var_field2, dvar_field2);
        ASSERT_EQ(var_field3, dvar_field3);
        Cdr::state dec_state_end(cdr);
        ASSERT_EQ(enc_state_end, dec_state_end);
        //}
    }

    {
        //{ Prepare buffer
        uint8_t tested_stream = 0 + encoding + endianness;
        auto buffer =
                std::unique_ptr<char, void (*)(
            void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
        FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
        Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
        //}

        //{ Encode
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        cdr << MemberId(1) << var_field1;
        cdr << MemberId(2) << var_field2;
        cdr << MemberId(3) << var_field3;
        cdr.end_serialize_type(enc_state);
        cdr.set_dds_cdr_options({0, 0});
        Cdr::state enc_state_end(cdr);
        //}

        //{ Test encoded content
        ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
        ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
        ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(),
                expected_streams[tested_stream].size()));
        //}

        //{ Decoding
        std::string dvar_field1;
        std::wstring dvar_field2;
        fixed_string<20> dvar_field3;
        cdr.reset();
        cdr.read_encapsulation();
        ASSERT_EQ(cdr.get_encoding_flag(), encoding);
        ASSERT_EQ(cdr.endianness(), endianness);
        cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
                {
                    bool ret_value = true;

                    if (EncodingAlgorithmFlag::PL_CDR == cdr_inner.get_encoding_flag() ||
                    EncodingAlgorithmFlag::PL_CDR2 == cdr_inner.get_encoding_flag())
                    {
                        switch (mid.id)
                        {
                            case 1:
                                cdr_inner >> dvar_field1;
                                break;
                            case 2:
                                cdr_inner >> dvar_field2;
                                break;
                            case 3:
                                cdr_inner >> dvar_field3;
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
                                cdr_inner >> dvar_field1;
                                break;
                            case 1:
                                cdr_inner >> dvar_field2;
                                break;
                            case 2:
                                cdr_inner >> dvar_field3;
                                break;
                            default:
                                ret_value = false;
                                break;
                        }
                    }

                    return ret_value;
                });
        ASSERT_EQ(var_field1, dvar_field1);
        ASSERT_EQ(var_field2, dvar_field2);
        ASSERT_EQ(var_field3, dvar_field3);
        Cdr::state dec_state_end(cdr);
        ASSERT_EQ(enc_state_end, dec_state_end);
        //}
    }
}

/*!
 * @test Test a structure with two fields of struct type.
 * @code{.idl}
 * struct InnerBasicTypesShortStruct
 * {
 *     @id(3)
 *     unsigned short value1;
 *     @id(16383)
 *     unsigned short value2;
 * };
 *
 * struct InnerStructStruct
 * {
 *     @id(1)
 *     InnerBasicTypesShortStruct var_field1;
 *     @id(2)
 *     InnerBasicTypesShortStruct var_field2;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, one_inner_struct)
{
    const uint16_t ushort_value {0xCDDC};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x1c, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x01, 0x00, 0x08, // LongMemberHeader
        0x00, 0x00, 0x3F, 0xFF, // LongMemberHeader
        0x00, 0x00, 0x00, 0x02, // LongMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x00, 0x02, 0x00, 0x1c, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x01, 0x00, 0x08, // LongMemberHeader
        0x00, 0x00, 0x3F, 0xFF, // LongMemberHeader
        0x00, 0x00, 0x00, 0x02, // LongMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x1c, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0xFF, 0x3F, 0x00, 0x00, // LongMemberHeader
        0x02, 0x00, 0x00, 0x00, // LongMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x02, 0x00, 0x1c, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0xFF, 0x3F, 0x00, 0x00, // LongMemberHeader
        0x02, 0x00, 0x00, 0x00, // LongMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval,             // Short
        ival, fval,             // Short
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval,             // Short
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival,             // Short
        fval, ival,             // Short
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival,             // Short
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x2E, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0E, // NEXTINT + DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x50, 0x00, 0x00, 0x02, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0E, // NEXTINT + DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x2E, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x0E, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
        0x03, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0xFF, 0x3F, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x0E, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
        0x03, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0xFF, 0x3F, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    InnerBasicTypesShortStruct value { ushort_value };

    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding));
    size_t current_alignment {0};
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(encoding, current_alignment)};
    calculated_size += calculator.calculate_member_serialized_size(MemberId(1), value, current_alignment);
    calculated_size += calculator.calculate_member_serialized_size(MemberId(2), value, current_alignment);
    calculated_size += calculator.end_calculate_type_serialized_size(encoding, current_alignment);
    calculated_size += 4; // Encapsulation
    //}

    {
        //{ Prepare buffer
        uint8_t tested_stream = 0 + encoding + endianness;
        auto buffer =
                std::unique_ptr<char, void (*)(
            void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
        FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
        Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
        //}

        //{ Encode
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        cdr.serialize_member(MemberId(1), value);
        cdr.serialize_member(MemberId(2), value);
        cdr.end_serialize_type(enc_state);
        cdr.set_dds_cdr_options({0, 0});
        Cdr::state enc_state_end(cdr);
        //}

        //{ Test encoded content
        ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
        ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
        ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(),
                expected_streams[tested_stream].size()));
        //}

        //{ Decoding
        InnerBasicTypesShortStruct dvalue1, dvalue2;
        cdr.reset();
        cdr.read_encapsulation();
        ASSERT_EQ(cdr.get_encoding_flag(), encoding);
        ASSERT_EQ(cdr.endianness(), endianness);
        cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
                {
                    bool ret_value = true;

                    if (EncodingAlgorithmFlag::PL_CDR == cdr_inner.get_encoding_flag() ||
                    EncodingAlgorithmFlag::PL_CDR2 == cdr_inner.get_encoding_flag())
                    {
                        switch (mid.id)
                        {
                            case 1:
                                cdr_inner.deserialize_member(dvalue1);
                                break;
                            case 2:
                                cdr_inner.deserialize_member(dvalue2);
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
                                cdr_inner.deserialize_member(dvalue1);
                                break;
                            case 1:
                                cdr_inner.deserialize_member(dvalue2);
                                break;
                            default:
                                ret_value = false;
                                break;
                        }
                    }

                    return ret_value;
                });
        ASSERT_EQ(value, dvalue1);
        ASSERT_EQ(value, dvalue2);
        Cdr::state dec_state_end(cdr);
        ASSERT_EQ(enc_state_end, dec_state_end);
        //}
    }

    {
        //{ Prepare buffer
        uint8_t tested_stream = 0 + encoding + endianness;
        auto buffer =
                std::unique_ptr<char, void (*)(
            void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
        FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
        Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
        //}

        //{ Encode
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
        ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
        ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(),
                expected_streams[tested_stream].size()));
        //}

        //{ Decoding
        InnerBasicTypesShortStruct dvalue1, dvalue2;
        cdr.reset();
        cdr.read_encapsulation();
        ASSERT_EQ(cdr.get_encoding_flag(), encoding);
        ASSERT_EQ(cdr.endianness(), endianness);
        cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
                {
                    bool ret_value = true;

                    if (EncodingAlgorithmFlag::PL_CDR == cdr_inner.get_encoding_flag() ||
                    EncodingAlgorithmFlag::PL_CDR2 == cdr_inner.get_encoding_flag())
                    {
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
                    }
                    else
                    {
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
                    }

                    return ret_value;
                });
        ASSERT_EQ(value, dvalue1);
        ASSERT_EQ(value, dvalue2);
        Cdr::state dec_state_end(cdr);
        ASSERT_EQ(enc_state_end, dec_state_end);
        //}
    }
}

/*!
 * @test Test a structure with two fields of struct type.
 * @code{.idl}
 * struct InnerBasicTypesShortStruct
 * {
 *     @id(3)
 *     unsigned short value1;
 *     @id(16383)
 *     unsigned short value2;
 * };
 *
 * struct InnerBasicTypesStruct
 * {
 *     @id(0)
 *     InnerBasicTypesShortStruct value1;
 *     @id(1)
 *     unsigned short value2;
 * };
 *
 * struct InnerStructStruct
 * {
 *     @id(1)
 *     InnerBasicTypesStruct var_field1;
 *     @id(2)
 *     InnerBasicTypesStruct var_field2;
 * };
 * @endcode
 */
TEST_P(XCdrBasicTypesTest, two_inner_struct)
{
    const uint16_t ushort_value {0xCDDC};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x2c, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x1c, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x01, 0x00, 0x08, // LongMemberHeader
        0x00, 0x00, 0x3F, 0xFF, // LongMemberHeader
        0x00, 0x00, 0x00, 0x02, // LongMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x00, 0x02, 0x00, 0x2c, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x1c, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x01, 0x00, 0x08, // LongMemberHeader
        0x00, 0x00, 0x3F, 0xFF, // LongMemberHeader
        0x00, 0x00, 0x00, 0x02, // LongMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x2c, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x1c, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0xFF, 0x3F, 0x00, 0x00, // LongMemberHeader
        0x02, 0x00, 0x00, 0x00, // LongMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x02, 0x00, 0x2c, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x1c, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0xFF, 0x3F, 0x00, 0x00, // LongMemberHeader
        0x02, 0x00, 0x00, 0x00, // LongMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x1e, // DHEADER
        0x00, 0x00, 0x00, 0x0A, // DHEADER
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x0A, // DHEADER
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x02, // Encapsulation
        0x1e, 0x00, 0x00, 0x00, // DHEADER
        0x0A, 0x00, 0x00, 0x00, // DHEADER
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x0A, 0x00, 0x00, 0x00, // DHEADER
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x02, // Encapsulation
        0x00, 0x00, 0x00, 0x4e, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x1e, // NEXTINT + DHEADER
        0x50, 0x00, 0x00, 0x00, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0E, // NEXTINT + DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x50, 0x00, 0x00, 0x02, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x1e, // NEXTINT + DHEADER
        0x50, 0x00, 0x00, 0x00, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0E, // NEXTINT + DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval,             // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x02, // Encapsulation
        0x4e, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x1e, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
        0x00, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x0E, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
        0x03, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0xFF, 0x3F, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x1e, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
        0x00, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x0E, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
        0x03, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0xFF, 0x3F, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,             // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    InnerBasicTypesStruct value { ushort_value };

    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding));
    size_t current_alignment {0};
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(encoding, current_alignment)};
    calculated_size += calculator.calculate_member_serialized_size(MemberId(1), value, current_alignment);
    calculated_size += calculator.calculate_member_serialized_size(MemberId(2), value, current_alignment);
    calculated_size += calculator.end_calculate_type_serialized_size(encoding, current_alignment);
    calculated_size += 4; // Encapsulation
    //}

    {
        //{ Prepare buffer
        uint8_t tested_stream = 0 + encoding + endianness;
        auto buffer =
                std::unique_ptr<char, void (*)(
            void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
        FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
        Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
        //}

        //{ Encode
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        cdr.serialize_member(MemberId(1), value);
        cdr.serialize_member(MemberId(2), value);
        cdr.end_serialize_type(enc_state);
        cdr.set_dds_cdr_options({0, 0});
        Cdr::state enc_state_end(cdr);
        //}

        //{ Test encoded content
        ASSERT_EQ(cdr.get_serialized_data_length(), expected_streams[tested_stream].size());
        ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
        ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(),
                expected_streams[tested_stream].size()));
        //}

        //{ Decoding
        InnerBasicTypesStruct dvalue1, dvalue2;
        cdr.reset();
        cdr.read_encapsulation();
        ASSERT_EQ(cdr.get_encoding_flag(), encoding);
        ASSERT_EQ(cdr.endianness(), endianness);
        cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
                {
                    bool ret_value = true;

                    if (EncodingAlgorithmFlag::PL_CDR == cdr_inner.get_encoding_flag() ||
                    EncodingAlgorithmFlag::PL_CDR2 == cdr_inner.get_encoding_flag())
                    {
                        switch (mid.id)
                        {
                            case 1:
                                cdr_inner.deserialize_member(dvalue1);
                                break;
                            case 2:
                                cdr_inner.deserialize_member(dvalue2);
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
                                cdr_inner.deserialize_member(dvalue1);
                                break;
                            case 1:
                                cdr_inner.deserialize_member(dvalue2);
                                break;
                            default:
                                ret_value = false;
                                break;
                        }
                    }

                    return ret_value;
                });
        ASSERT_EQ(value, dvalue1);
        ASSERT_EQ(value, dvalue2);
        Cdr::state dec_state_end(cdr);
        ASSERT_EQ(enc_state_end, dec_state_end);
        //}
    }

    {
        //{ Prepare buffer
        uint8_t tested_stream = 0 + encoding + endianness;
        auto buffer =
                std::unique_ptr<char, void (*)(
            void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
        FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
        Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
        //}

        //{ Encode
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
        ASSERT_EQ(cdr.get_serialized_data_length(), calculated_size);
        ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(),
                expected_streams[tested_stream].size()));
        //}

        //{ Decoding
        InnerBasicTypesStruct dvalue1, dvalue2;
        cdr.reset();
        cdr.read_encapsulation();
        ASSERT_EQ(cdr.get_encoding_flag(), encoding);
        ASSERT_EQ(cdr.endianness(), endianness);
        cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId& mid)->bool
                {
                    bool ret_value = true;

                    if (EncodingAlgorithmFlag::PL_CDR == cdr_inner.get_encoding_flag() ||
                    EncodingAlgorithmFlag::PL_CDR2 == cdr_inner.get_encoding_flag())
                    {
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
                    }
                    else
                    {
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
                    }

                    return ret_value;
                });
        ASSERT_EQ(value, dvalue1);
        ASSERT_EQ(value, dvalue2);
        Cdr::state dec_state_end(cdr);
        ASSERT_EQ(enc_state_end, dec_state_end);
        //}
    }
}

INSTANTIATE_TEST_SUITE_P(
    XCdrTest,
    XCdrBasicTypesTest,
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

