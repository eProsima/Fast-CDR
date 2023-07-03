#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <vector>

#include <fastcdr/Cdr.h>

using namespace eprosima::fastcdr;

static CdrVersion get_version_from_algorithm(
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

class InnerBasicTypesShortStruct
{
public:

    InnerBasicTypesShortStruct() = default;

    InnerBasicTypesShortStruct(
            uint16_t v)
    {
        value1_ = v;
        value2_ = v;
    }

    bool operator ==(
            const InnerBasicTypesShortStruct& other) const
    {
        return value1_ == other.value1_ && value2_ == other.value2_;
    }

    void serialize(
            Cdr& cdr) const
    {
        Cdr::state current_status(cdr);
        cdr.begin_serialize_type(current_status, cdr.get_encoding_flag());
        cdr << MemberId(3) << value1_;
        cdr << MemberId(0x3FFF) << value2_;
        cdr.end_serialize_type(current_status);
    }

    void deserialize(
            Cdr& cdr)
    {
        cdr.deserialize_type(cdr.get_encoding_flag(), [this](Cdr& cdr_inner, const MemberId& mid) -> bool
                {
                    bool ret_value = true;
                    if (EncodingAlgorithmFlag::PL_CDR == cdr_inner.get_encoding_flag() ||
                    EncodingAlgorithmFlag::PL_CDR2 == cdr_inner.get_encoding_flag())
                    {
                        switch (mid.id)
                        {
                            case 3:
                                cdr_inner >> value1_;
                                break;
                            case 0x3FFF:
                                cdr_inner >> value2_;
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
                                cdr_inner >> value1_;
                                break;
                            case 1:
                                cdr_inner >> value2_;
                                break;
                            default:
                                ret_value = false;
                                break;
                        }
                    }

                    return ret_value;
                });
    }

private:

    uint16_t value1_ {0};

    uint16_t value2_ {0};
};

class InnerBasicTypesStruct
{
public:

    InnerBasicTypesStruct() = default;

    InnerBasicTypesStruct(
            uint16_t v)
    {
        value1_ = v;
        value2_ = v;
    }

    bool operator ==(
            const InnerBasicTypesStruct& other) const
    {
        return value1_ == other.value1_ && value2_ == other.value2_;
    }

    void serialize(
            Cdr& cdr) const
    {
        Cdr::state current_status(cdr);
        cdr.begin_serialize_type(current_status, cdr.get_encoding_flag());
        cdr << MemberId(0) << value1_;
        cdr << MemberId(1) << value2_;
        cdr.end_serialize_type(current_status);
    }

    void deserialize(
            Cdr& cdr)
    {
        cdr.deserialize_type(cdr.get_encoding_flag(), [this](Cdr& cdr_inner, const MemberId& mid) -> bool
                {
                    bool ret_value = true;
                    switch (mid.id)
                    {
                        case 0:
                            cdr_inner >> value1_;
                            break;
                        case 1:
                            cdr_inner >> value2_;
                            break;
                        default:
                            ret_value = false;
                            break;
                    }

                    return ret_value;
                });
    }

private:

    InnerBasicTypesShortStruct value1_;

    uint16_t value2_ {0};
};

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

    //{ Encoding value.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr.serialize_member(MemberId(1), value);
    cdr.end_serialize_type(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding value.
    _T dvalue;
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

template<class _Align, class _T>
void align_serialize_the_value(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _Align align_value,
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

    //{ Encode the value.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr.serialize_member(MemberId(0), align_value);
    cdr.serialize_member(MemberId(1), value);
    cdr.end_serialize_type(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
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

                return ret_value;
            });
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_EQ(value, dvalue);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

template<class _Align>
void longdouble_align_serialize_the_value(
        const XCdrStreamValues& expected_streams_begin,
        const XCdrStreamValues& expected_streams_end,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _Align align_value,
        long double value)
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
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr.serialize_member(MemberId(0), align_value);
    cdr.serialize_member(MemberId(1), value);
    cdr.end_serialize_type(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), total_size);
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams_begin[tested_stream].data(),
            expected_streams_begin[tested_stream].size()));
    ASSERT_EQ(0,
            memcmp(buffer.get() + cdr.getSerializedDataLength() - expected_streams_end[tested_stream].size(),
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

                return ret_value;
            });
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_EQ(value, dvalue);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

template<class _T>
void serialize(
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

    //{ Encode the value.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(1) << value;
    cdr.end_serialize_type(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
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

template<class _Align, class _T>
void align_serialize(
        const XCdrStreamValues& expected_streams,
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        _Align align_value,
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

    //{ Encode the value.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(0) << align_value << MemberId(1) << value;
    cdr.end_serialize_type(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
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
        long double value)
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
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(0) << align_value;
    cdr << MemberId(1) << value;
    cdr.end_serialize_type(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), total_size);
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams_begin[tested_stream].data(),
            expected_streams_begin[tested_stream].size()));
    ASSERT_EQ(0,
            memcmp(buffer.get() + cdr.getSerializedDataLength() - expected_streams_end[tested_stream].size(),
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

                return ret_value;
            });
    ASSERT_EQ(align_value, dalign_value);
    ASSERT_EQ(value, dvalue);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

template<class _T>
void appendable_serialize_the_value(
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

    //{ Encoding value.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr.serialize_appendable_member(MemberId(1), value);
    cdr.end_serialize_type(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
    ASSERT_EQ(0, memcmp(buffer.get(), expected_streams[tested_stream].data(), expected_streams[tested_stream].size()));
    //}

    //{ Decoding value.
    _T dvalue;
    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId&)->bool
            {
                bool ret_value = true;

                cdr_inner.deserialize_appendable_member(dvalue);

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

template<class _T>
void appendable_serialize(
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

    //{ Encode the value.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    cdr << MemberId(1) << appendable << value;
    cdr.end_serialize_type(enc_state);
    Cdr::state enc_state_end(cdr);
    //}

    //{ Test encoded content
    ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
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

                cdr_inner >> appendable >> dvalue;

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

TEST_P(XCdrBasicTypesTest, short)
{
    constexpr int16_t short_value {static_cast<int16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, short_value);

    serialize(expected_streams, encoding, endianness, short_value);
}

TEST_P(XCdrBasicTypesTest, ushort)
{
    constexpr uint16_t ushort_value {static_cast<uint16_t>(0xCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, ushort_value);

    serialize(expected_streams, encoding, endianness, ushort_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, long_value);

    serialize(expected_streams, encoding, endianness, long_value);
}

TEST_P(XCdrBasicTypesTest, ulong)
{
    constexpr uint32_t ulong_value {0xCDCDCDDC};
    constexpr uint8_t ival {0xCD};
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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, ulong_value);

    serialize(expected_streams, encoding, endianness, ulong_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, longlong_value);

    serialize(expected_streams, encoding, endianness, longlong_value);
}

TEST_P(XCdrBasicTypesTest, ulonglong)
{
    constexpr uint64_t ulonglong_value {0xCDCDCDCDCDCDCDDCull};
    constexpr uint8_t ival {0xCD};
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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, ulonglong_value);

    serialize(expected_streams, encoding, endianness, ulonglong_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, float_value);

    serialize(expected_streams, encoding, endianness, float_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, double_value);

    serialize(expected_streams, encoding, endianness, double_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x18, // DHEADER
        0x40, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x10, // Size
    };
    expected_streams_end[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
    };
    expected_streams_begin[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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
        Cdr::state enc_state_end(cdr);
        //}

        //{ Test encoded content
        ASSERT_EQ(cdr.getSerializedDataLength(), total_size);
        ASSERT_EQ(0, memcmp(buffer.get(), expected_streams_begin[tested_stream].data(),
                expected_streams_begin[tested_stream].size()));
        ASSERT_EQ(0,
                memcmp(buffer.get() + cdr.getSerializedDataLength() - expected_streams_end[tested_stream].size(),
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
        Cdr::state enc_state_end(cdr);
        //}

        //{ Test encoded content
        ASSERT_EQ(cdr.getSerializedDataLength(), total_size);
        ASSERT_EQ(0, memcmp(buffer.get(), expected_streams_begin[tested_stream].data(),
                expected_streams_begin[tested_stream].size()));
        ASSERT_EQ(0,
                memcmp(buffer.get() + cdr.getSerializedDataLength() - expected_streams_end[tested_stream].size(),
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

TEST_P(XCdrBasicTypesTest, boolean)
{
    constexpr bool boolean_value {true};
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // DHEADER
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // DHEADER
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, boolean_value);

    serialize(expected_streams, encoding, endianness, boolean_value);
}

TEST_P(XCdrBasicTypesTest, octet)
{
    constexpr uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // DHEADER
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // DHEADER
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, octet_value);

    serialize(expected_streams, encoding, endianness, octet_value);
}

TEST_P(XCdrBasicTypesTest, char)
{
    constexpr char char_value {'}'};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value,             // Char
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value,             // Char
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // DHEADER
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // DHEADER
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, char_value);

    serialize(expected_streams, encoding, endianness, char_value);
}

TEST_P(XCdrBasicTypesTest, wchar)
{
    constexpr wchar_t wchar_value {static_cast<wchar_t>(0xCDCDCDDC)};
    constexpr uint8_t ival {0xCD};
    constexpr uint8_t fval {0xDC};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval, // Wchar
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // Wchar
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, ival, ival  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Wchar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Wchar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, wchar_value);

    serialize(expected_streams, encoding, endianness, wchar_value);
}

TEST_P(XCdrBasicTypesTest, string)
{
    const std::string string_value {"AB"};
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x07, // DHEADER
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x07, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0b, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0b, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, string_value);

    serialize(expected_streams, encoding, endianness, string_value);
}

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
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x0c, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB, // WString
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x0c, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00, // WString
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x60, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x60, // EMHEADER1(M) with NEXTINT
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, string_value);

    serialize(expected_streams, encoding, endianness, string_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x01  // Enum
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x01, 0x00, 0x00, 0x00  // Enum
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, enum_value);

    serialize(expected_streams, encoding, endianness, enum_value);
}

TEST_P(XCdrBasicTypesTest, enum16)
{
    constexpr Enum16 enum_value {ENUM16_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x00, 0x01              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x00, 0x01              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01, 0x00              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        0x00, 0x01              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x01              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x01, 0x00              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, enum_value);

    serialize(expected_streams, encoding, endianness, enum_value);
}

TEST_P(XCdrBasicTypesTest, enum8)
{
    constexpr Enum8 enum_value {ENUM8_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        0x01             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        0x01             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        0x01,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        0x01,            // Octet
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        0x01             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x01             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // DHEADER
        0x01             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // DHEADER
        0x01             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x01             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        0x01             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, enum_value);

    serialize(expected_streams, encoding, endianness, enum_value);
}

TEST_P(XCdrBasicTypesTest, array_final_ulong)
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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x30, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, array_value);

    serialize(expected_streams, encoding, endianness, array_value);
}

TEST_P(XCdrBasicTypesTest, array_appendable_ulong)
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
        0x00, 0x00, 0x00, 0x08, // DHEADER
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        0x00, 0x00, 0x00, 0x08, // DHEADER
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x08, // DHEADER + NEXTINT
        ival, ival, ival, fval, // ULong
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) without NEXTINT
        0x08, 0x00, 0x00, 0x00, // DHEADER + NEXTINT
        fval, ival, ival, ival, // ULong
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    appendable_serialize_the_value(expected_streams, encoding, endianness, array_value);

    appendable_serialize(expected_streams, encoding, endianness, array_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x2E, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x26, // DHEADER + NEXTINT
        0x00, 0x00, 0x00, 0x0e, // DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        ival, fval,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        ival, fval,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x0e, // DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        fval, ival,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        fval, ival
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x2E, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x26, 0x00, 0x00, 0x00, // DHEADER + NEXTINT
        0x0e, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,
        0x00, 0x00,             // Alignment
        0xFF, 0x3F, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival,
        0x00, 0x00,             // Alignment
        0x0e, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        ival, fval,
        0x00, 0x00,             // Alignment
        0xFF, 0x3F, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        ival, fval
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_the_value(expected_streams, encoding, endianness, array_value);

    serialize(expected_streams, encoding, endianness, array_value);

    appendable_serialize_the_value(expected_streams, encoding, endianness, array_value);

    appendable_serialize(expected_streams, encoding, endianness, array_value);
}

TEST_P(XCdrBasicTypesTest, short_align_1)
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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0e, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0e, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, short_value);

    align_serialize(expected_streams, encoding, endianness, align_value, short_value);
}

TEST_P(XCdrBasicTypesTest, short_align_2)
{
    const uint16_t align_value {0xABBA};
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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0e, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0e, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, short_value);

    align_serialize(expected_streams, encoding, endianness, align_value, short_value);
}

TEST_P(XCdrBasicTypesTest, short_align_4)
{
    const uint32_t align_value {0xABABABBA};
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
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        iava, iava, iava, fava,
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0e, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0e, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // Short
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, short_value);

    align_serialize(expected_streams, encoding, endianness, align_value, short_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0e, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0e, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, ushort_value);

    align_serialize(expected_streams, encoding, endianness, align_value, ushort_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0e, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0e, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, ushort_value);

    align_serialize(expected_streams, encoding, endianness, align_value, ushort_value);
}

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
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        iava, iava, iava, fava,
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        fval, ival              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0e, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval              // UShort
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0e, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fval, ival              // UShort
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, ushort_value);

    align_serialize(expected_streams, encoding, endianness, align_value, ushort_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, long_value);

    align_serialize(expected_streams, encoding, endianness, align_value, long_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, long_value);

    align_serialize(expected_streams, encoding, endianness, align_value, long_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // Long
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // Long
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, long_value);

    align_serialize(expected_streams, encoding, endianness, align_value, long_value);
}

TEST_P(XCdrBasicTypesTest, ulong_align_1)
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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, ulong_value);

    align_serialize(expected_streams, encoding, endianness, align_value, ulong_value);
}

TEST_P(XCdrBasicTypesTest, ulong_align_2)
{
    const uint16_t align_value {0xABBA};
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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, ulong_value);

    align_serialize(expected_streams, encoding, endianness, align_value, ulong_value);
}

TEST_P(XCdrBasicTypesTest, ulong_align_4)
{
    const uint32_t align_value {0xABABABBA};
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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // ULong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // ULong
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, ulong_value);

    align_serialize(expected_streams, encoding, endianness, align_value, ulong_value);
}

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
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
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
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, longlong_value);

    align_serialize(expected_streams, encoding, endianness, align_value, longlong_value);
}

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
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
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
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, longlong_value);

    align_serialize(expected_streams, encoding, endianness, align_value, longlong_value);
}

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
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        iava, iava, iava, fava,
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        fval, ival, ival, ival, // LongLong
        ival, ival, ival, ival  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // LongLong
        ival, ival, ival, fval  // LongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, longlong_value);

    align_serialize(expected_streams, encoding, endianness, align_value, longlong_value);
}

TEST_P(XCdrBasicTypesTest, ulonglong_align_1)
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
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
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
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, ulonglong_value);

    align_serialize(expected_streams, encoding, endianness, align_value, ulonglong_value);
}

TEST_P(XCdrBasicTypesTest, ulonglong_align_2)
{
    const uint16_t align_value {0xABBA};
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
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
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
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, ulonglong_value);

    align_serialize(expected_streams, encoding, endianness, align_value, ulonglong_value);
}

TEST_P(XCdrBasicTypesTest, ulonglong_align_4)
{
    const uint32_t align_value {0xABABABBA};
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
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        iava, iava, iava, fava,
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        fval, ival, ival, ival, // ULongLong
        ival, ival, ival, ival  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, ival, // ULongLong
        ival, ival, ival, fval  // ULongLong
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, ulonglong_value);

    align_serialize(expected_streams, encoding, endianness, align_value, ulonglong_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, float_value);

    align_serialize(expected_streams, encoding, endianness, align_value, float_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, float_value);

    align_serialize(expected_streams, encoding, endianness, align_value, float_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00  // Float
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, fval, ival  // Float
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, float_value);

    align_serialize(expected_streams, encoding, endianness, align_value, float_value);
}

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
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
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
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, double_value);

    align_serialize(expected_streams, encoding, endianness, align_value, double_value);
}

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
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
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
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, double_value);

    align_serialize(expected_streams, encoding, endianness, align_value, double_value);
}

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
        0x00, 0x00, 0x00, 0x0c, // DHEADER
        iava, iava, iava, fava,
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0c, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        0x00, 0x00, 0x00, 0x00, // Double
        0x00, 0x00, fval, ival  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x14, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x30, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, fval, 0x00, 0x00, // Double
        0x00, 0x00, 0x00, 0x00  // Double
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, double_value);

    align_serialize(expected_streams, encoding, endianness, align_value, double_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
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
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    longdouble_align_serialize_the_value(expected_streams_begin, expected_streams_end, encoding, endianness,
            align_value, longdouble_value);

    longdouble_align_serialize(expected_streams_begin, expected_streams_end, encoding, endianness,
            align_value, longdouble_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
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
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    longdouble_align_serialize_the_value(expected_streams_begin, expected_streams_end, encoding, endianness,
            align_value, longdouble_value);

    longdouble_align_serialize(expected_streams_begin, expected_streams_end, encoding, endianness,
            align_value, longdouble_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
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
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    longdouble_align_serialize_the_value(expected_streams_begin, expected_streams_end, encoding, endianness,
            align_value, longdouble_value);

    longdouble_align_serialize(expected_streams_begin, expected_streams_end, encoding, endianness,
            align_value, longdouble_value);
}

TEST_P(XCdrBasicTypesTest, boolean_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr bool boolean_value {true};
    constexpr uint8_t b_value {0x1};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        align_value,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0d, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0d, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, boolean_value);

    align_serialize(expected_streams, encoding, endianness, align_value, boolean_value);
}

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
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // DHEADER
        iava, fava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0d, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0d, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, boolean_value);

    align_serialize(expected_streams, encoding, endianness, align_value, boolean_value);
}

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
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        iava, iava, iava, fava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0d, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0d, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        b_value                 // Boolean
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, boolean_value);

    align_serialize(expected_streams, encoding, endianness, align_value, boolean_value);
}

TEST_P(XCdrBasicTypesTest, octet_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr uint8_t octet_value {0xCD};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        align_value,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0d, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0d, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, octet_value);

    align_serialize(expected_streams, encoding, endianness, align_value, octet_value);
}

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
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // DHEADER
        iava, fava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0d, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0d, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, octet_value);

    align_serialize(expected_streams, encoding, endianness, align_value, octet_value);
}

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
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        iava, iava, iava, fava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0d, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0d, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        octet_value             // Octet
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, octet_value);

    align_serialize(expected_streams, encoding, endianness, align_value, octet_value);
}

TEST_P(XCdrBasicTypesTest, char_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr char char_value {'}'};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value,             // Char
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
        char_value,             // Char
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        align_value,
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0d, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0d, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value              // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, char_value);

    align_serialize(expected_streams, encoding, endianness, align_value, char_value);
}

TEST_P(XCdrBasicTypesTest, char_align_2)
{
    const uint16_t align_value {0xABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr char char_value {'}'};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        char_value              // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value,            // Char
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
        char_value,            // Char
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // DHEADER
        iava, fava,
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0d, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0d, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value             // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, char_value);

    align_serialize(expected_streams, encoding, endianness, align_value, char_value);
}

TEST_P(XCdrBasicTypesTest, char_align_4)
{
    const uint32_t align_value {0xABABABBA};
    constexpr uint8_t iava {0xAB};
    constexpr uint8_t fava {0xBA};
    constexpr char char_value {'}'};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x01, // ShortMemberHeader
        char_value,            // Char
        0x00, 0x00, 0x00,       // Alignment
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x01, 0x00, // ShortMemberHeader
        char_value,            // Char
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        iava, iava, iava, fava,
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0d, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        char_value             // Char
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0d, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        char_value             // Char
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, char_value);

    align_serialize(expected_streams, encoding, endianness, align_value, char_value);
}

TEST_P(XCdrBasicTypesTest, wchar_align_1)
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
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval, // WChar
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // WChar
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // WChar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, wchar_value);

    align_serialize(expected_streams, encoding, endianness, align_value, wchar_value);
}

TEST_P(XCdrBasicTypesTest, wchar_align_2)
{
    const uint16_t align_value {0xABBA};
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
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval,  // WChar
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // WChar
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // WChar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, wchar_value);

    align_serialize(expected_streams, encoding, endianness, align_value, wchar_value);
}

TEST_P(XCdrBasicTypesTest, wchar_align_4)
{
    const uint32_t align_value {0xABABABBA};
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
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x04, // ShortMemberHeader
        ival, ival, ival, fval, // WChar
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x04, 0x00, // ShortMemberHeader
        fval, ival, ival, ival, // WChar
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x08, // DHEADER
        iava, iava, iava, fava,
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x08, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        fval, ival, ival, ival  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        ival, ival, ival, fval  // WChar
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fval, ival, ival, ival  // WChar
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, wchar_value);

    align_serialize(expected_streams, encoding, endianness, align_value, wchar_value);
}

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
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0b, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0b, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
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
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, string_value);

    align_serialize(expected_streams, encoding, endianness, align_value, string_value);
}

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
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0b, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0b, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
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
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, string_value);

    align_serialize(expected_streams, encoding, endianness, align_value, string_value);
}

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
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0b, // DHEADER
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x0b, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x13, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, string_value);

    align_serialize(expected_streams, encoding, endianness, align_value, string_value);
}

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
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x01, 0x00, 0x0c, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB, // WString
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x01, 0x00, // ShortMemberHeader
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x0c, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00, // WString
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x18, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x60, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x18, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x60, // EMHEADER1(M) with NEXTINT
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, wstring_value);

    align_serialize(expected_streams, encoding, endianness, align_value, wstring_value);
}

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
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // ShortMemberHeader
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x01, 0x00, 0x0c, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB, // WString
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x02, 0x00, // ShortMemberHeader
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x0c, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00, // WString
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        0x00, 0x00,             // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        0x00, 0x00,             // Alignment
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x18, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x60, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x18, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x60, // EMHEADER1(M) with NEXTINT
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, wstring_value);

    align_serialize(expected_streams, encoding, endianness, align_value, wstring_value);
}

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
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x04, // ShortMemberHeader
        iava, iava, iava, fava,
        0x00, 0x01, 0x00, 0x0c, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB, // WString
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x04, 0x00, // ShortMemberHeader
        fava, iava, iava, iava,
        0x01, 0x00, 0x0c, 0x00, // ShortMemberHeader
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00, // WString
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x18, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x60, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x02, // WString length
        0x00, 0x00, 0x00, valA, // WString
        0x00, 0x00, 0x00, valB  // WString
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x18, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x60, // EMHEADER1(M) with NEXTINT
        0x02, 0x00, 0x00, 0x00, // WString length
        valA, 0x00, 0x00, 0x00, // WString
        valB, 0x00, 0x00, 0x00  // WString
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, wstring_value);

    align_serialize(expected_streams, encoding, endianness, align_value, wstring_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, enum_value);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, enum_value);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x10, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x20, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x00, 0x00, 0x01  // Enum32
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x10, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        0x01, 0x00, 0x00, 0x00  // Enum32
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, enum_value);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value);
}

TEST_P(XCdrBasicTypesTest, enum_align_1)
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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0e, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0e, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x01, 0x00              // Enum16
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, enum_value);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value);
}

TEST_P(XCdrBasicTypesTest, enum_align_2)
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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0e, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0e, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x01, 0x00              // Enum16
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, enum_value);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value);
}

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
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        0x01, 0x00              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x06, // DHEADER
        iava, iava, iava, fava,
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x06, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        0x01, 0x00              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0e, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x10, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        0x00, 0x01              // Enum16
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0e, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        0x01, 0x00              // Enum16
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, enum_value);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value);
}

TEST_P(XCdrBasicTypesTest, enum8_align_1)
{
    const uint8_t align_value {0xAB};
    constexpr Enum8 enum_value {ENUM8_VAL_2};

    //{ Defining expected XCDR streams
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        align_value,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        align_value,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        align_value,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x02, // DHEADER
        align_value,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x02, 0x00, 0x00, 0x00, // DHEADER
        align_value,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0d, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0d, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        align_value,
        0x00, 0x00, 0x00,       // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        enum_value              // Enum8
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, enum_value);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value);
}

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
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, fava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, fava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x03, // DHEADER
        iava, fava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x03, 0x00, 0x00, 0x00, // DHEADER
        fava, iava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0d, // DHEADER
        0x10, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, fava,
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0d, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x10, // EMHEADER1(M) without NEXTINT
        fava, iava,
        0x00, 0x00,             // Alignment
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        enum_value              // Enum8
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, enum_value);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value);
}

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
        0x00, 0x00, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x00, // Encapsulation
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
        0x00, 0x06, 0x00, 0x00, // Encapsulation
        iava, iava, iava, fava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x00, // Encapsulation
        fava, iava, iava, iava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x05, // DHEADER
        iava, iava, iava, fava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x05, 0x00, 0x00, 0x00, // DHEADER
        fava, iava, iava, iava,
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x0d, // DHEADER
        0x20, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        iava, iava, iava, fava,
        0x00, 0x00, 0x00, 0x01, // EMHEADER1(M) without NEXTINT
        enum_value              // Enum8
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x0d, 0x00, 0x00, 0x00, // DHEADER
        0x00, 0x00, 0x00, 0x20, // EMHEADER1(M) without NEXTINT
        fava, iava, iava, iava,
        0x01, 0x00, 0x00, 0x00, // EMHEADER1(M) without NEXTINT
        enum_value              // Enum8
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    align_serialize_the_value(expected_streams, encoding, endianness, align_value, enum_value);

    align_serialize(expected_streams, encoding, endianness, align_value, enum_value);
}

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
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x2E, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0e, // NEXTINT + DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x50, 0x00, 0x00, 0x02, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0e, // NEXTINT + DHEADER
        0x10, 0x00, 0x00, 0x03, // EMHEADER1(M) without NEXTINT
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x10, 0x00, 0x3F, 0xFF, // EMHEADER1(M) without NEXTINT
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
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
        InnerBasicTypesShortStruct value { {ushort_value} };
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        cdr.serialize_member(MemberId(1), value);
        cdr.serialize_member(MemberId(2), value);
        cdr.end_serialize_type(enc_state);
        Cdr::state enc_state_end(cdr);
        //}

        //{ Test encoded content
        ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
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
        InnerBasicTypesShortStruct value { {ushort_value} };
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
        ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
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
        0x00, 0x08, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x1e, // DHEADER
        0x00, 0x00, 0x00, 0x0a, // DHEADER
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval,             // Short
        0x00, 0x00,             // Alignment
        0x00, 0x00, 0x00, 0x0a, // DHEADER
        0x00, 0x00, 0x00, 0x04, // DHEADER
        ival, fval,             // Short
        ival, fval,             // Short
        ival, fval              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x00, // Encapsulation
        0x1e, 0x00, 0x00, 0x00, // DHEADER
        0x0a, 0x00, 0x00, 0x00, // DHEADER
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival,             // Short
        0x00, 0x00,             // Alignment
        0x0a, 0x00, 0x00, 0x00, // DHEADER
        0x04, 0x00, 0x00, 0x00, // DHEADER
        fval, ival,             // Short
        fval, ival,             // Short
        fval, ival              // Short
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0a, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x4e, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x1e, // NEXTINT + DHEADER
        0x50, 0x00, 0x00, 0x00, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0e, // NEXTINT + DHEADER
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
        0x00, 0x00, 0x00, 0x0e, // NEXTINT + DHEADER
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
        0x00, 0x0b, 0x00, 0x00, // Encapsulation
        0x4e, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x1e, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
        0x00, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x0e, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
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
        0x0e, 0x00, 0x00, 0x00, // NEXTINT + DHEADER
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
        InnerBasicTypesStruct value { {ushort_value} };
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        cdr.serialize_member(MemberId(1), value);
        cdr.serialize_member(MemberId(2), value);
        cdr.end_serialize_type(enc_state);
        Cdr::state enc_state_end(cdr);
        //}

        //{ Test encoded content
        ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
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
        InnerBasicTypesStruct value { {ushort_value} };
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
        ASSERT_EQ(cdr.getSerializedDataLength(), expected_streams[tested_stream].size());
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

