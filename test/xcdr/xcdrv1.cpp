#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <vector>

#include <fastcdr/Cdr.h>

using namespace eprosima::fastcdr;

using XCdrStreamValues =
        std::array<std::vector<unsigned char>,
                1 + Cdr::XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT>;

class XCdrv1Test : public ::testing::TestWithParam< Cdr::XCdrHeaderSelection>
{
};

TEST_P(XCdrv1Test, pl_octet_member)
{
    constexpr unsigned char octet_value = 0xCD;

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + Cdr::XCdrHeaderSelection::SHORT_HEADER] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::XCdrHeaderSelection::LONG_HEADER] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x3F, 0x08, 0x00, // LongMemberHeader
        0x01, 0x00, 0x00, 0x00, // Member ID and flags
        0x01, 0x00, 0x00, 0x00, // Member ID and flags
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::XCdrHeaderSelection::AUTO_WITH_SHORT_HEADER_BY_DEFAULT] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + Cdr::XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value,            // Octet
        0x00, 0x00, 0x00,       // Extra needed space for temp LongMemberHeader
        0x00, 0x00, 0x00, 0x00, // Extra needed space for temp LongMemberHeader
        octet_value             // Extra needed space for temp LongMemberHeader
    };
    //}

    //{ Prepare buffer
    Cdr::EncodingAlgorithmFlag encoding = Cdr::EncodingAlgorithmFlag::PL_CDR;
    Cdr::Endianness endianness = Cdr::Endianness::LITTLE_ENDIANNESS;
    uint8_t tested_stream = 0 + static_cast<uint8_t>(GetParam());
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(malloc(expected_streams[tested_stream].size())), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, Cdr::CdrType::DDS_CDR);
    //}

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_member(MemberId(1), enc_state, GetParam());
    cdr.serialize(octet_value);
    cdr.end_serialize_member(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
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
    cdr.begin_deserialize_member(member_id, dec_state, is_present);
    ASSERT_TRUE(is_present);
    unsigned char octet_dec_value = 0;
    cdr.deserialize(octet_dec_value);
    ASSERT_EQ(octet_value, octet_dec_value);
    cdr.end_deserialize_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

INSTANTIATE_TEST_SUITE_P(
    XCdrTest,
    XCdrv1Test,
    ::testing::Values(
        Cdr::XCdrHeaderSelection::SHORT_HEADER,
        Cdr::XCdrHeaderSelection::LONG_HEADER,
        Cdr::XCdrHeaderSelection::AUTO_WITH_SHORT_HEADER_BY_DEFAULT,
        Cdr::XCdrHeaderSelection::AUTO_WITH_LONG_HEADER_BY_DEFAULT
        ));
