#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <vector>

#include <fastcdr/Cdr.h>
#include <fastcdr/xcdr/optional.hpp>

using namespace eprosima::fastcdr;

using XCdrStreamValues =
        std::array<std::vector<uint8_t>,
                1 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS>;


class XCdrOptionalTest : public ::testing::TestWithParam< std::tuple<Cdr::EncodingAlgorithmFlag, Cdr::Endianness>>
{
public:

    Cdr::CdrVersion get_version_from_algorithm(
            Cdr::EncodingAlgorithmFlag ef)
    {
        Cdr::CdrVersion cdr_version { Cdr::CdrVersion::XCDRv2 };

        switch (ef)
        {
            case Cdr::EncodingAlgorithmFlag::PLAIN_CDR:
            case Cdr::EncodingAlgorithmFlag::PL_CDR:
                cdr_version = Cdr::CdrVersion::XCDRv1;
                break;
            default:
                break;
        }

        return cdr_version;
    }

};

TEST_P(XCdrOptionalTest, null_serialize_the_value)
{
    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), false, enc_state);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_FALSE(is_present);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, short_serialize_the_value)
{
    const int16_t short_value = static_cast<int16_t>(0xCDDC);
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(short_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    int16_t short_dec_value = 0;
    cdr.deserialize(short_dec_value);
    ASSERT_EQ(short_value, short_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, ushort_serialize_the_value)
{
    const uint16_t ushort_value = static_cast<uint16_t>(0xCDDC);
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(ushort_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    uint16_t ushort_dec_value = 0;
    cdr.deserialize(ushort_dec_value);
    ASSERT_EQ(ushort_value, ushort_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, long_serialize_the_value)
{
    const int32_t long_value = static_cast<int32_t>(0xCDCDCDDC);
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(long_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    int32_t long_dec_value = 0;
    cdr.deserialize(long_dec_value);
    ASSERT_EQ(long_value, long_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, ulong_serialize_the_value)
{
    const uint32_t ulong_value = 0xCDCDCDDC;
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(ulong_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    uint32_t ulong_dec_value = 0;
    cdr.deserialize(ulong_dec_value);
    ASSERT_EQ(ulong_value, ulong_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, longlong_serialize_the_value)
{
    const int64_t longlong_value = static_cast<int64_t>(0xCDCDCDCDCDCDCDDCll);
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(longlong_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    int64_t longlong_dec_value = 0;
    cdr.deserialize(longlong_dec_value);
    ASSERT_EQ(longlong_value, longlong_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, ulonglong_serialize_the_value)
{
    const uint64_t ulonglong_value = 0xCDCDCDCDCDCDCDDCull;
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(ulonglong_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    uint64_t ulonglong_dec_value = 0;
    cdr.deserialize(ulonglong_dec_value);
    ASSERT_EQ(ulonglong_value, ulonglong_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, float_serialize_the_value)
{
    const float float_value = 13.0;
    constexpr uint8_t ival = 0x41;
    constexpr uint8_t fval = 0x50;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(float_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    float float_dec_value = 0;
    cdr.deserialize(float_dec_value);
    ASSERT_EQ(float_value, float_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, double_serialize_the_value)
{
    const double double_value = 13.0;
    constexpr uint8_t ival = 0x40;
    constexpr uint8_t fval = 0x2a;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(double_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    double double_dec_value = 0;
    cdr.deserialize(double_dec_value);
    ASSERT_EQ(double_value, double_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, longdouble_serialize_the_value)
{
    const long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // Size
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(longdouble_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0,
            memcmp(buffer.get(), expected_streams[tested_stream].data(),
            expected_streams[tested_stream].size() - sizeof(long double)));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    long double longdouble_dec_value = 0;
    cdr.deserialize(longdouble_dec_value);
    ASSERT_EQ(longdouble_value, longdouble_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, boolean_serialize_the_value)
{
    const bool boolean_value = true;
    constexpr uint8_t b_value = 0x1;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(boolean_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    uint8_t boolean_dec_value = 0;
    cdr.deserialize(boolean_dec_value);
    ASSERT_EQ(boolean_value, boolean_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, octet_serialize_the_value)
{
    const uint8_t octet_value = 0xCD;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(octet_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    uint8_t octet_dec_value = 0;
    cdr.deserialize(octet_dec_value);
    ASSERT_EQ(octet_value, octet_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, char_serialize_the_value)
{
    const char char_value = '}';

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(char_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    char char_dec_value = 0;
    cdr.deserialize(char_dec_value);
    ASSERT_EQ(char_value, char_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, wchar_serialize_the_value)
{
    const wchar_t wchar_value = static_cast<wchar_t>(0xCDCDCDDC);
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Wchar
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(wchar_value);
    cdr.end_serialize_opt_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    wchar_t wchar_dec_value = 0;
    cdr.deserialize(wchar_dec_value);
    ASSERT_EQ(wchar_value, wchar_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, null_optional)
{
    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<uint8_t> value;
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<uint8_t> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_FALSE(dvalue.has_value());
    //}
}

TEST_P(XCdrOptionalTest, short_optional)
{
    constexpr int16_t short_value = static_cast<int16_t>(0xCDDC);
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<int16_t> value {short_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<int16_t> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, ushort_optional)
{
    constexpr uint16_t ushort_value = static_cast<uint16_t>(0xCDDC);
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<uint16_t> value {ushort_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<uint16_t> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, long_optional)
{
    constexpr int32_t long_value = static_cast<int32_t>(0xCDCDCDDC);
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<int32_t> value {long_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<int32_t> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, ulong_optional)
{
    constexpr uint32_t ulong_value = 0xCDCDCDDC;
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<uint32_t> value {ulong_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<uint32_t> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, longlong_optional)
{
    constexpr int64_t longlong_value = static_cast<int64_t>(0xCDCDCDCDCDCDCDDCll);
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<int64_t> value {longlong_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<int64_t> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, ulonglong_optional)
{
    constexpr uint64_t ulonglong_value = 0xCDCDCDCDCDCDCDDCull;
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<uint64_t> value {ulonglong_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<uint64_t> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, float_optional)
{
    constexpr float float_value = 13.0;
    constexpr uint8_t ival = 0x41;
    constexpr uint8_t fval = 0x50;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<float> value {float_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<float> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, double_optional)
{
    constexpr double double_value = 13.0;
    constexpr uint8_t ival = 0x40;
    constexpr uint8_t fval = 0x2a;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<double> value {double_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<double> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, longdouble_optional)
{
    constexpr long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // Size
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<long double> value {longdouble_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0,
            memcmp(buffer.get(), expected_streams[tested_stream].data(),
            expected_streams[tested_stream].size() - sizeof(long double)));
    //}

    //{ Decoding optional not present
    optional<long double> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, boolean_optional)
{
    constexpr bool boolean_value = true;
    constexpr uint8_t b_value = 0x1;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<bool> value {boolean_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<bool> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, octet_optional)
{
    constexpr uint8_t octet_value = 0xCD;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<uint8_t> value {octet_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<uint8_t> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, char_optional)
{
    constexpr char char_value = '}';

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<char> value {char_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<char> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, wchar_optional)
{
    constexpr wchar_t wchar_value = static_cast<wchar_t>(0xCDCDCDDC);
    constexpr uint8_t ival = 0xCD;
    constexpr uint8_t fval = 0xDC;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + Cdr::EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Wchar
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<wchar_t> value {wchar_value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<wchar_t> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dvalue;
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

INSTANTIATE_TEST_SUITE_P(
    XCdrTest,
    XCdrOptionalTest,
    ::testing::Values(
        std::make_tuple(Cdr::EncodingAlgorithmFlag::PLAIN_CDR, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(Cdr::EncodingAlgorithmFlag::PLAIN_CDR, Cdr::Endianness::LITTLE_ENDIANNESS),
        std::make_tuple(Cdr::EncodingAlgorithmFlag::PL_CDR, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(Cdr::EncodingAlgorithmFlag::PL_CDR, Cdr::Endianness::LITTLE_ENDIANNESS),
        std::make_tuple(Cdr::EncodingAlgorithmFlag::PLAIN_CDR2, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(Cdr::EncodingAlgorithmFlag::PLAIN_CDR2, Cdr::Endianness::LITTLE_ENDIANNESS),
        std::make_tuple(Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(Cdr::EncodingAlgorithmFlag::DELIMIT_CDR2, Cdr::Endianness::LITTLE_ENDIANNESS),
        std::make_tuple(Cdr::EncodingAlgorithmFlag::PL_CDR2, Cdr::Endianness::BIG_ENDIANNESS),
        std::make_tuple(Cdr::EncodingAlgorithmFlag::PL_CDR2, Cdr::Endianness::LITTLE_ENDIANNESS)
        ));

