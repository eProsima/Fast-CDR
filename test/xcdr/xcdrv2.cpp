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

#include <algorithm>
#include <array>
#include <memory>
#include <vector>

#include <fastcdr/Cdr.h>

using namespace eprosima::fastcdr;

using XCdrStreamValues =
        std::array<std::vector<unsigned char>,
                1 + Cdr::XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT>;

class XCdrv2Test : public ::testing::TestWithParam<std::tuple<EncodingAlgorithmFlag, CdrVersion>>
{
};

class XCdrv2PLTest : public ::testing::TestWithParam< Cdr::XCdrHeaderSelection>
{
};

TEST_P(XCdrv2Test, auto_selection_on_decode)
{
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = Cdr::Endianness::LITTLE_ENDIANNESS;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(100, sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), 100);

    //{ Encode a ushort and a ulong.
    Cdr cdr(fast_buffer, endianness, CdrVersion::XCDRv2);
    const uint16_t us {0x01FC};
    const uint32_t ul {0x01FC1FCD};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(0) << us;
    cdr << MemberId(1) << ul;
    cdr.end_serialize_type(enc_state);
    //}

    //{ Decode a ushort and a ulong.
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

TEST_P(XCdrv2PLTest, pl_octet_opt_member)
{
    constexpr unsigned char octet_value = 0xCD;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::XCdrHeaderSelection::SHORT_HEADER] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::XCdrHeaderSelection::LONG_HEADER] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x09, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x01, 0x00, 0x00, 0x00, // Member size
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::XCdrHeaderSelection::AUTO_WITH_SHORT_HEADER_BY_DEFAULT] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = EncodingAlgorithmFlag::PL_CDR2;
    Cdr::Endianness endianness = Cdr::Endianness::LITTLE_ENDIANNESS;
    uint8_t tested_stream = 0 + static_cast<uint8_t>(GetParam());
    size_t total_size = expected_streams[tested_stream].size() + 4; // Extra needed space for temp LongMemberHeader
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(total_size, sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), total_size);
    Cdr cdr(fast_buffer, endianness, CdrVersion::XCDRv2);
    //}

    //{ Encode optional not present.
    optional<uint8_t> opt_value {octet_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr.serialize_member(MemberId(1), opt_value, GetParam());
    cdr.end_serialize_type(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    auto it = std::find(expected_streams[tested_stream].begin(), expected_streams[tested_stream].end(), octet_value);
    ASSERT_EQ(cdr.get_serialized_data_length(),
            static_cast<size_t>(std::distance(expected_streams[tested_stream].begin(), ++it)));
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<uint8_t> opt_dec_value = 0;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&opt_dec_value](Cdr& cdr_inner, const MemberId& mid) -> bool
            {
                switch (mid.id)
                {
                    case 1:
                        cdr_inner.deserialize_member(opt_dec_value);
                }

                return true;
            });
    ASSERT_EQ(opt_value, opt_dec_value);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

INSTANTIATE_TEST_SUITE_P(
    XCdrTest,
    XCdrv2Test,
    ::testing::Values(
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR2, CdrVersion::XCDRv1),
        std::make_tuple(EncodingAlgorithmFlag::PLAIN_CDR2, CdrVersion::XCDRv2),
        std::make_tuple(EncodingAlgorithmFlag::DELIMIT_CDR2, CdrVersion::XCDRv1),
        std::make_tuple(EncodingAlgorithmFlag::DELIMIT_CDR2, CdrVersion::XCDRv2),
        std::make_tuple(EncodingAlgorithmFlag::PL_CDR2, CdrVersion::XCDRv1),
        std::make_tuple(EncodingAlgorithmFlag::PL_CDR2, CdrVersion::XCDRv2)
        ));

INSTANTIATE_TEST_SUITE_P(
    XCdrTest,
    XCdrv2PLTest,
    ::testing::Values(
        Cdr::XCdrHeaderSelection::SHORT_HEADER,
        Cdr::XCdrHeaderSelection::LONG_HEADER,
        Cdr::XCdrHeaderSelection::AUTO_WITH_SHORT_HEADER_BY_DEFAULT,
        Cdr::XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT
        ));

