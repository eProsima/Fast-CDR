#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <vector>

#include <fastcdr/Cdr.h>
#include <fastcdr/xcdr/optional.hpp>

using namespace eprosima::fastcdr;

using XCdrStreamValues =
        std::array<std::vector<uint8_t>,
                1 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS>;


class XCdrOptionalTest : public ::testing::TestWithParam< std::tuple<EncodingAlgorithmFlag, Cdr::Endianness>>
{
};

class InnserOptionalShortStruct
{
public:

    InnserOptionalShortStruct() = default;

    InnserOptionalShortStruct(
            int16_t v)
    {
        value = v;
    }

    bool operator ==(
            const InnserOptionalShortStruct& other) const
    {
        return value == other.value;
    }

    void serialize(
            Cdr& cdr) const
    {
        Cdr::state current_status = cdr.get_state();
        cdr.begin_serialize_type(current_status, cdr.get_encoding_flag());
        cdr << MemberId(3) << value;
        cdr.end_serialize_type(current_status);
    }

    void deserialize(
            Cdr& cdr)
    {
        cdr.deserialize_type(cdr.get_encoding_flag(), [this](Cdr& cdr_inner, const MemberId& mid,
                bool sequence_id) -> bool
                {
                    bool ret_value = true;
                    assert(((EncodingAlgorithmFlag::PL_CDR == cdr_inner.get_encoding_flag() ||
                    EncodingAlgorithmFlag::PL_CDR2 == cdr_inner.get_encoding_flag()) &&
                    !sequence_id) ||
                    (EncodingAlgorithmFlag::PL_CDR != cdr_inner.get_encoding_flag() &&
                    EncodingAlgorithmFlag::PL_CDR2 != cdr_inner.get_encoding_flag() && sequence_id));
                    if (sequence_id)
                    {
                        switch (mid.id)
                        {
                            case 0:
                                cdr_inner >> value;
                                ret_value = false;
                                break;
                            default:
                                break;
                        }

                    }
                    else
                    {
                        switch (mid.id)
                        {
                            case 3:
                                cdr_inner >> value;
                                break;
                            default:
                                break;
                        }
                    }

                    return ret_value;
                });
    }

private:

    optional<int16_t> value;
};

CdrVersion get_version_from_algorithm(
        EncodingAlgorithmFlag ef)
{
    CdrVersion cdr_version { CdrVersion::XCDRv2 };

    switch (ef)
    {
        case EncodingAlgorithmFlag::PLAIN_CDR:
        case EncodingAlgorithmFlag::PL_CDR:
            cdr_version = CdrVersion::XCDRv1;
            break;
        default:
            break;
    }

    return cdr_version;
}

template<class _T>
void serialize_the_value(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _T value)
{
    //{ Prepare buffer
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
    cdr.serialize(value);
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
    _T dec_value {0};
    cdr.deserialize(dec_value);
    ASSERT_EQ(value, dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

template<class _T>
void align_1_serialize_the_value(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _T value)
{
    const uint8_t align_value {0xAB};

    //{ Prepare buffer
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
    cdr << align_value;
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(value);
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
    uint8_t dalign_value {0};
    cdr >> dalign_value;
    ASSERT_EQ(align_value, dalign_value);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    _T dec_value {0};
    cdr.deserialize(dec_value);
    ASSERT_EQ(value, dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

template<class _T>
void align_2_serialize_the_value(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _T value)
{
    const uint16_t align_value {0xABBA};

    //{ Prepare buffer
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
    cdr << align_value;
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(value);
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
    uint16_t dalign_value {0};
    cdr >> dalign_value;
    ASSERT_EQ(align_value, dalign_value);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    _T dec_value {0};
    cdr.deserialize(dec_value);
    ASSERT_EQ(value, dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

template<class _T>
void align_4_serialize_the_value(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _T value)
{
    const uint32_t align_value {0xABABABBA};

    //{ Prepare buffer
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
    cdr << align_value;
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    cdr.serialize(value);
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
    uint32_t dalign_value {0};
    cdr >> dalign_value;
    ASSERT_EQ(align_value, dalign_value);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    _T dec_value {0};
    cdr.deserialize(dec_value);
    ASSERT_EQ(value, dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

template<class _T>
void serialize_optional(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _T value)
{
    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<_T> opt_value {value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << opt_value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    optional<_T> dopt_value;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dopt_value;
    ASSERT_TRUE(dopt_value.has_value());
    ASSERT_EQ(*opt_value, *dopt_value);
    //}
}

template<class _T>
void align_1_serialize_optional(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _T value)
{
    const uint8_t align_value {0xAB};

    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<_T> opt_value {value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << align_value << MemberId(1) << opt_value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    uint8_t dalign_value {0};
    optional<_T> dopt_value;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dalign_value >> dopt_value;
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_TRUE(dopt_value.has_value());
    ASSERT_EQ(*opt_value, *dopt_value);
    //}
}

template<class _T>
void align_2_serialize_optional(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _T value)
{
    const uint16_t align_value {0xABBA};

    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<_T> opt_value {value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << align_value << MemberId(1) << opt_value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    uint16_t dalign_value {0};
    optional<_T> dopt_value;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dalign_value >> dopt_value;
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_TRUE(dopt_value.has_value());
    ASSERT_EQ(*opt_value, *dopt_value);
    //}
}

template<class _T>
void align_4_serialize_optional(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _T value)
{
    const uint32_t align_value {0xABABABBA};

    //{ Prepare buffer
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<_T> opt_value {value};
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << align_value << MemberId(1) << opt_value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding optional not present
    uint32_t dalign_value {0};
    optional<_T> dopt_value;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dalign_value >> dopt_value;
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_TRUE(dopt_value.has_value());
    ASSERT_EQ(*opt_value, *dopt_value);
    //}
}

TEST_P(XCdrOptionalTest, null_serialize_the_value)
{
    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    const int16_t short_value {static_cast<int16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, short_value);
}

TEST_P(XCdrOptionalTest, ushort_serialize_the_value)
{
    const uint16_t ushort_value {static_cast<uint16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, ushort_value);
}

TEST_P(XCdrOptionalTest, long_serialize_the_value)
{
    const int32_t long_value {static_cast<int32_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, long_value);
}

TEST_P(XCdrOptionalTest, ulong_serialize_the_value)
{
    const uint32_t ulong_value {0xCDCDCDDC};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, ulong_value);
}

TEST_P(XCdrOptionalTest, longlong_serialize_the_value)
{
    const int64_t longlong_value {static_cast<int64_t>(0xCDCDCDCDCDCDCDDCll)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, longlong_value);
}

TEST_P(XCdrOptionalTest, ulonglong_serialize_the_value)
{
    const uint64_t ulonglong_value {0xCDCDCDCDCDCDCDDCull};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, ulonglong_value);
}

TEST_P(XCdrOptionalTest, float_serialize_the_value)
{
    const float float_value {13.0};
    constexpr uint8_t ival {0x41};
    constexpr uint8_t fval {0x50};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, float_value);
}

TEST_P(XCdrOptionalTest, double_serialize_the_value)
{
    const double double_value {13.0};
    constexpr uint8_t ival {0x40};
    constexpr uint8_t fval {0x2a};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, double_value);
}

TEST_P(XCdrOptionalTest, longdouble_serialize_the_value)
{
    const long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
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
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    long double longdouble_dec_value {0};
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
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, boolean_value);
}

TEST_P(XCdrOptionalTest, octet_serialize_the_value)
{
    const uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, octet_value);
}

TEST_P(XCdrOptionalTest, char_serialize_the_value)
{
    const char char_value {'}'};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, char_value);
}

TEST_P(XCdrOptionalTest, wchar_serialize_the_value)
{
    const wchar_t wchar_value {static_cast<wchar_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Wchar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, wchar_value);
}

TEST_P(XCdrOptionalTest, null_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value             // Alignment
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value             // Alignment
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value             // Alignment
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value             // Alignment
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    cdr << align_value;
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
    uint8_t dalign_value {0};
    cdr >> dalign_value;
    ASSERT_EQ(align_value, dalign_value);
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

TEST_P(XCdrOptionalTest, null_align_2_serialize_the_value)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t ival {0xAB};
    constexpr uint8_t fval {0xBA};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, fval,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        ival, fval
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fval, ival
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, fval,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival,
        0x00              // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        ival, fval,
        0x00              // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fval, ival,
        0x00              // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        ival, fval
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fval, ival
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    cdr << align_value;
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
    uint16_t dalign_value {0};
    cdr >> dalign_value;
    ASSERT_EQ(align_value, dalign_value);
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

TEST_P(XCdrOptionalTest, null_align_4_serialize_the_value)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t ival {0xAB};
    constexpr uint8_t fval {0xBA};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval,
        0x00, 0x01, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival,
        0x01, 0x00, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival,
        0x00              // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval,
        0x00              // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival,
        0x00              // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    cdr << align_value;
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
    uint32_t dalign_value {0};
    cdr >> dalign_value;
    ASSERT_EQ(align_value, dalign_value);
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

TEST_P(XCdrOptionalTest, short_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};
    const int16_t short_value {static_cast<int16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_the_value(expected_streams, encoding, endianness, short_value);
}

TEST_P(XCdrOptionalTest, short_align_2_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const int16_t short_value {static_cast<int16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_the_value(expected_streams, encoding, endianness, short_value);
}

TEST_P(XCdrOptionalTest, short_align_4_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const int16_t short_value {static_cast<int16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_the_value(expected_streams, encoding, endianness, short_value);
}

TEST_P(XCdrOptionalTest, ushort_align_1_serialize_the_value)
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
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_the_value(expected_streams, encoding, endianness, ushort_value);
}

TEST_P(XCdrOptionalTest, ushort_align_2_serialize_the_value)
{
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
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_the_value(expected_streams, encoding, endianness, ushort_value);
}

TEST_P(XCdrOptionalTest, ushort_align_4_serialize_the_value)
{
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
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_the_value(expected_streams, encoding, endianness, ushort_value);
}

TEST_P(XCdrOptionalTest, long_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};
    const int32_t long_value {static_cast<int32_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_the_value(expected_streams, encoding, endianness, long_value);
}

TEST_P(XCdrOptionalTest, long_align_2_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const int32_t long_value {static_cast<int32_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_the_value(expected_streams, encoding, endianness, long_value);
}

TEST_P(XCdrOptionalTest, long_align_4_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const int32_t long_value {static_cast<int32_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_the_value(expected_streams, encoding, endianness, long_value);
}

TEST_P(XCdrOptionalTest, ulong_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};
    const uint32_t ulong_value {0xCDCDCDDC};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_the_value(expected_streams, encoding, endianness, ulong_value);
}

TEST_P(XCdrOptionalTest, ulong_align_2_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const uint32_t ulong_value {0xCDCDCDDC};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_the_value(expected_streams, encoding, endianness, ulong_value);
}

TEST_P(XCdrOptionalTest, ulong_align_4_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const uint32_t ulong_value {0xCDCDCDDC};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_the_value(expected_streams, encoding, endianness, ulong_value);
}

TEST_P(XCdrOptionalTest, longlong_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};
    const int64_t longlong_value {static_cast<int64_t>(0xCDCDCDCDCDCDCDDCll)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_the_value(expected_streams, encoding, endianness, longlong_value);
}

TEST_P(XCdrOptionalTest, longlong_align_2_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const int64_t longlong_value {static_cast<int64_t>(0xCDCDCDCDCDCDCDDCll)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_the_value(expected_streams, encoding, endianness, longlong_value);
}

TEST_P(XCdrOptionalTest, longlong_align_4_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const int64_t longlong_value {static_cast<int64_t>(0xCDCDCDCDCDCDCDDCll)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_the_value(expected_streams, encoding, endianness, longlong_value);
}

TEST_P(XCdrOptionalTest, ulonglong_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};
    const uint64_t ulonglong_value {0xCDCDCDCDCDCDCDDCll};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_the_value(expected_streams, encoding, endianness, ulonglong_value);
}

TEST_P(XCdrOptionalTest, ulonglong_align_2_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const uint64_t ulonglong_value {0xCDCDCDCDCDCDCDDCll};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_the_value(expected_streams, encoding, endianness, ulonglong_value);
}

TEST_P(XCdrOptionalTest, ulonglong_align_4_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const uint64_t ulonglong_value {0xCDCDCDCDCDCDCDDCll};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_the_value(expected_streams, encoding, endianness, ulonglong_value);
}

TEST_P(XCdrOptionalTest, float_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};
    const float float_value {13.0};
    constexpr uint8_t ival {0x41};
    constexpr uint8_t fval {0x50};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_the_value(expected_streams, encoding, endianness, float_value);
}

TEST_P(XCdrOptionalTest, float_align_2_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const float float_value {13.0};
    constexpr uint8_t ival {0x41};
    constexpr uint8_t fval {0x50};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_the_value(expected_streams, encoding, endianness, float_value);
}

TEST_P(XCdrOptionalTest, float_align_4_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const float float_value {13.0};
    constexpr uint8_t ival {0x41};
    constexpr uint8_t fval {0x50};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_the_value(expected_streams, encoding, endianness, float_value);
}

TEST_P(XCdrOptionalTest, double_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};
    const double double_value {13.0};
    constexpr uint8_t ival {0x40};
    constexpr uint8_t fval {0x2a};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_the_value(expected_streams, encoding, endianness, double_value);
}

TEST_P(XCdrOptionalTest, double_align_2_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const double double_value {13.0};
    constexpr uint8_t ival {0x40};
    constexpr uint8_t fval {0x2a};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_the_value(expected_streams, encoding, endianness, double_value);
}

TEST_P(XCdrOptionalTest, double_align_4_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const double double_value {13.0};
    constexpr uint8_t ival {0x40};
    constexpr uint8_t fval {0x2a};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_the_value(expected_streams, encoding, endianness, double_value);
}

TEST_P(XCdrOptionalTest, longdouble_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};
    const long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // Size
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    cdr << align_value;
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
    uint8_t dalign_value {0};
    cdr >> dalign_value;
    ASSERT_EQ(align_value, dalign_value);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    long double longdouble_dec_value {0};
    cdr.deserialize(longdouble_dec_value);
    ASSERT_EQ(longdouble_value, longdouble_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, longdouble_align_2_serialize_the_value)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // Size
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    cdr << align_value;
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
    uint16_t dalign_value {0};
    cdr >> dalign_value;
    ASSERT_EQ(align_value, dalign_value);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    long double longdouble_dec_value {0};
    cdr.deserialize(longdouble_dec_value);
    ASSERT_EQ(longdouble_value, longdouble_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, longdouble_align_4_serialize_the_value)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // Size
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    cdr << align_value;
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
    uint32_t dalign_value {0};
    cdr >> dalign_value;
    ASSERT_EQ(align_value, dalign_value);
    Cdr::state dec_state(cdr);
    MemberId member_id;
    bool is_present = false;
    cdr.begin_deserialize_opt_member(member_id, is_present, dec_state);
    ASSERT_TRUE(is_present);
    long double longdouble_dec_value {0};
    cdr.deserialize(longdouble_dec_value);
    ASSERT_EQ(longdouble_value, longdouble_dec_value);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, boolean_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};
    const bool boolean_value = true;
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_the_value(expected_streams, encoding, endianness, boolean_value);
}

TEST_P(XCdrOptionalTest, boolean_align_2_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const bool boolean_value = true;
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_the_value(expected_streams, encoding, endianness, boolean_value);
}

TEST_P(XCdrOptionalTest, boolean_align_4_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const bool boolean_value = true;
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_the_value(expected_streams, encoding, endianness, boolean_value);
}

TEST_P(XCdrOptionalTest, octet_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};
    const uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_the_value(expected_streams, encoding, endianness, octet_value);
}

TEST_P(XCdrOptionalTest, octet_align_2_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_the_value(expected_streams, encoding, endianness, octet_value);
}

TEST_P(XCdrOptionalTest, octet_align_4_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_the_value(expected_streams, encoding, endianness, octet_value);
}

TEST_P(XCdrOptionalTest, char_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};
    const char char_value {'}'};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_the_value(expected_streams, encoding, endianness, char_value);
}

TEST_P(XCdrOptionalTest, char_align_2_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const char char_value {'}'};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value             // Char
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_the_value(expected_streams, encoding, endianness, char_value);
}

TEST_P(XCdrOptionalTest, char_align_4_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const char char_value {'}'};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value             // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_the_value(expected_streams, encoding, endianness, char_value);
}

TEST_P(XCdrOptionalTest, wchar_align_1_serialize_the_value)
{
    const uint8_t align_value {0xAB};
    const wchar_t wchar_value {static_cast<wchar_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // WChar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_the_value(expected_streams, encoding, endianness, wchar_value);
}

TEST_P(XCdrOptionalTest, wchar_align_2_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const wchar_t wchar_value {static_cast<wchar_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // WChar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_the_value(expected_streams, encoding, endianness, wchar_value);
}

TEST_P(XCdrOptionalTest, wchar_align_4_serialize_the_value)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const wchar_t wchar_value {static_cast<wchar_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_the_value(expected_streams, encoding, endianness, wchar_value);
}

TEST_P(XCdrOptionalTest, two_inner_null_serialize_the_value)
{
    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // DHEADER
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // DHEADER
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // DHEADER
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // DHEADER
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    Cdr::state enc_state_inner_type(cdr);
    cdr.begin_serialize_type(enc_state_inner_type, encoding);
    Cdr::state enc_state_inner(cdr);
    cdr.begin_serialize_opt_member(MemberId(3), false, enc_state_inner);
    cdr.end_serialize_opt_member(enc_state_inner);
    cdr.end_serialize_type(enc_state_inner_type);
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
    cdr.deserialize_type(encoding, [encoding](Cdr& cdr_inner, const MemberId& mid, bool sequence_id) -> bool
            {
                assert(((EncodingAlgorithmFlag::PL_CDR == encoding || EncodingAlgorithmFlag::PL_CDR2 == encoding) &&
                !sequence_id) ||
                (EncodingAlgorithmFlag::PL_CDR != encoding && EncodingAlgorithmFlag::PL_CDR2 != encoding && sequence_id));
                bool is_present_inner = false;
                Cdr::state dec_state_inner(cdr_inner);
                MemberId member_id_inner = sequence_id ? MEMBER_ID_INVALID : mid;
                cdr_inner.begin_deserialize_opt_member(member_id_inner, is_present_inner, dec_state_inner);
                assert(!is_present_inner);
                cdr_inner.end_deserialize_opt_member(dec_state_inner);
                return sequence_id ? false : true;
            });
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, two_inner_short_serialize_the_value)
{
    const int16_t short_value {static_cast<int16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x06, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x06, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x0c, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x0c, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x01,                   // Present
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x01,                   // Present
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x04, // DHEADER
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x04, 0x00, 0x00, 0x00, // DHEADER
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0a, // NEXTINT
        0x00, 0x00, 0x00, 0x06, // DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) without NEXTINT
        0x0a, 0x00, 0x00, 0x00, // NEXTINT
        0x06, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_opt_member(MemberId(1), true, enc_state);
    Cdr::state enc_state_inner_type(cdr);
    cdr.begin_serialize_type(enc_state_inner_type, encoding);
    Cdr::state enc_state_inner(cdr);
    cdr.begin_serialize_opt_member(MemberId(3), true, enc_state_inner);
    cdr.serialize(short_value);
    cdr.end_serialize_opt_member(enc_state_inner);
    cdr.end_serialize_type(enc_state_inner_type);
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
    int16_t dvalue {0};
    cdr.deserialize_type(encoding, [encoding, &dvalue](Cdr& cdr_inner, const MemberId& mid, bool sequence_id) -> bool
            {
                assert(((EncodingAlgorithmFlag::PL_CDR == encoding || EncodingAlgorithmFlag::PL_CDR2 == encoding) &&
                !sequence_id) ||
                (EncodingAlgorithmFlag::PL_CDR != encoding && EncodingAlgorithmFlag::PL_CDR2 != encoding && sequence_id));
                bool is_present_inner = false;
                Cdr::state dec_state_inner(cdr_inner);
                MemberId member_id_inner = sequence_id ? MEMBER_ID_INVALID : mid;
                cdr_inner.begin_deserialize_opt_member(member_id_inner, is_present_inner, dec_state_inner);
                assert(is_present_inner);
                cdr_inner.deserialize(dvalue);
                cdr_inner.end_deserialize_opt_member(dec_state_inner);
                return sequence_id ? false : true;
            });
    ASSERT_EQ(short_value, dvalue);
    cdr.end_deserialize_opt_member(dec_state);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

TEST_P(XCdrOptionalTest, null_optional)
{
    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    constexpr int16_t short_value {static_cast<int16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional(expected_streams, encoding, endianness, short_value);
}

TEST_P(XCdrOptionalTest, ushort_optional)
{
    constexpr uint16_t ushort_value {static_cast<uint16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, ival, fval        // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, fval, ival        // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional(expected_streams, encoding, endianness, ushort_value);
}

TEST_P(XCdrOptionalTest, long_optional)
{
    constexpr int32_t long_value {static_cast<int32_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional(expected_streams, encoding, endianness, long_value);
}

TEST_P(XCdrOptionalTest, ulong_optional)
{
    constexpr uint32_t ulong_value {0xCDCDCDDC};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional(expected_streams, encoding, endianness, ulong_value);
}

TEST_P(XCdrOptionalTest, longlong_optional)
{
    constexpr int64_t longlong_value {static_cast<int64_t>(0xCDCDCDCDCDCDCDDCll)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional(expected_streams, encoding, endianness, longlong_value);
}

TEST_P(XCdrOptionalTest, ulonglong_optional)
{
    constexpr uint64_t ulonglong_value {0xCDCDCDCDCDCDCDDCull};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional(expected_streams, encoding, endianness, ulonglong_value);
}

TEST_P(XCdrOptionalTest, float_optional)
{
    constexpr float float_value {13.0};
    constexpr uint8_t ival {0x41};
    constexpr uint8_t fval {0x50};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional(expected_streams, encoding, endianness, float_value);
}

TEST_P(XCdrOptionalTest, double_optional)
{
    constexpr double double_value {13.0};
    constexpr uint8_t ival {0x40};
    constexpr uint8_t fval {0x2a};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional(expected_streams, encoding, endianness, double_value);
}

TEST_P(XCdrOptionalTest, longdouble_optional)
{
    constexpr long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
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
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional(expected_streams, encoding, endianness, boolean_value);
}

TEST_P(XCdrOptionalTest, octet_optional)
{
    constexpr uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional(expected_streams, encoding, endianness, octet_value);
}

TEST_P(XCdrOptionalTest, char_optional)
{
    constexpr char char_value {'}'};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional(expected_streams, encoding, endianness, char_value);
}

TEST_P(XCdrOptionalTest, wchar_optional)
{
    constexpr wchar_t wchar_value {static_cast<wchar_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Wchar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional(expected_streams, encoding, endianness, wchar_value);
}

TEST_P(XCdrOptionalTest, null_align_1_optional)
{
    const uint8_t align_value {0xAB};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    cdr << align_value << MemberId(1) << value;
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
    uint8_t dalign_value {0};
    cdr >> dalign_value >> dvalue;
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_FALSE(dvalue.has_value());
    //}
}

TEST_P(XCdrOptionalTest, null_align_2_optional)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t ival {0xAB};
    constexpr uint8_t fval {0xBA};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, fval,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        ival, fval
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fval, ival
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, fval,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        ival, fval,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fval, ival,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        ival, fval
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fval, ival
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    cdr << align_value << MemberId(1) << value;
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
    uint16_t dalign_value {0};
    cdr >> dalign_value >> dvalue;
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_FALSE(dvalue.has_value());
    //}
}

TEST_P(XCdrOptionalTest, null_align_4_optional)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t ival {0xAB};
    constexpr uint8_t fval {0xBA};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval,
        0x00, 0x01, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival,
        0x01, 0x00, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival,
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    cdr << align_value << MemberId(1) << value;
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
    uint32_t dalign_value {0};
    cdr >> dalign_value >> dvalue;
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_FALSE(dvalue.has_value());
    //}
}

TEST_P(XCdrOptionalTest, short_align_1_optional)
{
    const uint8_t align_value {0xAB};
    const int16_t short_value {static_cast<int16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_optional(expected_streams, encoding, endianness, short_value);
}

TEST_P(XCdrOptionalTest, short_align_2_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const int16_t short_value {static_cast<int16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_optional(expected_streams, encoding, endianness, short_value);
}

TEST_P(XCdrOptionalTest, short_align_4_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    const int16_t short_value {static_cast<int16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_optional(expected_streams, encoding, endianness, short_value);
}

TEST_P(XCdrOptionalTest, ushort_align_1_optional)
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
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_optional(expected_streams, encoding, endianness, ushort_value);
}

TEST_P(XCdrOptionalTest, ushort_align_2_optional)
{
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
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_optional(expected_streams, encoding, endianness, ushort_value);
}

TEST_P(XCdrOptionalTest, ushort_align_4_optional)
{
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
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_optional(expected_streams, encoding, endianness, ushort_value);
}

TEST_P(XCdrOptionalTest, long_align_1_optional)
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
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_optional(expected_streams, encoding, endianness, long_value);
}

TEST_P(XCdrOptionalTest, long_align_2_optional)
{
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
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_optional(expected_streams, encoding, endianness, long_value);
}

TEST_P(XCdrOptionalTest, long_align_4_optional)
{
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
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_optional(expected_streams, encoding, endianness, long_value);
}

TEST_P(XCdrOptionalTest, ulong_align_1_optional)
{
    const uint8_t align_value {0xAB};
    constexpr uint32_t ulong_value {0xCDCDCDDC};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_optional(expected_streams, encoding, endianness, ulong_value);
}

TEST_P(XCdrOptionalTest, ulong_align_2_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint32_t ulong_value {0xCDCDCDDC};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_optional(expected_streams, encoding, endianness, ulong_value);
}

TEST_P(XCdrOptionalTest, ulong_align_4_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint32_t ulong_value {0xCDCDCDDC};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_optional(expected_streams, encoding, endianness, ulong_value);
}

TEST_P(XCdrOptionalTest, longlong_align_1_optional)
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
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_optional(expected_streams, encoding, endianness, longlong_value);
}

TEST_P(XCdrOptionalTest, longlong_align_2_optional)
{
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
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_optional(expected_streams, encoding, endianness, longlong_value);
}

TEST_P(XCdrOptionalTest, longlong_align_4_optional)
{
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
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_optional(expected_streams, encoding, endianness, longlong_value);
}

TEST_P(XCdrOptionalTest, ulonglong_align_1_optional)
{
    const uint8_t align_value {0xAB};
    constexpr uint64_t ulonglong_value {0xCDCDCDCDCDCDCDDCll};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_optional(expected_streams, encoding, endianness, ulonglong_value);
}

TEST_P(XCdrOptionalTest, ulonglong_align_2_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint64_t ulonglong_value {0xCDCDCDCDCDCDCDDCll};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_optional(expected_streams, encoding, endianness, ulonglong_value);
}

TEST_P(XCdrOptionalTest, ulonglong_align_4_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint64_t ulonglong_value {0xCDCDCDCDCDCDCDDCll};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,        // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_optional(expected_streams, encoding, endianness, ulonglong_value);
}

TEST_P(XCdrOptionalTest, float_align_1_optional)
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
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_optional(expected_streams, encoding, endianness, float_value);
}

TEST_P(XCdrOptionalTest, float_align_2_optional)
{
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
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_optional(expected_streams, encoding, endianness, float_value);
}

TEST_P(XCdrOptionalTest, float_align_4_optional)
{
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
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, fval, ival  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_optional(expected_streams, encoding, endianness, float_value);
}

TEST_P(XCdrOptionalTest, double_align_1_optional)
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
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_optional(expected_streams, encoding, endianness, double_value);
}

TEST_P(XCdrOptionalTest, double_align_2_optional)
{
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
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_optional(expected_streams, encoding, endianness, double_value);
}

TEST_P(XCdrOptionalTest, double_align_4_optional)
{
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
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x08, // ShortMemberHeader
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x08, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_optional(expected_streams, encoding, endianness, double_value);
}

TEST_P(XCdrOptionalTest, longdouble_align_1_optional)
{
    const uint8_t align_value {0xAB};
    constexpr long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // Size
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    cdr << align_value << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0,
            memcmp(buffer.get(), expected_streams[tested_stream].data(),
            expected_streams[tested_stream].size() - sizeof(long double)));
    //}

    //{ Decoding optional not present
    uint8_t dalign_value {0};
    optional<long double> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dalign_value >> dvalue;
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, longdouble_align_2_optional)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // Size
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    cdr << align_value << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0,
            memcmp(buffer.get(), expected_streams[tested_stream].data(),
            expected_streams[tested_stream].size() - sizeof(long double)));
    //}

    //{ Decoding optional not present
    uint16_t dalign_value {0};
    optional<long double> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dalign_value >> dvalue;
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, longdouble_align_4_optional)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr long double longdouble_value = std::numeric_limits<long double>::min();

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
        0x00, 0x00, 0x55, 0x55, // LongDouble
        0x55, 0x56, 0x00, 0x01, // LongDouble
        0x80, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x00  // LongDouble
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) with NEXTINT
        0x10, 0x00, 0x00, 0x00, // Size
        0x00, 0x00, 0x00, 0x00, // LongDouble
        0x00, 0x00, 0x00, 0x80, // LongDouble
        0x01, 0x00, 0x56, 0x55, // LongDouble
        0x55, 0x55, 0x00, 0x80  // LongDouble
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
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
    cdr << align_value << MemberId(1) << value;
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0,
            memcmp(buffer.get(), expected_streams[tested_stream].data(),
            expected_streams[tested_stream].size() - sizeof(long double)));
    //}

    //{ Decoding optional not present
    uint32_t dalign_value {0};
    optional<long double> dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr >> dalign_value >> dvalue;
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_TRUE(dvalue.has_value());
    ASSERT_EQ(*value, *dvalue);
    //}
}

TEST_P(XCdrOptionalTest, boolean_align_1_optional)
{
    const uint8_t align_value {0xAB};
    constexpr bool boolean_value = true;
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_optional(expected_streams, encoding, endianness, boolean_value);
}

TEST_P(XCdrOptionalTest, boolean_align_2_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr bool boolean_value = true;
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_optional(expected_streams, encoding, endianness, boolean_value);
}

TEST_P(XCdrOptionalTest, boolean_align_4_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr bool boolean_value = true;
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_optional(expected_streams, encoding, endianness, boolean_value);
}

TEST_P(XCdrOptionalTest, octet_align_1_optional)
{
    const uint8_t align_value {0xAB};
    constexpr uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_optional(expected_streams, encoding, endianness, octet_value);
}

TEST_P(XCdrOptionalTest, octet_align_2_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_optional(expected_streams, encoding, endianness, octet_value);
}

TEST_P(XCdrOptionalTest, octet_align_4_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_optional(expected_streams, encoding, endianness, octet_value);
}

TEST_P(XCdrOptionalTest, char_align_1_optional)
{
    const uint8_t align_value {0xAB};
    constexpr char char_value {'}'};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_optional(expected_streams, encoding, endianness, char_value);
}

TEST_P(XCdrOptionalTest, char_align_2_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr char char_value {'}'};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value             // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_optional(expected_streams, encoding, endianness, char_value);
}

TEST_P(XCdrOptionalTest, char_align_4_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr char char_value {'}'};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value             // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_optional(expected_streams, encoding, endianness, char_value);
}

TEST_P(XCdrOptionalTest, wchar_align_1_optional)
{
    const uint8_t align_value {0xAB};
    constexpr wchar_t wchar_value {static_cast<wchar_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        align_value,
        0x01,                   // Present
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // WChar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_1_serialize_optional(expected_streams, encoding, endianness, wchar_value);
}

TEST_P(XCdrOptionalTest, wchar_align_2_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr wchar_t wchar_value {static_cast<wchar_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x01,                   // Present
        0x00,                   // Alignment
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // WChar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_2_serialize_optional(expected_streams, encoding, endianness, wchar_value);
}

TEST_P(XCdrOptionalTest, wchar_align_4_optional)
{
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr wchar_t wchar_value {static_cast<wchar_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_4_serialize_optional(expected_streams, encoding, endianness, wchar_value);
}

TEST_P(XCdrOptionalTest, two_inner_null_optional)
{
    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x00, 0x00  // ShortMemberHeader
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // DHEADER
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // DHEADER
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // DHEADER
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // DHEADER
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional not present.
    optional<InnserOptionalShortStruct> opt_value { {} };
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << opt_value;
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
    optional<InnserOptionalShortStruct> dopt_value;
    cdr >> dopt_value;
    ASSERT_EQ(opt_value.has_value(), dopt_value.has_value());
    ASSERT_EQ(*opt_value, *dopt_value);
    //}
}

TEST_P(XCdrOptionalTest, two_inner_short_optional)
{
    const int16_t short_value {static_cast<int16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x06, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x02, // ShortMemberHeader
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x06, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x0c, // ShortMemberHeader
        0x00, 0x03, 0x00, 0x02, // ShortMemberHeader
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x0c, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x02, 0x00, // ShortMemberHeader
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x01,                   // Present
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x01,                   // Present
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x04, // DHEADER
        0x01,                   // Present
        0x00,                   // Alignment
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x04, 0x00, 0x00, 0x00, // DHEADER
        0x01,                   // Present
        0x00,                   // Alignment
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0a, // NEXTINT
        0x00, 0x00, 0x00, 0x06, // DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x40, // EMHEADER1(M) without NEXTINT
        0x0a, 0x00, 0x00, 0x00, // NEXTINT
        0x06, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    //{ Prepare buffer
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    uint8_t tested_stream = 0 + encoding + endianness;
    auto buffer =
            std::unique_ptr<char, void (*)(
        void*)>{reinterpret_cast<char*>(calloc(expected_streams[tested_stream].size(), sizeof(char))), free};
    FastBuffer fast_buffer(buffer.get(), expected_streams[tested_stream].size());
    Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
    //}

    //{ Encode optional.
    optional<InnserOptionalShortStruct> opt_value { {short_value} };
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    cdr << MemberId(1) << opt_value;
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
    optional<InnserOptionalShortStruct> dopt_value;
    cdr >> dopt_value;
    ASSERT_EQ(opt_value.has_value(), dopt_value.has_value());
    ASSERT_EQ(*opt_value, *dopt_value);
    //}
}

INSTANTIATE_TEST_SUITE_P(
    XCdrTest,
    XCdrOptionalTest,
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

