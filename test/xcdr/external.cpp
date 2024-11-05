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

#include <string>

#include <gtest/gtest.h>

#include <fastcdr/Cdr.h>
#include <fastcdr/CdrSizeCalculator.hpp>
#include <fastcdr/xcdr/external.hpp>
#include <fastcdr/xcdr/optional.hpp>

#include "utility.hpp"

/// Auxiliary class
class ExternalClass
{
public:

    ExternalClass() noexcept = default;

    ExternalClass(
            const std::string text) noexcept
        : text_ {text}
    {
    }

    bool operator ==(
            const ExternalClass& other) const noexcept
    {
        return other.text_ == text_;
    }

    void text(
            const std::string& text) noexcept
    {
        text_ = text;
    }

    std::string& text() noexcept
    {
        return text_;
    }

    const std::string& text() const noexcept
    {
        return text_;
    }

private:

    std::string text_;
};

namespace eprosima {
namespace fastcdr {

template<>
size_t calculate_serialized_size(
        eprosima::fastcdr::CdrSizeCalculator& calculator,
        const ExternalClass& data,
        size_t& current_alignment)
{
    eprosima::fastcdr::EncodingAlgorithmFlag previous_encoding = calculator.get_encoding();
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(previous_encoding, current_alignment)};


    calculated_size += calculator.calculate_member_serialized_size(eprosima::fastcdr::MemberId(
                        1), data.text(), current_alignment);

    calculated_size += calculator.end_calculate_type_serialized_size(previous_encoding, current_alignment);

    return calculated_size;
}

template<>
void serialize(
        Cdr& cdr,
        const ExternalClass& data)

{
    Cdr::state current_status(cdr);
    cdr.begin_serialize_type(current_status, cdr.get_encoding_flag());
    cdr << MemberId(1) << data.text();
    cdr.end_serialize_type(current_status);
}

template<>
void deserialize(
        Cdr& cdr,
        ExternalClass& data)
{
    cdr.deserialize_type(cdr.get_encoding_flag(), [&data](Cdr& cdr_inner, const MemberId& mid) -> bool
            {
                bool ret_value = true;
                if (EncodingAlgorithmFlag::PL_CDR == cdr_inner.get_encoding_flag() ||
                EncodingAlgorithmFlag::PL_CDR2 == cdr_inner.get_encoding_flag())
                {
                    switch (mid.id)
                    {
                        case 1:
                            cdr_inner >> data.text();
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
                            cdr_inner >> data.text();
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

using namespace eprosima::fastcdr;

//! @test Tests the default constructor.
TEST(XCdrExternalAPITest, default_constructor)
{
    external<ExternalClass> ext;

    ASSERT_FALSE(ext);
    ASSERT_TRUE(nullptr == ext.get());
    ASSERT_FALSE(ext.is_locked());
}

//! @test Tests the constructor from a pointer.
TEST(XCdrExternalAPITest, constructor_from_pointer)
{
    // Calling constructor with locked = false
    ExternalClass* _ext {new ExternalClass("test")};
    external<ExternalClass> ext {_ext};

    ASSERT_TRUE(ext);
    ASSERT_TRUE(nullptr != ext.get());
    ASSERT_EQ((*ext).text(), "test");
    _ext->text("test2");
    ASSERT_EQ((*ext).text(), "test2");
    ASSERT_FALSE(ext.is_locked());

    // Calling constructor with locked = true
    ExternalClass* _ext2 {new ExternalClass("test")};
    external<ExternalClass> lock_ext {_ext2, true};

    ASSERT_TRUE(lock_ext);
    ASSERT_TRUE(nullptr != lock_ext.get());
    ASSERT_EQ((*lock_ext).text(), "test");
    _ext2->text("test2");
    ASSERT_EQ((*lock_ext).text(), "test2");
    ASSERT_TRUE(lock_ext.is_locked());
}

//! @test Tests the constructor from a shared pointer.
TEST(XCdrExternalAPITest, constructor_from_shared_pointer)
{
    // Calling constructor with a shared pointer managing an object.
    std::shared_ptr<ExternalClass> _ext {std::make_shared<ExternalClass>("test")};
    external<ExternalClass> ext {_ext};

    ASSERT_TRUE(ext);
    ASSERT_TRUE(nullptr != ext.get());
    ASSERT_EQ((*ext).text(), "test");
    _ext.get()->text("test2");
    ASSERT_EQ((*ext).text(), "test2");
    ASSERT_FALSE(ext.is_locked());

    // Calling constructor with a empty shared pointer.
    std::shared_ptr<ExternalClass> _empty_ext;
    external<ExternalClass> empty_ext {_empty_ext};

    ASSERT_FALSE(empty_ext);
    ASSERT_TRUE(nullptr == empty_ext.get());
    ASSERT_FALSE(empty_ext.is_locked());
}

//! @test Tests the copy constructor.
TEST(XCdrExternalAPITest, copy_contructor)
{
    // Calling constructor with empty external.
    external<ExternalClass> _empty_ext;
    external<ExternalClass> empty_ext {_empty_ext};

    ASSERT_FALSE(empty_ext);
    ASSERT_TRUE(nullptr == empty_ext.get());
    ASSERT_FALSE(empty_ext.is_locked());

    // Calling constructor with no-locked external.
    ExternalClass* __ext {new ExternalClass("test")};
    external<ExternalClass> _ext {__ext};
    external<ExternalClass> ext {_ext};

    ASSERT_TRUE(ext);
    ASSERT_TRUE(nullptr != ext.get());
    ASSERT_EQ((*ext).text(), "test");
    _ext->text("test2");
    ASSERT_EQ((*ext).text(), "test2");
    ASSERT_FALSE(ext.is_locked());

    // Calling constructor with locked = true
    ExternalClass* __lock_ext {new ExternalClass("test")};
    external<ExternalClass> _lock_ext {__lock_ext, true};
    external<ExternalClass> lock_ext {_lock_ext};

    ASSERT_TRUE(lock_ext);
    ASSERT_TRUE(nullptr != lock_ext.get());
    ASSERT_EQ((*lock_ext).text(), "test");
    _lock_ext->text("test2");
    ASSERT_EQ((*lock_ext).text(), "test");
    ASSERT_TRUE(lock_ext.is_locked());
}


//! @test Tests the assignment.
TEST(XCdrExternalAPITest, assignment)
{
    // Calling assignment over an empty external with empty external.
    external<ExternalClass> _empty_ext;
    external<ExternalClass> empty_ext;
    empty_ext = _empty_ext;

    ASSERT_FALSE(empty_ext);
    ASSERT_TRUE(nullptr == empty_ext.get());
    ASSERT_FALSE(empty_ext.is_locked());

    // Calling assignment over an external with empty external.
    external<ExternalClass> not_empty_ext {new ExternalClass("test")};
    not_empty_ext = _empty_ext;

    ASSERT_FALSE(not_empty_ext);
    ASSERT_TRUE(nullptr == not_empty_ext.get());
    ASSERT_FALSE(not_empty_ext.is_locked());

    // Calling assignment over an empty external with an external;
    ExternalClass* __ext {new ExternalClass("test")};
    external<ExternalClass> _ext {__ext};
    external<ExternalClass> empty_ext2;
    empty_ext2 = _ext;

    ASSERT_TRUE(empty_ext2);
    ASSERT_TRUE(nullptr != empty_ext2.get());
    ASSERT_EQ((*empty_ext2).text(), "test");
    _ext->text("test2");
    ASSERT_EQ((*empty_ext2).text(), "test2");
    ASSERT_FALSE(empty_ext2.is_locked());

    // Calling assignment over an external with an external;
    external<ExternalClass> not_empty_ext2 {new ExternalClass("test")};
    not_empty_ext2 = _ext;

    ASSERT_TRUE(not_empty_ext2);
    ASSERT_TRUE(nullptr != not_empty_ext2.get());
    ASSERT_EQ((*not_empty_ext2).text(), "test2");
    _ext->text("test3");
    ASSERT_EQ((*not_empty_ext2).text(), "test3");
    ASSERT_FALSE(not_empty_ext2.is_locked());

    // Calling assignment over an empty external with a locked external;
    ExternalClass* __locked_ext {new ExternalClass("test")};
    external<ExternalClass> _locked_ext {__locked_ext, true};
    external<ExternalClass> empty_ext3;
    empty_ext3 = _locked_ext;

    ASSERT_TRUE(empty_ext3);
    ASSERT_TRUE(nullptr != empty_ext3.get());
    ASSERT_EQ((*empty_ext3).text(), "test");
    _locked_ext->text("test2");
    ASSERT_EQ((*empty_ext3).text(), "test");
    ASSERT_FALSE(empty_ext3.is_locked());

    // Calling assignment over an external with a locked external;
    external<ExternalClass> not_empty_ext3 {new ExternalClass("test")};
    not_empty_ext3 = _locked_ext;

    ASSERT_TRUE(not_empty_ext3);
    ASSERT_TRUE(nullptr != not_empty_ext3.get());
    ASSERT_EQ((*not_empty_ext3).text(), "test2");
    _locked_ext->text("test3");
    ASSERT_EQ((*not_empty_ext3).text(), "test2");
    ASSERT_FALSE(not_empty_ext3.is_locked());

    // Calling assignment over a locked external.
    external<ExternalClass> locked_ext {new ExternalClass("test"), true};
    EXPECT_THROW(locked_ext = _locked_ext, exception::LockedExternalAccessException);
}

//! @test Tests the dereference object functions.
TEST(XCdrExternalAPITest, dereference_object)
{
    external<ExternalClass> ext {new ExternalClass("test")};

    ASSERT_EQ((*ext).text(), "test");
    (*ext).text("test2");
    ASSERT_EQ((*ext).text(), "test2");

    const external<ExternalClass>& ext_ref = ext;
    ASSERT_EQ((*ext_ref).text(), "test2");
}

//! @test Tests the functions to get the pointer.
TEST(XCdrExternalAPITest, get_pointer)
{
    external<ExternalClass> ext {new ExternalClass("test")};

    ASSERT_EQ(ext.get()->text(), "test");
    ext.get()->text("test2");
    ASSERT_EQ(ext.get()->text(), "test2");

    const external<ExternalClass>& ext_ref = ext;
    ASSERT_EQ(ext_ref.get()->text(), "test2");
}

//! @test Tests the function to get the shared pointer.
TEST(XCdrExternalAPITest, get_shared_pointer)
{
    external<ExternalClass> ext {new ExternalClass("test")};

    ASSERT_EQ(ext.get_shared_ptr()->text(), "test");
    ext.get_shared_ptr()->text("test2");
    ASSERT_EQ(ext.get_shared_ptr()->text(), "test2");
}

//! @test Tests the dereference object member functions.
TEST(XCdrExternalAPITest, dereference_member)
{
    external<ExternalClass> ext {new ExternalClass("test")};

    ASSERT_EQ(ext->text(), "test");
    ext->text("test2");
    ASSERT_EQ(ext->text(), "test2");

    const external<ExternalClass>& ext_ref = ext;
    ASSERT_EQ(ext_ref->text(), "test2");
}

//! @test Tests the comparison functions.
TEST(XCdrExternalAPITest, compare)
{
    external<ExternalClass> empty_ext;
    external<ExternalClass> ext {new ExternalClass("test")};
    external<ExternalClass> ext2 {new ExternalClass("test")};

    ASSERT_FALSE(ext == empty_ext);
    ASSERT_TRUE(ext != empty_ext);

    ASSERT_FALSE(ext == ext2);
    ASSERT_TRUE(ext != ext2);

    ext = ext2;

    ASSERT_TRUE(ext == ext2);
    ASSERT_FALSE(ext != ext2);

    ext = empty_ext;

    ASSERT_TRUE(ext == empty_ext);
    ASSERT_FALSE(ext != empty_ext);

    ASSERT_FALSE(ext == ext2);
    ASSERT_TRUE(ext != ext2);

}

//! @test Tests function to check it is null.
TEST(XCdrExternalAPITest, check_null)
{
    external<ExternalClass> empty_ext;
    ASSERT_FALSE(empty_ext);
    ASSERT_TRUE(!empty_ext);
    external<ExternalClass> ext {new ExternalClass("test")};
    ASSERT_TRUE(ext);
    ASSERT_FALSE(!ext);
}


//! @test Tests the function to check it is locked.
TEST(XCdrExternalAPITest, is_locked)
{
    external<ExternalClass> empty_ext;
    ASSERT_FALSE(empty_ext.is_locked());

    external<ExternalClass> ext {new ExternalClass("test")};
    ASSERT_FALSE(ext.is_locked());

    external<ExternalClass> locked_ext {new ExternalClass("test"), true};
    ASSERT_TRUE(locked_ext.is_locked());
}

//! @test Tests the function to lock.
TEST(XCdrExternalAPITest, lock)
{
    external<ExternalClass> empty_ext;
    ASSERT_FALSE(empty_ext.is_locked());
    empty_ext.lock();
    ASSERT_TRUE(empty_ext.is_locked());

    external<ExternalClass> ext {new ExternalClass("test")};
    ASSERT_FALSE(ext.is_locked());
    ext.lock();
    ASSERT_TRUE(ext.is_locked());

    external<ExternalClass> locked_ext {new ExternalClass("test"), true};
    ASSERT_TRUE(locked_ext.is_locked());
    locked_ext.lock();
    ASSERT_TRUE(locked_ext.is_locked());
}

using XCdrStreamValues =
        std::array<std::vector<uint8_t>,
                1 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS>;


class XCdrExternalTest : public ::testing::TestWithParam< std::tuple<EncodingAlgorithmFlag, Cdr::Endianness>>
{
};

template<class _T>
void serialize_external(
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        const external<_T>& ext_value,
        external<_T>& ret_ext_value,
        bool low_api)
{
    //{ Defining expected XCDR streams
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};
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
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x01, 0x00, 0x07, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x01, 0x00, 0x07, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
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
        0x00, 0x00, 0x00, 0x0B, // DHEADER
        0x00, 0x00, 0x00, 0x07, // DHEADER
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x0B, 0x00, 0x00, 0x00, // DHEADER
        0x07, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x13, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0B, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x01, // Encapsulation
        0x13, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x0B, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    //}

    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding));
    size_t current_alignment {0};
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(encoding, current_alignment)};
    calculated_size += calculator.calculate_member_serialized_size(MemberId(1), ext_value, current_alignment);
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

    //{ Encode optional not present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    if (low_api)
    {
        cdr.serialize_member(MemberId(1), ext_value);
    }
    else
    {
        cdr << MemberId(1) << ext_value;
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

    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId&)->bool
            {
                bool ret_value = true;

                if (low_api)
                {
                    cdr_inner.deserialize_member(ret_ext_value);
                }
                else
                {
                    cdr_inner >> ret_ext_value;
                }

                if (EncodingAlgorithmFlag::PL_CDR != encoding && EncodingAlgorithmFlag::PL_CDR2 != encoding)
                {
                    ret_value = false;
                }

                return ret_value;
            });
    ASSERT_TRUE(ret_ext_value);
    ASSERT_EQ(*ext_value, *ret_ext_value);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

template<class _T>
void serialize_optional_external(
        EncodingAlgorithmFlag encoding,
        Cdr::Endianness endianness,
        const optional<external<_T>>& ext_value,
        optional<external<_T>>& ret_ext_value,
        bool low_api)
{
    //{ Defining expected XCDR streams
    constexpr uint8_t valA {65};
    constexpr uint8_t valB {66};
    XCdrStreamValues expected_streams;
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x00, 0x00, 0x01, // Encapsulation
        0x00, 0x01, 0x00, 0x07, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x01, 0x00, 0x01, // Encapsulation
        0x01, 0x00, 0x07, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x02, 0x00, 0x00, // Encapsulation
        0x00, 0x01, 0x00, 0x10, // ShortMemberHeader
        0x00, 0x01, 0x00, 0x07, // ShortMemberHeader
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x3F, 0x02, 0x00, 0x00, // Sentinel
        0x3F, 0x02, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x01, 0x00, 0x10, 0x00, // ShortMemberHeader
        0x01, 0x00, 0x07, 0x00, // ShortMemberHeader
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00,       // String
        0x00,                   // Alignment
        0x02, 0x3F, 0x00, 0x00, // Sentinel
        0x02, 0x3F, 0x00, 0x00  // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x01, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x01, // Encapsulation
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x0F, // DHEADER
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x00, 0x00, 0x00, 0x07, // DHEADER
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x01, // Encapsulation
        0x0F, 0x00, 0x00, 0x00, // DHEADER
        0x01,                   // Present
        0x00, 0x00, 0x00,       // Alignment
        0x07, 0x00, 0x00, 0x00, // DHEADER
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x01, // Encapsulation
        0x00, 0x00, 0x00, 0x13, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x0B, // DHEADER
        0x50, 0x00, 0x00, 0x01, // EMHEADER1(M) with NEXTINT
        0x00, 0x00, 0x00, 0x03, // String length
        valA, valB, 0x00        // String
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x01, // Encapsulation
        0x13, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x0B, 0x00, 0x00, 0x00, // DHEADER
        0x01, 0x00, 0x00, 0x50, // EMHEADER1(M) with NEXTINT
        0x03, 0x00, 0x00, 0x00, // String length
        valA, valB, 0x00        // String
    };
    //}

    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding));
    size_t current_alignment {0};
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(encoding, current_alignment)};
    calculated_size += calculator.calculate_member_serialized_size(MemberId(1), ext_value, current_alignment);
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

    //{ Encode optional present.
    cdr.set_encoding_flag(encoding);
    cdr.serialize_encapsulation();
    Cdr::state enc_state(cdr);
    cdr.begin_serialize_type(enc_state, encoding);
    if (low_api)
    {
        cdr.serialize_member(MemberId(1), ext_value);
    }
    else
    {
        cdr << MemberId(1) << ext_value;
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

    cdr.reset();
    cdr.read_encapsulation();
    ASSERT_EQ(cdr.get_encoding_flag(), encoding);
    ASSERT_EQ(cdr.endianness(), endianness);
    cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId&)->bool
            {
                bool ret_value = true;

                if (low_api)
                {
                    cdr_inner.deserialize_member(ret_ext_value);
                }
                else
                {
                    cdr_inner >> ret_ext_value;
                }

                if (EncodingAlgorithmFlag::PL_CDR != encoding && EncodingAlgorithmFlag::PL_CDR2 != encoding)
                {
                    ret_value = false;
                }

                return ret_value;
            });
    ASSERT_TRUE(ret_ext_value.has_value());
    ASSERT_TRUE(ret_ext_value.value());
    ASSERT_EQ(**ext_value, **ret_ext_value);
    Cdr::state dec_state_end(cdr);
    ASSERT_EQ(enc_state_end, dec_state_end);
    //}
}

/*!
 * @test Test encoding of an empty external field of ExternalClass type
 * @code{.idl}
 * struct ExternalClass
 * {
 *     string var_str;
 * };
 *
 * struct NullExternal
 * {
 *     @external
 *     ExternalClass var_ext;
 * };
 * @endcode
 */
TEST_P(XCdrExternalTest, null_external)
{
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    external<ExternalClass> ext_value;

    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding));
    size_t current_alignment {0};
    calculator.begin_calculate_type_serialized_size(encoding, current_alignment);
    EXPECT_THROW(calculator.calculate_member_serialized_size(MemberId(
                1), ext_value, current_alignment), exception::BadParamException);

    {

        //{ Prepare buffer
        auto buffer =
                std::unique_ptr<char, void (*)(
            void*)>{reinterpret_cast<char*>(calloc(20, sizeof(char))), free};
        FastBuffer fast_buffer(buffer.get(), 20);
        Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
        //}

        //{ Encode null external.
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        EXPECT_THROW(cdr.serialize_member(MemberId(1), ext_value), exception::BadParamException);
        //}
    }

    {
        //{ Prepare buffer
        auto buffer =
                std::unique_ptr<char, void (*)(
            void*)>{reinterpret_cast<char*>(calloc(20, sizeof(char))), free};
        FastBuffer fast_buffer(buffer.get(), 20);
        Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
        //}

        //{ Encode null external.
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        EXPECT_THROW(cdr << MemberId(1) << ext_value, exception::BadParamException);
        //}
    }

}

/*!
 * @test Test encoding of an external field of ExternalClass type and deserialize on empty external
 * @code{.idl}
 * struct ExternalClass
 * {
 *     string var_str;
 * };
 *
 * struct ExternalToEmptyExternal
 * {
 *     @external
 *     ExternalClass var_ext;
 * };
 * @endcode
 */
TEST_P(XCdrExternalTest, external_to_empty_external)
{
    external<ExternalClass> ext_value {new ExternalClass("AB")};
    external<ExternalClass> ret_value;

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_external(encoding, endianness, ext_value, ret_value, true);

    serialize_external(encoding, endianness, ext_value, ret_value, false);
}

/*!
 * @test Test encoding of an external field of ExternalClass type and deserialize on other external
 * @code{.idl}
 * struct ExternalClass
 * {
 *     string var_str;
 * };
 *
 * struct ExternalToOtherExternal
 * {
 *     @external
 *     ExternalClass var_ext;
 * };
 * @endcode
 */
TEST_P(XCdrExternalTest, external_to_other_external)
{
    external<ExternalClass> ext_value {new ExternalClass("AB")};
    external<ExternalClass> ret_value {new ExternalClass("CD")};

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_external(encoding, endianness, ext_value, ret_value, true);

    serialize_external(encoding, endianness, ext_value, ret_value, false);
}

/*!
 * @test Test encoding of a locked external field of ExternalClass type and deserialize on other external
 * @code{.idl}
 * struct ExternalClass
 * {
 *     string var_str;
 * };
 *
 * struct LockedExternalToExternal
 * {
 *     @external
 *     ExternalClass var_ext;
 * };
 * @endcode
 */
TEST_P(XCdrExternalTest, locked_external_to_external)
{
    external<ExternalClass> ext_value {new ExternalClass("AB"), true};
    external<ExternalClass> ret_value {new ExternalClass("CD")};

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_external(encoding, endianness, ext_value, ret_value, true);

    serialize_external(encoding, endianness, ext_value, ret_value, false);
}

/*!
 * @test Test encoding of an external field of ExternalClass type and deserialize on locked external
 * @code{.idl}
 * struct ExternalClass
 * {
 *     string var_str;
 * };
 *
 * struct ExternalToLockedExternal
 * {
 *     @external
 *     ExternalClass var_ext;
 * };
 * @endcode
 */
TEST_P(XCdrExternalTest, external_to_locked_external)
{
    external<ExternalClass> ext_value {new ExternalClass("AB")};
    external<ExternalClass> ret_value {new ExternalClass("CD"), true};

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    EXPECT_THROW(serialize_external(encoding, endianness, ext_value, ret_value, true), exception::BadParamException);

    EXPECT_THROW(serialize_external(encoding, endianness, ext_value, ret_value, false), exception::BadParamException);
}

/*!
 * @test Test encoding of an empty optional field of an external.
 * @code{.idl}
 * struct ExternalClass
 * {
 *     string var_str;
 * };
 *
 * struct NullOptional
 * {
 *     @optional @external
 *     ExternalClass var_ext;
 * };
 * @endcode
 */
TEST_P(XCdrExternalTest, null_optional)
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
        0x3F, 0x02, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x03, 0x00, 0x00, // Encapsulation
        0x02, 0x3F, 0x00, 0x00, // Sentinel
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x06, 0x00, 0x03, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PLAIN_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x07, 0x00, 0x03, // Encapsulation
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x08, 0x00, 0x03, // Encapsulation
        0x00, 0x00, 0x00, 0x01, // DHEADER
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::DELIMIT_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x09, 0x00, 0x03, // Encapsulation
        0x01, 0x00, 0x00, 0x00, // DHEADER
        0x00                    // Not present
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::BIG_ENDIANNESS] =
    {
        0x00, 0x0A, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x00  // DHEADER
    };
    expected_streams[0 + EncodingAlgorithmFlag::PL_CDR2 + Cdr::Endianness::LITTLE_ENDIANNESS] =
    {
        0x00, 0x0B, 0x00, 0x00, // Encapsulation
        0x00, 0x00, 0x00, 0x00  // DHEADER
    };
    //}

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    optional<external<ExternalClass>> opt_value;

    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding));
    size_t current_alignment {0};
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(encoding, current_alignment)};
    calculated_size += calculator.calculate_member_serialized_size(MemberId(1), opt_value, current_alignment);
    calculated_size += calculator.end_calculate_type_serialized_size(encoding, current_alignment);
    calculated_size += 4;     // Encapsulation
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

        //{ Encode optional not present.
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        cdr.serialize_member(MemberId(1), opt_value);
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

        //{ Decoding optional not present
        optional<external<ExternalClass>> dopt_value;
        cdr.reset();
        cdr.read_encapsulation();
        ASSERT_EQ(cdr.get_encoding_flag(), encoding);
        ASSERT_EQ(cdr.endianness(), endianness);
        cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId&)->bool
                {
                    bool ret_value = true;

                    cdr_inner.deserialize_member(dopt_value);

                    if (EncodingAlgorithmFlag::PL_CDR != encoding && EncodingAlgorithmFlag::PL_CDR2 != encoding)
                    {
                        ret_value = false;
                    }

                    return ret_value;
                });
        ASSERT_FALSE(dopt_value.has_value());
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

        //{ Encode optional not present.
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        cdr << MemberId(1) << opt_value;
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

        //{ Decoding optional not present
        optional<external<ExternalClass>> dopt_value;
        cdr.reset();
        cdr.read_encapsulation();
        ASSERT_EQ(cdr.get_encoding_flag(), encoding);
        ASSERT_EQ(cdr.endianness(), endianness);
        cdr.deserialize_type(encoding, [&](Cdr& cdr_inner, const MemberId&)->bool
                {
                    bool ret_value = true;

                    cdr_inner >> dopt_value;

                    if (EncodingAlgorithmFlag::PL_CDR != encoding && EncodingAlgorithmFlag::PL_CDR2 != encoding)
                    {
                        ret_value = false;
                    }

                    return ret_value;
                });
        ASSERT_FALSE(dopt_value.has_value());
        //}
    }

}

/*!
 * @test Test encoding of an optional with an empty external field of ExternalClass type
 * @code{.idl}
 * struct ExternalClass
 * {
 *     string var_str;
 * };
 *
 * struct NullExternal
 * {
 *     @optional @external
 *     ExternalClass var_ext;
 * };
 * @endcode
 */
TEST_P(XCdrExternalTest, optional_null_external)
{
    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());
    optional<external<ExternalClass>> ext_value {{}};

    //{ Calculate encoded size.
    CdrSizeCalculator calculator(get_version_from_algorithm(encoding));
    size_t current_alignment {0};
    calculator.begin_calculate_type_serialized_size(encoding, current_alignment);
    EXPECT_THROW(calculator.calculate_member_serialized_size(MemberId(
                1), ext_value, current_alignment), exception::BadParamException);

    {

        //{ Prepare buffer
        auto buffer =
                std::unique_ptr<char, void (*)(
            void*)>{reinterpret_cast<char*>(calloc(20, sizeof(char))), free};
        FastBuffer fast_buffer(buffer.get(), 20);
        Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
        //}

        //{ Encode null external.
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        EXPECT_THROW(cdr.serialize_member(MemberId(1), ext_value), exception::BadParamException);
        //}
    }

    {
        //{ Prepare buffer
        auto buffer =
                std::unique_ptr<char, void (*)(
            void*)>{reinterpret_cast<char*>(calloc(20, sizeof(char))), free};
        FastBuffer fast_buffer(buffer.get(), 20);
        Cdr cdr(fast_buffer, endianness, get_version_from_algorithm(encoding));
        //}

        //{ Encode optional not present.
        cdr.set_encoding_flag(encoding);
        cdr.serialize_encapsulation();
        Cdr::state enc_state(cdr);
        cdr.begin_serialize_type(enc_state, encoding);
        EXPECT_THROW(cdr << MemberId(1) << ext_value, exception::BadParamException);
        //}
    }

}

/*!
 * @test Test encoding of an optional with an external field of ExternalClass type and deserialize on null optional.
 * @code{.idl}
 * struct ExternalClass
 * {
 *     string var_str;
 * };
 *
 * struct OptionalExternalToNullOptional
 * {
 *     @optional @external
 *     ExternalClass var_ext;
 * };
 * @endcode
 */
TEST_P(XCdrExternalTest, optional_external_to_null_optional)
{
    optional<external<ExternalClass>> ext_value {{new ExternalClass("AB")}};
    optional<external<ExternalClass>> ret_value;

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional_external(encoding, endianness, ext_value, ret_value, true);

    serialize_optional_external(encoding, endianness, ext_value, ret_value, false);
}

/*!
 * @test Test encoding of an optional with an external field of ExternalClass type and deserialize on optional with
 * empty external
 * @code{.idl}
 * struct ExternalClass
 * {
 *     string var_str;
 * };
 *
 * struct OptionalExternalToEmptyExternal
 * {
 *     @optional @external
 *     ExternalClass var_ext;
 * };
 * @endcode
 */
TEST_P(XCdrExternalTest, optional_external_to_empty_external)
{
    optional<external<ExternalClass>> ext_value {{new ExternalClass("AB")}};
    optional<external<ExternalClass>> ret_value {{}};

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional_external(encoding, endianness, ext_value, ret_value, true);

    serialize_optional_external(encoding, endianness, ext_value, ret_value, false);
}

/*!
 * @test Test encoding of an optional with an external field of ExternalClass type and deserialize on an optional with
 * other external
 * @code{.idl}
 * struct ExternalClass
 * {
 *     string var_str;
 * };
 *
 * struct OptionalExternalToOtherExternal
 * {
 *     @optional @external
 *     ExternalClass var_ext;
 * };
 * @endcode
 */
TEST_P(XCdrExternalTest, optional_external_to_other_external)
{
    optional<external<ExternalClass>> ext_value {{new ExternalClass("AB")}};
    optional<external<ExternalClass>> ret_value {{new ExternalClass("CD")}};

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional_external(encoding, endianness, ext_value, ret_value, true);

    serialize_optional_external(encoding, endianness, ext_value, ret_value, false);
}

/*!
 * @test Test encoding of an optional with a locked external field of ExternalClass type and deserialize on an optional
 * with other external
 * @code{.idl}
 * struct ExternalClass
 * {
 *     string var_str;
 * };
 *
 * struct OptionalLockedExternalToExternal
 * {
 *     @optional @external
 *     ExternalClass var_ext;
 * };
 * @endcode
 */
TEST_P(XCdrExternalTest, optional_locked_external_to_external)
{
    optional<external<ExternalClass>> ext_value {{new ExternalClass("AB"), true}};
    optional<external<ExternalClass>> ret_value {{new ExternalClass("CD")}};

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    serialize_optional_external(encoding, endianness, ext_value, ret_value, true);

    serialize_optional_external(encoding, endianness, ext_value, ret_value, false);
}

/*!
 * @test Test encoding of an optional with a external field of ExternalClass type and deserialize on an optional with
 * locked external
 * @code{.idl}
 * struct ExternalClass
 * {
 *     string var_str;
 * };
 *
 * struct OptionalExternalToLockedExternal
 * {
 *     @optional @external
 *     ExternalClass var_ext;
 * };
 * @endcode
 */
TEST_P(XCdrExternalTest, optional_external_to_locked_external)
{
    optional<external<ExternalClass>> ext_value {{new ExternalClass("AB")}};
    optional<external<ExternalClass>> ret_value {{new ExternalClass("CD"), true}};

    EncodingAlgorithmFlag encoding = std::get<0>(GetParam());
    Cdr::Endianness endianness = std::get<1>(GetParam());

    EXPECT_THROW(serialize_optional_external(encoding, endianness, ext_value, ret_value,
            true), exception::BadParamException);

    EXPECT_THROW(serialize_optional_external(encoding, endianness, ext_value, ret_value,
            false), exception::BadParamException);
}


INSTANTIATE_TEST_SUITE_P(
    XCdrTest,
    XCdrExternalTest,
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
