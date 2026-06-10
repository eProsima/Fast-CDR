// Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <map>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <fastcdr/Cdr.h>
#include <fastcdr/CdrEncoding.hpp>
#include <fastcdr/FastBuffer.h>
#include <fastcdr/cdr/fixed_size_string.hpp>
#include <fastcdr/exceptions/BadParamException.h>

using namespace eprosima::fastcdr;
using eprosima::fastcdr::exception::BadParamException;

namespace {

constexpr size_t TEST_BUFFER_LENGTH = 1024;

template<typename SerializedT, typename Configure>
void serialize_into(
        char* buffer,
        size_t buffer_size,
        const SerializedT& value,
        Configure configure)
{
    FastBuffer fast_buffer(buffer, buffer_size);
    Cdr cdr_ser(fast_buffer);
    configure(cdr_ser);
    cdr_ser << value;
}

template<typename SerializedT>
void serialize_into(
        char* buffer,
        size_t buffer_size,
        const SerializedT& value)
{
    serialize_into(buffer, buffer_size, value, [](Cdr&)
            {
            });
}

} // namespace


// =====================================================================================
// std::string
// =====================================================================================

TEST(XCdrTypeConsistency, StdString_Fit)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::string output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

TEST(XCdrTypeConsistency, StdString_Fail)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::string output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 5), BadParamException);
}

TEST(XCdrTypeConsistency, StdString_DefaultValue)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::string output {"prefilled"};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::DEFAULT_VALUE, 5));
    EXPECT_TRUE(output.empty());
}

TEST(XCdrTypeConsistency, StdString_Trim)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::string output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::TRIM, 5));
    EXPECT_EQ(output, std::string("hello"));
}


// =====================================================================================
// std::wstring
// =====================================================================================

TEST(XCdrTypeConsistency, StdWstring_Fit)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::wstring output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

TEST(XCdrTypeConsistency, StdWstring_Fail)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello world"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::wstring output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 5), BadParamException);
}

TEST(XCdrTypeConsistency, StdWstring_DefaultValue)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello world"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::wstring output {L"prefilled"};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::DEFAULT_VALUE, 5));
    EXPECT_TRUE(output.empty());
}

TEST(XCdrTypeConsistency, StdWstring_Trim)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello world"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::wstring output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::TRIM, 5));
    EXPECT_EQ(output, std::wstring(L"hello"));
}


// =====================================================================================
// char*&
// =====================================================================================

TEST(XCdrTypeConsistency, CharPtr_Fit)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    char* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 10));
    ASSERT_NE(output, nullptr);
    EXPECT_STREQ(output, "hello");
    free(output);
}

TEST(XCdrTypeConsistency, CharPtr_Fail)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    char* output {nullptr};
    EXPECT_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 5), BadParamException);
    EXPECT_EQ(output, nullptr);
}

TEST(XCdrTypeConsistency, CharPtr_DefaultValue)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    char* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::DEFAULT_VALUE, 5));
    EXPECT_EQ(output, nullptr);
}

TEST(XCdrTypeConsistency, CharPtr_Trim)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hello world"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    char* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::TRIM, 5));
    ASSERT_NE(output, nullptr);
    EXPECT_STREQ(output, "hello");
    free(output);
}


// =====================================================================================
// wchar_t*&
// =====================================================================================

TEST(XCdrTypeConsistency, WcharPtr_Fit)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    wchar_t* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 10));
    ASSERT_NE(output, nullptr);
    EXPECT_EQ(std::wcscmp(output, L"hello"), 0);
    free(output);
}

TEST(XCdrTypeConsistency, WcharPtr_Fail)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello world"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    wchar_t* output {nullptr};
    EXPECT_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 5), BadParamException);
    EXPECT_EQ(output, nullptr);
}

TEST(XCdrTypeConsistency, WcharPtr_DefaultValue)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello world"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    wchar_t* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::DEFAULT_VALUE, 5));
    EXPECT_EQ(output, nullptr);
}

TEST(XCdrTypeConsistency, WcharPtr_Trim)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::wstring input {L"hello world"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    wchar_t* output {nullptr};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::TRIM, 5));
    ASSERT_NE(output, nullptr);
    EXPECT_EQ(std::wcscmp(output, L"hello"), 0);
    free(output);
}


// =====================================================================================
// fixed_string<MAX>
// =====================================================================================

TEST(XCdrTypeConsistency, FixedString_Fit)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"hi"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    fixed_string<8> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL));
    EXPECT_STREQ(output.c_str(), "hi");
}

TEST(XCdrTypeConsistency, FixedString_Fail)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"too long for the fixed string"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    fixed_string<8> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL), BadParamException);
}

TEST(XCdrTypeConsistency, FixedString_DefaultValue)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"too long for the fixed string"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    fixed_string<8> output {"prefill"};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::DEFAULT_VALUE));
    EXPECT_STREQ(output.c_str(), "");
}

TEST(XCdrTypeConsistency, FixedString_Trim)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::string input {"abcdefghijkl"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    fixed_string<8> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::TRIM));
    EXPECT_STREQ(output.c_str(), "abcdefgh");
}


// =====================================================================================
// std::vector<primitive>
// =====================================================================================

TEST(XCdrTypeConsistency, StdVectorPrimitive_Fit)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<int32_t> input {1, 2, 3};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::vector<int32_t> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

TEST(XCdrTypeConsistency, StdVectorPrimitive_Fail)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<int32_t> input {1, 2, 3, 4, 5};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::vector<int32_t> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 3), BadParamException);
}

TEST(XCdrTypeConsistency, StdVectorPrimitive_DefaultValue)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<int32_t> input {1, 2, 3, 4, 5};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::vector<int32_t> output {99};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::DEFAULT_VALUE, 3));
    EXPECT_TRUE(output.empty());
}

TEST(XCdrTypeConsistency, StdVectorPrimitive_Trim)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<int32_t> input {1, 2, 3, 4, 5};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::vector<int32_t> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::TRIM, 3));
    const std::vector<int32_t> expected {1, 2, 3};
    EXPECT_EQ(output, expected);
}


// =====================================================================================
// std::vector<non-primitive>
// =====================================================================================

TEST(XCdrTypeConsistency, StdVectorNonPrimitive_Fit)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"alpha", "beta"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::vector<std::string> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

TEST(XCdrTypeConsistency, StdVectorNonPrimitive_Fail)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"alpha", "beta", "gamma", "delta"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::vector<std::string> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 2), BadParamException);
}

TEST(XCdrTypeConsistency, StdVectorNonPrimitive_DefaultValue)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"alpha", "beta", "gamma", "delta"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::vector<std::string> output {"x"};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());
}

TEST(XCdrTypeConsistency, StdVectorNonPrimitive_Trim)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<std::string> input {"alpha", "beta", "gamma", "delta"};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::vector<std::string> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::TRIM, 2));
    const std::vector<std::string> expected {"alpha", "beta"};
    EXPECT_EQ(output, expected);
}


// =====================================================================================
// std::vector<bool>
// =====================================================================================

TEST(XCdrTypeConsistency, StdVectorBool_Fit)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<bool> input {true, false, true};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::vector<bool> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

TEST(XCdrTypeConsistency, StdVectorBool_Fail)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<bool> input {true, false, true, true, false};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::vector<bool> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 3), BadParamException);
}

TEST(XCdrTypeConsistency, StdVectorBool_DefaultValue)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<bool> input {true, false, true, true, false};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::vector<bool> output {true};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::DEFAULT_VALUE, 3));
    EXPECT_TRUE(output.empty());
}

TEST(XCdrTypeConsistency, StdVectorBool_Trim)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::vector<bool> input {true, false, true, true, false};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::vector<bool> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::TRIM, 3));
    const std::vector<bool> expected {true, false, true};
    EXPECT_EQ(output, expected);
}


// =====================================================================================
// std::map<primitive, primitive>
// =====================================================================================

TEST(XCdrTypeConsistency, StdMapPrimitive_Fit)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, int32_t> input {{1, 10}, {2, 20}};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::map<int32_t, int32_t> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

TEST(XCdrTypeConsistency, StdMapPrimitive_Fail)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, int32_t> input {{1, 10}, {2, 20}, {3, 30}, {4, 40}};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::map<int32_t, int32_t> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 2), BadParamException);
}

TEST(XCdrTypeConsistency, StdMapPrimitive_DefaultValue)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, int32_t> input {{1, 10}, {2, 20}, {3, 30}, {4, 40}};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::map<int32_t, int32_t> output {{99, 99}};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());
}

TEST(XCdrTypeConsistency, StdMapPrimitive_Trim)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, int32_t> input {{1, 10}, {2, 20}, {3, 30}, {4, 40}};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::map<int32_t, int32_t> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::TRIM, 2));
    const std::map<int32_t, int32_t> expected {{1, 10}, {2, 20}};
    EXPECT_EQ(output, expected);
}


// =====================================================================================
// std::map<primitive, non-primitive>
// =====================================================================================

TEST(XCdrTypeConsistency, StdMapNonPrimitive_Fit)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "one"}, {2, "two"}};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::map<int32_t, std::string> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 10));
    EXPECT_EQ(output, input);
}

TEST(XCdrTypeConsistency, StdMapNonPrimitive_Fail)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::map<int32_t, std::string> output;
    EXPECT_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::FAIL, 2), BadParamException);
}

TEST(XCdrTypeConsistency, StdMapNonPrimitive_DefaultValue)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::map<int32_t, std::string> output {{99, "prefill"}};
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::DEFAULT_VALUE, 2));
    EXPECT_TRUE(output.empty());
}

TEST(XCdrTypeConsistency, StdMapNonPrimitive_Trim)
{
    char buffer[TEST_BUFFER_LENGTH];
    const std::map<int32_t, std::string> input {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    serialize_into(buffer, sizeof(buffer), input);

    FastBuffer fast_buffer(buffer, sizeof(buffer));
    Cdr cdr_des(fast_buffer);

    std::map<int32_t, std::string> output;
    ASSERT_NO_THROW(cdr_des.deserialize(output, CdrTypeConsistencyFlag::TRIM, 2));
    const std::map<int32_t, std::string> expected {{1, "one"}, {2, "two"}};
    EXPECT_EQ(output, expected);
}
