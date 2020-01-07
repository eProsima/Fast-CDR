// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <fastcdr/Cdr.h>
#include <fastcdr/FastCdr.h>
#include <fastcdr/exceptions/Exception.h>

#include <stdio.h>
#include <limits>
#include <iostream>

#include <gtest/gtest.h>

#define EXPECT_LONG_DOUBLE_EQ(val1, val2) (val1 == val2)

using namespace eprosima::fastcdr;
using namespace ::exception;

#define BUFFER_LENGTH 2000

static const uint8_t octet_t = 32;
static const char char_t =  'Z';
static const wchar_t wchar = 'Z';
static const int8_t int8 = 64;
static const uint16_t ushort_t = 65500;
static const int16_t short_t = -32700;
static const uint32_t ulong_t = 4294967200;
static const int32_t long_t = -2147483600;
static const uint64_t ulonglong_t = 18446744073709551600u;
static const int64_t longlong_t = -9223372036800;
static const float float_tt = std::numeric_limits<float>::min();
static const double double_tt = std::numeric_limits<double>::min();
static const long double ldouble_tt = std::numeric_limits<long double>::min();
static const bool bool_t = true;
static const std::string string_t = "Hola a todos, esto es un test";
static const std::string emptystring_t = "";
static const std::wstring wstring_t = L"Hola a todos, esto es un test con widestring";
static const std::wstring emptywstring_t = L"";
static const std::array<uint8_t, 5> octet_array_t = {{1, 2, 3, 4, 5}};
static const uint8_t octet_array_2_t[5] = {5, 4, 3, 2, 1};
static const std::array<char, 5> char_array_t = {{'A', 'B', 'C', 'D', 'E'}};
static const std::array<wchar_t, 5> wchar_array_t = {{'A', 'B', 'C', 'D', 'E'}};
static const char char_array_2_t[5] = {'E', 'D', 'C', 'B', 'A'};
static const wchar_t wchar_array_2_t[5] = {'E', 'D', 'C', 'B', 'A'};
static const std::array<int8_t, 5> int8_array_t = {{6, 7, 8, 9, 10}};
static const int8_t int8_array_2_t[5] = {10, 9, 8, 7, 6};
static const std::array<uint16_t, 5> ushort_array_t = {{65500, 65501, 65502, 65503, 65504}};
static const uint16_t ushort_array_2_t[5] = {65504, 65503, 65502, 65501, 65500};
static const std::array<int16_t, 5> short_array_t = {{-32700, -32701, -32702, -32703, -32704}};
static const int16_t short_array_2_t[5] = {-32704, -32703, -32702, -32701, -32700};
static const std::array<uint32_t, 5> ulong_array_t = {{4294967200, 4294967201, 4294967202, 4294967203, 4294967204}};
static const uint32_t ulong_array_2_t[5] = {4294967204, 4294967203, 4294967202, 4294967201, 4294967200};
static const std::array<int32_t, 5> long_array_t = {{-2147483600, -2147483601, -2147483602, -2147483603, -2147483604}};
static const int32_t long_array_2_t[5] = {-2147483604, -2147483603, -2147483602, -2147483601, -2147483600};
static const std::array<uint64_t, 5> ulonglong_array_t = {{18446744073709551600u, 18446744073709551601u, 18446744073709551602u, 18446744073709551603u, 18446744073709551604u}};
static const uint64_t ulonglong_array_2_t[5] = {18446744073709551604u, 18446744073709551603u, 18446744073709551602u, 18446744073709551601u, 18446744073709551600u};
static const std::array<int64_t, 5> longlong_array_t = {{-9223372036800, -9223372036801, -9223372036802, -9223372036803, -9223372036804}};
static const int64_t longlong_array_2_t[5] = {-9223372036804, -9223372036803, -9223372036802, -9223372036801, -9223372036800};
static const std::array<float, 5> float_array_t = {{float_tt, float_tt + 1, float_tt + 2, float_tt + 3, float_tt + 4}};
static const float float_array_2_t[5] = {float_tt + 4, float_tt + 3, float_tt + 2, float_tt + 1, float_tt};
static const std::array<double, 5> double_array_t = {{double_tt, double_tt + 1, double_tt + 2, double_tt + 3, double_tt + 4}};
static const double double_array_2_t[5] = {double_tt + 4, double_tt + 3, double_tt + 2, double_tt + 1, double_tt};
static const std::array<long double, 5> ldouble_array_t = {{ldouble_tt, ldouble_tt + 1, ldouble_tt + 2, ldouble_tt + 3, ldouble_tt + 4}};
static const long double ldouble_array_2_t[5] = {ldouble_tt + 4, ldouble_tt + 3, ldouble_tt + 2, ldouble_tt + 1, ldouble_tt};
static const std::array<bool, 5> bool_array_t = {{true, false, true, false, true}};
static const bool bool_array_2_t[5] = {false, true, false, true, false};
static const std::array<std::string, 5> string_array_t = {{"HOLA", "ADIOS", "HELLO", "BYE", "GOODBYE"}};
static const std::array<std::wstring, 5> wstring_array_t = {{L"HOLA", L"ADIOS", L"HELLO", L"BYE", L"GOODBYE"}};
static const std::string string_array_2_t[5] = {"HOLA", "ADIOS", "HELLO", "BYE", "GOODBYE"};
static const std::wstring wstring_array_2_t[5] = {L"HOLA", L"ADIOS", L"HELLO", L"BYE", L"GOODBYE"};
static const std::vector<uint8_t> octet_vector_t(octet_array_2_t, octet_array_2_t + sizeof(octet_array_2_t) / sizeof(uint8_t));
static const std::vector<char> char_vector_t(char_array_2_t, char_array_2_t + sizeof(char_array_2_t) / sizeof(char));
static const std::vector<wchar_t> wchar_vector_t(wchar_array_2_t, wchar_array_2_t + sizeof(wchar_array_2_t) / sizeof(wchar_t));
static const std::vector<int8_t> int8_vector_t(int8_array_2_t, int8_array_2_t + sizeof(int8_array_2_t) / sizeof(int8_t));
static const std::vector<uint16_t> ushort_vector_t(ushort_array_2_t, ushort_array_2_t + sizeof(ushort_array_2_t) / sizeof(uint16_t));
static const std::vector<int16_t> short_vector_t(short_array_2_t, short_array_2_t + sizeof(short_array_2_t) / sizeof(int16_t));
static const std::vector<uint32_t> ulong_vector_t(ulong_array_2_t, ulong_array_2_t + sizeof(ulong_array_2_t) / sizeof(uint32_t));
static const std::vector<int32_t> long_vector_t(long_array_2_t, long_array_2_t + sizeof(long_array_2_t) / sizeof(int32_t));
static const std::vector<uint64_t> ulonglong_vector_t(ulonglong_array_2_t, ulonglong_array_2_t + sizeof(ulonglong_array_2_t) / sizeof(uint64_t));
static const std::vector<int64_t> longlong_vector_t(longlong_array_2_t, longlong_array_2_t + sizeof(longlong_array_2_t) / sizeof(int64_t));
static const std::vector<float> float_vector_t(float_array_2_t, float_array_2_t + sizeof(float_array_2_t) / sizeof(float));
static const std::vector<double> double_vector_t(double_array_2_t, double_array_2_t + sizeof(double_array_2_t) / sizeof(double));
static const std::vector<long double> ldouble_vector_t(ldouble_array_2_t, ldouble_array_2_t + sizeof(ldouble_array_2_t) / sizeof(long double));
static const std::vector<bool> bool_vector_t(bool_array_2_t, bool_array_2_t + sizeof(bool_array_2_t) / sizeof(bool));
static const std::vector<std::string> string_vector_t(string_array_2_t, string_array_2_t + sizeof(string_array_2_t) / sizeof(std::string));
static const std::vector<std::wstring> wstring_vector_t(wstring_array_2_t, wstring_array_2_t + sizeof(wstring_array_2_t) / sizeof(std::wstring));
static const std::array<std::array<std::array<uint32_t, 3>, 2>, 2> triple_ulong_array_t = {{ {{ {{1, 2, 3}}, {{4, 5, 6}} }}, {{ {{7, 8, 9}}, {{10, 11, 12}} }} }};
// Added because error 336.
static const uint8_t octet_seq_t[5] = {5, 4, 3, 2, 1};
static const char char_seq_t[5] = {'E', 'D', 'C', 'B', 'A'};
static const wchar_t wchar_seq_t[5] = {'E', 'D', 'C', 'B', 'A'};
static const int8_t int8_seq_t[5] = {10, 9, 8, 7, 6};
static const uint16_t ushort_seq_t[5] = {65504, 65503, 65502, 65501, 65500};
static const int16_t short_seq_t[5] = {-32704, -32703, -32702, -32701, -32700};
static const uint32_t ulong_seq_t[5] = {4294967204, 4294967203, 4294967202, 4294967201, 4294967200};
static const int32_t long_seq_t[5] = {-2147483604, -2147483603, -2147483602, -2147483601, -2147483600};
static const uint64_t ulonglong_seq_t[5] = {18446744073709551604u, 18446744073709551603u, 18446744073709551602u, 18446744073709551601u, 18446744073709551600u};
static const int64_t longlong_seq_t[5] = {-9223372036804, -9223372036803, -9223372036802, -9223372036801, -9223372036800};
static const float float_seq_t[5] = {float_tt + 4, float_tt + 3, float_tt + 2, float_tt + 1, float_tt};
static const double double_seq_t[5] = {double_tt + 4, double_tt + 3, double_tt + 2, double_tt + 1, double_tt};
static const long double ldouble_seq_t[5] = {ldouble_tt + 4, ldouble_tt + 3, ldouble_tt + 2, ldouble_tt + 1, ldouble_tt};
static const bool bool_seq_t[5] = {true, true, false, false, true};
static const std::string string_seq_t[5] = {"HELLO", "BYE", "GOODBYE", "HOLA", "ADIOS"};
static const std::wstring wstring_seq_t[5] = {L"HELLO", L"BYE", L"GOODBYE", L"HOLA", L"ADIOS"};
// Added because error 337
static const char *c_string_t = "HOLA";
static const wchar_t *c_wstring_t = L"HOLA";

template<typename T>
void EXPECT_ARRAY_EQ(T *array1, const T *array2, size_t size)
{
    for(size_t count = 0; count < size; ++count)
    {
        EXPECT_EQ(array1[count], array2[count]);
    }
}

static void EXPECT_ARRAY_FLOAT_EQ(float *array1, const float *array2, size_t size)
{
    for(size_t count = 0; count < size; ++count)
    {
        EXPECT_FLOAT_EQ(array1[count], array2[count]);
    }
}

static void EXPECT_ARRAY_DOUBLE_EQ(double *array1, const double *array2, size_t size)
{
    for(size_t count = 0; count < size; ++count)
    {
        EXPECT_DOUBLE_EQ(array1[count], array2[count]);
    }
}

static void EXPECT_ARRAY_LONG_DOUBLE_EQ(long double *array1, const long double *array2, size_t size)
{
    for(size_t count = 0; count < size; ++count)
    {
        EXPECT_LONG_DOUBLE_EQ(array1[count], array2[count]);
    }
}

static std::string GET_BINARY_STRING() {
    const char binary_string_buffer[] = "\000\041";
    const std::string binary_string(binary_string_buffer,
        sizeof(binary_string_buffer));
    return binary_string;
}

static std::wstring GET_BINARY_WSTRING() {
    const wchar_t binary_string_buffer[] = L"\000\041";
    const std::wstring binary_string(binary_string_buffer,
        sizeof(binary_string_buffer) / sizeof(wchar_t));
    return binary_string;
}

template<class T>
void CDR_STREAM_HANDLE_ERROR(const T& value) {
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << value;
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    T deserialized_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> deserialized_value;
    });

    EXPECT_EQ(deserialized_value, value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << value;
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> deserialized_value;
    },
    NotEnoughMemoryException);
}

template<class T>
void FAST_CDR_STREAM_HANDLE_ERROR(const T& value) {
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << value;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    T deserialized_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> deserialized_value;
    });

    EXPECT_EQ(deserialized_value, value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << value;
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> deserialized_value;
    },
    NotEnoughMemoryException);
}

TEST(FastBufferTests, Constructors)
{
    char buffer[BUFFER_LENGTH];

    /*
    // Copy assignment
    {
        eprosima::fastcdr::FastBuffer buffer1(buffer, BUFFER_LENGTH);
        buffer1.resize(1000);
        eprosima::fastcdr::FastBuffer buffer2 = buffer1;
    }

    // Copy constructor
    {
        eprosima::fastcdr::FastBuffer buffer1(buffer, BUFFER_LENGTH);
        buffer1.resize(1000);
        eprosima::fastcdr::FastBuffer buffer2(buffer1);
    }
    */

    // Move assignment
    {
        eprosima::fastcdr::FastBuffer buffer1(buffer, BUFFER_LENGTH);
        buffer1.resize(1000);
        eprosima::fastcdr::FastBuffer buffer2 = std::move(buffer1);
    }

    // Move constructor
    {
        eprosima::fastcdr::FastBuffer buffer1(buffer, BUFFER_LENGTH);
        buffer1.resize(1000);
        eprosima::fastcdr::FastBuffer buffer2(std::move(buffer1));
    }
}

TEST(CDRTests, Octet)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << octet_t;
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint8_t octet_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> octet_value;
    });

    EXPECT_EQ(octet_value, octet_t);
}

TEST(CDRTests, Char)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << char_t;
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    char char_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> char_value;
    });

    EXPECT_EQ(char_value, char_t);
}

TEST(CDRTests, WChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << wchar;
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    wchar_t char_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> char_value;
    });

    EXPECT_EQ(char_value, wchar);
}

TEST(CDRTests, Int8)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << int8;
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint8_t int8_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> int8_value;
    });

    EXPECT_EQ(int8_value, int8);
}

TEST(CDRTests, UnsignedShort)
{
    CDR_STREAM_HANDLE_ERROR(ushort_t);
}

TEST(CDRTests, Short)
{
    CDR_STREAM_HANDLE_ERROR(short_t);
}

TEST(CDRTests, UnsignedLong)
{
    CDR_STREAM_HANDLE_ERROR(ulong_t);
}

TEST(CDRTests, Long)
{
    CDR_STREAM_HANDLE_ERROR(long_t);
}

TEST(CDRTests, UnsignedLongLong)
{
    CDR_STREAM_HANDLE_ERROR(ulonglong_t);
}

TEST(CDRTests, LongLong)
{
    CDR_STREAM_HANDLE_ERROR(longlong_t);
}

TEST(CDRTests, Float)
{
    CDR_STREAM_HANDLE_ERROR(float_tt);
}

TEST(CDRTests, Double)
{
    CDR_STREAM_HANDLE_ERROR(double_tt);
}

TEST(CDRTests, LongDouble)
{
    CDR_STREAM_HANDLE_ERROR(ldouble_tt);
}

TEST(CDRTests, Boolean)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << bool_t;
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    bool bool_value = false;

    EXPECT_NO_THROW(
    {
        cdr_des >> bool_value;
    });

    EXPECT_EQ(bool_value, bool_t);
}

TEST(CDRTests, String)
{
    CDR_STREAM_HANDLE_ERROR(string_t);
}

TEST(CDRTests, BinaryString)
{
    CDR_STREAM_HANDLE_ERROR(GET_BINARY_STRING());
}

TEST(CDRTests, WString)
{
    CDR_STREAM_HANDLE_ERROR(wstring_t);
}

TEST(CDRTests, BinaryWString)
{
    CDR_STREAM_HANDLE_ERROR(GET_BINARY_WSTRING());
}

TEST(CDRTests, EmptyString)
{
    CDR_STREAM_HANDLE_ERROR(emptystring_t);
}

TEST(CDRTests, EmptyWString)
{
    CDR_STREAM_HANDLE_ERROR(emptywstring_t);
}

TEST(CDRTests, STDArrayOctet)
{
    CDR_STREAM_HANDLE_ERROR(octet_array_t);
}

TEST(CDRTests, STDArrayChar)
{
    CDR_STREAM_HANDLE_ERROR(char_array_t);
}

TEST(CDRTests, STDArrayWChar)
{
    CDR_STREAM_HANDLE_ERROR(wchar_array_t);
}

TEST(CDRTests, STDArrayInt8)
{
    CDR_STREAM_HANDLE_ERROR(int8_array_t);
}

TEST(CDRTests, STDArrayUnsignedShort)
{
    CDR_STREAM_HANDLE_ERROR(ushort_array_t);
}

TEST(CDRTests, STDArrayShort)
{
    CDR_STREAM_HANDLE_ERROR(short_array_t);
}

TEST(CDRTests, STDArrayUnsignedLong)
{
    CDR_STREAM_HANDLE_ERROR(ulong_array_t);
}

TEST(CDRTests, STDArrayLong)
{
    CDR_STREAM_HANDLE_ERROR(long_array_t);
}

TEST(CDRTests, STDArrayUnsignedLongLong)
{
    CDR_STREAM_HANDLE_ERROR(ulonglong_array_t);
}

TEST(CDRTests, STDArrayLongLong)
{
    CDR_STREAM_HANDLE_ERROR(longlong_array_t);
}

TEST(CDRTests, STDArrayFloat)
{
    CDR_STREAM_HANDLE_ERROR(float_array_t);
}

TEST(CDRTests, STDArrayDouble)
{
    CDR_STREAM_HANDLE_ERROR(double_array_t);
}

TEST(CDRTests, STDArrayLongDouble)
{
    CDR_STREAM_HANDLE_ERROR(ldouble_array_t);
}

TEST(CDRTests, STDArrayBoolean)
{
    CDR_STREAM_HANDLE_ERROR(bool_array_t);
}

TEST(CDRTests, STDArrayString)
{
    CDR_STREAM_HANDLE_ERROR(string_array_t);
}

TEST(CDRTests, STDArrayWString)
{
    CDR_STREAM_HANDLE_ERROR(wstring_array_t);
}

TEST(CDRTests, ArrayOctet)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(octet_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint8_t octet_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(octet_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(octet_array_2_value, octet_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(octet_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(octet_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(char_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    char char_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(char_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(char_array_2_value, char_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(char_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(char_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayWChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(wchar_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    wchar_t char_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(char_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(char_array_2_value, wchar_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(wchar_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(char_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayInt8)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(int8_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    int8_t int8_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(int8_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(int8_array_2_value, int8_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(int8_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(int8_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayUnsignedShort)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(ushort_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint16_t ushort_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(ushort_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(ushort_array_2_value, ushort_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(ushort_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(ushort_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayShort)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(short_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    int16_t short_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(short_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(short_array_2_value, short_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(short_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(short_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayUnsignedLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(ulong_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint32_t ulong_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(ulong_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(ulong_array_2_value, ulong_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(ulong_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(ulong_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(long_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    int32_t long_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(long_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(long_array_2_value, long_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(long_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(long_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayUnsignedLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(ulonglong_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint64_t ulonglong_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(ulonglong_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(ulonglong_array_2_value, ulonglong_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(ulonglong_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(ulonglong_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(longlong_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    int64_t longlong_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(longlong_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(longlong_array_2_value, longlong_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(longlong_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(longlong_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayFloat)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(float_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    float float_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(float_array_2_value, 5);
    });

    EXPECT_ARRAY_FLOAT_EQ(float_array_2_value, float_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(float_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(float_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(double_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    double double_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(double_array_2_value, 5);
    });

    EXPECT_ARRAY_DOUBLE_EQ(double_array_2_value, double_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(double_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(double_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayLongDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(ldouble_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    long double ldouble_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(ldouble_array_2_value, 5);
    });

    EXPECT_ARRAY_LONG_DOUBLE_EQ(ldouble_array_2_value, ldouble_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(ldouble_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(ldouble_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayBoolean)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(bool_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    bool bool_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(bool_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(bool_array_2_value, bool_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(bool_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(bool_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(string_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    std::string string_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(string_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(string_array_2_value, string_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(string_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(string_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, ArrayWString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(wstring_array_2_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    std::wstring string_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(string_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(string_array_2_value, wstring_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(wstring_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(string_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, STDVectorOctet)
{
    CDR_STREAM_HANDLE_ERROR(octet_vector_t);
}

TEST(CDRTests, STDVectorChar)
{
    CDR_STREAM_HANDLE_ERROR(char_vector_t);
}

TEST(CDRTests, STDVectorWChar)
{
    CDR_STREAM_HANDLE_ERROR(wchar_vector_t);
}

TEST(CDRTests, STDVectorInt8)
{
    CDR_STREAM_HANDLE_ERROR(int8_vector_t);
}

TEST(CDRTests, STDVectorUnsignedShort)
{
    CDR_STREAM_HANDLE_ERROR(ushort_vector_t);
}

TEST(CDRTests, STDVectorShort)
{
    CDR_STREAM_HANDLE_ERROR(short_vector_t);
}

TEST(CDRTests, STDVectorUnsignedLong)
{
    CDR_STREAM_HANDLE_ERROR(ulong_vector_t);
}

TEST(CDRTests, STDVectorLong)
{
    CDR_STREAM_HANDLE_ERROR(long_vector_t);
}

TEST(CDRTests, STDVectorUnsignedLongLong)
{
    CDR_STREAM_HANDLE_ERROR(ulonglong_vector_t);
}

TEST(CDRTests, STDVectorLongLong)
{
    CDR_STREAM_HANDLE_ERROR(longlong_vector_t);
}

TEST(CDRTests, STDVectorFloat)
{
    CDR_STREAM_HANDLE_ERROR(float_vector_t);
}

TEST(CDRTests, STDVectorDouble)
{
    CDR_STREAM_HANDLE_ERROR(double_vector_t);
}

TEST(CDRTests, STDVectorLongDouble)
{
    CDR_STREAM_HANDLE_ERROR(ldouble_vector_t);
}

TEST(CDRTests, STDVectorBoolean)
{
    CDR_STREAM_HANDLE_ERROR(bool_vector_t);
}

TEST(CDRTests, STDVectorString)
{
    CDR_STREAM_HANDLE_ERROR(string_vector_t);
}

TEST(CDRTests, STDVectorWString)
{
    CDR_STREAM_HANDLE_ERROR(wstring_vector_t);
}

TEST(CDRTests, STDTripleArrayUnsignedLong)
{
    CDR_STREAM_HANDLE_ERROR(triple_ulong_array_t);
}

TEST(CDRTests, SequenceOctet)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(octet_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint8_t *octet_seq_value = NULL; size_t octet_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(octet_seq_value, octet_seq_len);
    });

    EXPECT_EQ(octet_seq_len, 5u);
    EXPECT_ARRAY_EQ(octet_seq_value, octet_seq_t, octet_seq_len);

    free(octet_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(octet_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(octet_seq_value, octet_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(char_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    char *char_seq_value = NULL; size_t char_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(char_seq_value, char_seq_len);
    });

    EXPECT_EQ(char_seq_len, 5u);
    EXPECT_ARRAY_EQ(char_seq_value, char_seq_t, char_seq_len);

    free(char_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(char_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(char_seq_value, char_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceWChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(wchar_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    wchar_t *char_seq_value = NULL; size_t char_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(char_seq_value, char_seq_len);
    });

    EXPECT_EQ(char_seq_len, 5u);
    EXPECT_ARRAY_EQ(char_seq_value, wchar_seq_t, char_seq_len);

    free(char_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(wchar_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(char_seq_value, char_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceInt8)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(int8_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    int8_t *int8_seq_value = NULL; size_t int8_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(int8_seq_value, int8_seq_len);
    });

    EXPECT_EQ(int8_seq_len, 5u);
    EXPECT_ARRAY_EQ(int8_seq_value, int8_seq_t, int8_seq_len);

    free(int8_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(int8_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(int8_seq_value, int8_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceUnsignedShort)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(ushort_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint16_t *ushort_seq_value = NULL; size_t ushort_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(ushort_seq_value, ushort_seq_len);
    });

    EXPECT_EQ(ushort_seq_len, 5u);
    EXPECT_ARRAY_EQ(ushort_seq_value, ushort_seq_t, ushort_seq_len);

    free(ushort_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(ushort_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(ushort_seq_value, ushort_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceShort)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(short_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    int16_t *short_seq_value = NULL; size_t short_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(short_seq_value, short_seq_len);
    });

    EXPECT_EQ(short_seq_len, 5u);
    EXPECT_ARRAY_EQ(short_seq_value, short_seq_t, short_seq_len);

    free(short_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(short_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(short_seq_value, short_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceUnsignedLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(ulong_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint32_t *ulong_seq_value = NULL; size_t ulong_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(ulong_seq_value, ulong_seq_len);
    });

    EXPECT_EQ(ulong_seq_len, 5u);
    EXPECT_ARRAY_EQ(ulong_seq_value, ulong_seq_t, ulong_seq_len);

    free(ulong_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(ulong_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(ulong_seq_value, ulong_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(long_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    int32_t *long_seq_value = NULL; size_t long_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(long_seq_value, long_seq_len);
    });

    EXPECT_EQ(long_seq_len, 5u);
    EXPECT_ARRAY_EQ(long_seq_value, long_seq_t, long_seq_len);

    free(long_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(long_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(long_seq_value, long_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceUnsignedLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(ulonglong_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint64_t *ulonglong_seq_value = NULL; size_t ulonglong_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(ulonglong_seq_value, ulonglong_seq_len);
    });

    EXPECT_EQ(ulonglong_seq_len, 5u);
    EXPECT_ARRAY_EQ(ulonglong_seq_value, ulonglong_seq_t, ulonglong_seq_len);

    free(ulonglong_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(ulonglong_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(ulonglong_seq_value, ulonglong_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(longlong_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    int64_t *longlong_seq_value = NULL; size_t longlong_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(longlong_seq_value, longlong_seq_len);
    });

    EXPECT_EQ(longlong_seq_len, 5u);
    EXPECT_ARRAY_EQ(longlong_seq_value, longlong_seq_t, longlong_seq_len);

    free(longlong_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(longlong_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(longlong_seq_value, longlong_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceFloat)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(float_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    float *float_seq_value = NULL; size_t float_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(float_seq_value, float_seq_len);
    });

    EXPECT_EQ(float_seq_len, 5u);
    EXPECT_ARRAY_FLOAT_EQ(float_seq_value, float_seq_t, float_seq_len);

    free(float_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(float_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(float_seq_value, float_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(double_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    double *double_seq_value = NULL; size_t double_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(double_seq_value, double_seq_len);
    });

    EXPECT_EQ(double_seq_len, 5u);
    EXPECT_ARRAY_DOUBLE_EQ(double_seq_value, double_seq_t, double_seq_len);

    free(double_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(double_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(double_seq_value, double_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceLongDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(ldouble_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    long double *ldouble_seq_value = NULL; size_t ldouble_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(ldouble_seq_value, ldouble_seq_len);
    });

    EXPECT_EQ(ldouble_seq_len, 5u);
    EXPECT_ARRAY_LONG_DOUBLE_EQ(ldouble_seq_value, ldouble_seq_t, ldouble_seq_len);

    free(ldouble_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(ldouble_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(ldouble_seq_value, ldouble_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceBoolean)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(bool_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    bool *bool_seq_value = NULL; size_t bool_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(bool_seq_value, bool_seq_len);
    });

    EXPECT_EQ(bool_seq_len, 5u);
    EXPECT_ARRAY_EQ(bool_seq_value, bool_seq_t, bool_seq_len);

    free(bool_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(bool_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(bool_seq_value, bool_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(string_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    std::string *string_seq_value = NULL; size_t string_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(string_seq_value, string_seq_len);
    });

    EXPECT_EQ(string_seq_len, 5u);
    EXPECT_ARRAY_EQ(string_seq_value, string_seq_t, string_seq_len);

    delete[] string_seq_value;

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(string_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(string_seq_value, string_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, SequenceWString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(wstring_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    std::wstring *string_seq_value = NULL; size_t string_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(string_seq_value, string_seq_len);
    });

    EXPECT_EQ(string_seq_len, 5u);
    EXPECT_ARRAY_EQ(string_seq_value, wstring_seq_t, string_seq_len);

    delete[] string_seq_value;

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(wstring_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(string_seq_value, string_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, CString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serialize(c_string_t);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    char *c_string_value = NULL;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize(c_string_value);
    });

    EXPECT_EQ(strcmp(c_string_value, c_string_t) , 0);

    free(c_string_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize(c_string_t);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize(c_string_value);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, CWString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serialize(c_wstring_t);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    wchar_t *c_string_value = NULL;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize(c_string_value);
    });

    EXPECT_EQ(wcscmp(c_string_value, c_wstring_t) , 0);

    free(c_string_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize(c_wstring_t);
    },
    NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize(c_string_value);
    },
    NotEnoughMemoryException);
}

TEST(CDRTests, Complete)
{
    char buffer[BUFFER_LENGTH * 2];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH * 2);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << octet_t << char_t << int8 << ushort_t << short_t << ulong_t << long_t << ulonglong_t << longlong_t <<
            float_tt << double_tt << ldouble_tt << bool_t << string_t;
        cdr_ser << octet_array_t << char_array_t << int8_array_t << ushort_array_t << short_array_t << ulong_array_t <<
            long_array_t << ulonglong_array_t << longlong_array_t << float_array_t << double_array_t << ldouble_array_t << bool_array_t;
        cdr_ser.serializeArray(octet_array_2_t, 5).serializeArray(char_array_2_t, 5).serializeArray(int8_array_2_t, 5).
            serializeArray(ushort_array_2_t, 5).serializeArray(short_array_2_t, 5).serializeArray(ulong_array_2_t, 5).
            serializeArray(long_array_2_t, 5).serializeArray(ulonglong_array_2_t, 5).serializeArray(longlong_array_2_t, 5).
            serializeArray(float_array_2_t, 5).serializeArray(double_array_2_t, 5).serializeArray(ldouble_array_2_t, 5).serializeArray(bool_array_2_t, 5);
        cdr_ser << octet_vector_t << char_vector_t << int8_vector_t << ushort_vector_t << short_vector_t << ulong_vector_t <<
            long_vector_t << ulonglong_vector_t << longlong_vector_t << float_vector_t << double_vector_t << ldouble_vector_t << bool_vector_t;
        cdr_ser << triple_ulong_array_t;
        cdr_ser.serializeSequence(octet_seq_t, 5).serializeSequence(char_seq_t, 5).serializeSequence(int8_seq_t, 5).
            serializeSequence(ushort_seq_t, 5).serializeSequence(short_seq_t, 5).serializeSequence(ulong_seq_t, 5).
            serializeSequence(long_seq_t, 5).serializeSequence(ulonglong_seq_t, 5).serializeSequence(longlong_seq_t, 5).
            serializeSequence(float_seq_t, 5).serializeSequence(double_seq_t, 5).serializeSequence(ldouble_seq_t, 5);
        cdr_ser.serialize(c_string_t);
        // Wstring and wchar
        cdr_ser << wchar << wstring_t;
        cdr_ser.serializeArray(wchar_array_2_t, 5).serializeArray(wstring_array_2_t, 5);
        cdr_ser << wchar_array_t << wstring_array_t;
        cdr_ser << wchar_vector_t << wstring_vector_t;
        cdr_ser.serializeSequence(wchar_seq_t, 5);
        cdr_ser.serializeSequence(wstring_seq_t, 5);
        cdr_ser.serializeSequence(string_seq_t, 5);
        cdr_ser.serialize(c_wstring_t);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint8_t octet_value = 0;
    char char_value = 0;
    wchar_t wchar_value = 0;
    int8_t int8_value = 0;
    uint16_t ushort_value =0;
    int16_t short_value = 0;
    uint32_t ulong_value = 0;
    int32_t long_value = 0;
    uint64_t ulonglong_value = 0;
    int64_t longlong_value = 0;
    float float_value = 0;
    double double_value = 0;
    long double ldouble_value = 0;
    bool bool_value = false;
    std::string string_value = "";
    std::wstring wstring_value = L"";
    std::array<uint8_t, 5> octet_array_value;
    uint8_t octet_array_2_value[5];
    std::array<char, 5> char_array_value;
    std::array<wchar_t, 5> wchar_array_value;
    char char_array_2_value[5];
    wchar_t wchar_array_2_value[5];
    std::array<int8_t, 5> int8_array_value;
    int8_t int8_array_2_value[5];
    std::array<uint16_t, 5> ushort_array_value;
    uint16_t ushort_array_2_value[5];
    std::array<int16_t, 5> short_array_value;
    int16_t short_array_2_value[5];
    std::array<uint32_t, 5> ulong_array_value;
    uint32_t ulong_array_2_value[5];
    std::array<int32_t, 5> long_array_value;
    int32_t long_array_2_value[5];
    std::array<uint64_t, 5> ulonglong_array_value;
    uint64_t ulonglong_array_2_value[5];
    std::array<int64_t, 5> longlong_array_value;
    int64_t longlong_array_2_value[5];
    std::array<float, 5> float_array_value;
    float float_array_2_value[5];
    std::array<double, 5> double_array_value;
    double double_array_2_value[5];
    std::array<long double, 5> ldouble_array_value;
    long double ldouble_array_2_value[5];
    std::array<bool, 5> bool_array_value;
    bool bool_array_2_value[5];
    std::vector<uint8_t> octet_vector_value;
    std::vector<char> char_vector_value;
    std::vector<wchar_t> wchar_vector_value;
    std::vector<int8_t> int8_vector_value;
    std::vector<uint16_t> ushort_vector_value;
    std::vector<int16_t> short_vector_value;
    std::vector<uint32_t> ulong_vector_value;
    std::vector<int32_t> long_vector_value;
    std::vector<uint64_t> ulonglong_vector_value;
    std::vector<int64_t> longlong_vector_value;
    std::vector<float> float_vector_value;
    std::vector<double> double_vector_value;
    std::vector<long double> ldouble_vector_value;
    std::vector<bool> bool_vector_value;
    std::array<std::array<std::array<uint32_t, 3>, 2>, 2> triple_ulong_array_value;
    uint8_t *octet_seq_value = NULL; size_t octet_seq_len;
    char *char_seq_value = NULL; size_t char_seq_len;
    wchar_t *wchar_seq_value = NULL; size_t wchar_seq_len;
    int8_t *int8_seq_value = NULL; size_t int8_seq_len;
    uint16_t *ushort_seq_value = NULL; size_t ushort_seq_len;
    int16_t *short_seq_value = NULL; size_t short_seq_len;
    uint32_t *ulong_seq_value = NULL; size_t ulong_seq_len;
    int32_t *long_seq_value = NULL; size_t long_seq_len;
    uint64_t *ulonglong_seq_value = NULL; size_t ulonglong_seq_len;
    int64_t *longlong_seq_value = NULL; size_t longlong_seq_len;
    float *float_seq_value = NULL; size_t float_seq_len;
    double *double_seq_value = NULL; size_t double_seq_len;
    long double *ldouble_seq_value = NULL; size_t ldouble_seq_len;
    char *c_string_value = NULL;
    wchar_t *c_wstring_value = NULL;
    std::array<std::wstring, 5> wstring_array_value;
    std::wstring wstring_array_2_value[5];
    std::vector<std::wstring> wstring_vector_value;
    std::wstring *wstring_seq_value = NULL; size_t wstring_seq_len;
    std::string *string_seq_value = NULL; size_t string_seq_len;

    EXPECT_NO_THROW(
    {
        cdr_des >> octet_value >> char_value >> int8_value >> ushort_value >> short_value >> ulong_value >> long_value >>
            ulonglong_value >> longlong_value >> float_value >> double_value >> ldouble_value  >> bool_value >> string_value;
        cdr_des >> octet_array_value >> char_array_value >> int8_array_value >> ushort_array_value >> short_array_value >>
            ulong_array_value >> long_array_value >> ulonglong_array_value >> longlong_array_value >> float_array_value >>
            double_array_value >> ldouble_array_value >> bool_array_value;
        cdr_des.deserializeArray(octet_array_2_value, 5).deserializeArray(char_array_2_value, 5).deserializeArray(int8_array_2_value, 5).
            deserializeArray(ushort_array_2_value, 5).deserializeArray(short_array_2_value, 5).deserializeArray(ulong_array_2_value, 5).
                deserializeArray(long_array_2_value, 5).deserializeArray(ulonglong_array_2_value, 5).deserializeArray(longlong_array_2_value, 5).
                deserializeArray(float_array_2_value, 5).deserializeArray(double_array_2_value, 5).deserializeArray(ldouble_array_2_value, 5).
                deserializeArray(bool_array_2_value, 5);
        cdr_des >> octet_vector_value >> char_vector_value >> int8_vector_value >> ushort_vector_value >> short_vector_value >>
            ulong_vector_value >> long_vector_value >> ulonglong_vector_value >> longlong_vector_value >> float_vector_value >>
            double_vector_value >> ldouble_vector_value >> bool_vector_value;
        cdr_des >> triple_ulong_array_value;
        cdr_des.deserializeSequence(octet_seq_value, octet_seq_len).deserializeSequence(char_seq_value, char_seq_len).
            deserializeSequence(int8_seq_value, int8_seq_len).deserializeSequence(ushort_seq_value, ushort_seq_len).
            deserializeSequence(short_seq_value, short_seq_len).deserializeSequence(ulong_seq_value, ulong_seq_len).
            deserializeSequence(long_seq_value, long_seq_len).deserializeSequence(ulonglong_seq_value, ulonglong_seq_len).
            deserializeSequence(longlong_seq_value, longlong_seq_len).deserializeSequence(float_seq_value, float_seq_len).
            deserializeSequence(double_seq_value, double_seq_len).deserializeSequence(ldouble_seq_value, ldouble_seq_len);
        cdr_des.deserialize(c_string_value);
        // WString and wchar_t
        cdr_des >> wchar_value >> wstring_value;
        cdr_des.deserializeArray(wchar_array_2_value, 5).deserializeArray(wstring_array_2_value, 5);
        cdr_des >> wchar_array_value >> wstring_array_value;
        cdr_des >> wchar_vector_value >> wstring_vector_value;
        cdr_des.deserializeSequence(wchar_seq_value, wchar_seq_len);
        cdr_des.deserializeSequence(wstring_seq_value, wstring_seq_len);
        cdr_des.deserializeSequence(string_seq_value, string_seq_len);
        cdr_des.deserialize(c_wstring_value);
    });

    EXPECT_EQ(octet_value, octet_t);
    EXPECT_EQ(char_value, char_t);
    EXPECT_EQ(wchar_value, wchar);
    EXPECT_EQ(int8_value, int8);
    EXPECT_EQ(ushort_value, ushort_t);
    EXPECT_EQ(short_value, short_t);
    EXPECT_EQ(ulong_value, ulong_t);
    EXPECT_EQ(long_value, long_t);
    EXPECT_EQ(ulonglong_value, ulonglong_t);
    EXPECT_EQ(longlong_value, longlong_t);
    EXPECT_FLOAT_EQ(float_value, float_tt);
    EXPECT_DOUBLE_EQ(double_value, double_tt);
    EXPECT_LONG_DOUBLE_EQ(ldouble_value, ldouble_tt);
    EXPECT_EQ(bool_value, bool_t);
    EXPECT_EQ(string_value, string_t);
    EXPECT_EQ(wstring_value, wstring_t);

    EXPECT_EQ(octet_array_value, octet_array_t);
    EXPECT_EQ(char_array_value, char_array_t);
    EXPECT_EQ(wchar_array_value, wchar_array_t);
    EXPECT_EQ(int8_array_value, int8_array_t);
    EXPECT_EQ(ushort_array_value, ushort_array_t);
    EXPECT_EQ(short_array_value, short_array_t);
    EXPECT_EQ(ulong_array_value, ulong_array_t);
    EXPECT_EQ(long_array_value, long_array_t);
    EXPECT_EQ(ulonglong_array_value, ulonglong_array_t);
    EXPECT_EQ(longlong_array_value, longlong_array_t);
    EXPECT_EQ(float_array_value, float_array_t);
    EXPECT_EQ(double_array_value, double_array_t);
    EXPECT_EQ(ldouble_array_value, ldouble_array_t);
    EXPECT_EQ(bool_array_value, bool_array_t);
    EXPECT_EQ(wstring_array_value, wstring_array_t);

    EXPECT_ARRAY_EQ(octet_array_2_value, octet_array_2_t, 5);
    EXPECT_ARRAY_EQ(char_array_2_value, char_array_2_t, 5);
    EXPECT_ARRAY_EQ(wchar_array_2_value, wchar_array_2_t, 5);
    EXPECT_ARRAY_EQ(int8_array_2_value, int8_array_2_t, 5);
    EXPECT_ARRAY_EQ(ushort_array_2_value, ushort_array_2_t, 5);
    EXPECT_ARRAY_EQ(short_array_2_value, short_array_2_t, 5);
    EXPECT_ARRAY_EQ(ulong_array_2_value, ulong_array_2_t, 5);
    EXPECT_ARRAY_EQ(long_array_2_value, long_array_2_t, 5);
    EXPECT_ARRAY_EQ(ulonglong_array_2_value, ulonglong_array_2_t, 5);
    EXPECT_ARRAY_EQ(longlong_array_2_value, longlong_array_2_t, 5);
    EXPECT_ARRAY_FLOAT_EQ(float_array_2_value, float_array_2_t, 5);
    EXPECT_ARRAY_DOUBLE_EQ(double_array_2_value, double_array_2_t, 5);
    EXPECT_ARRAY_LONG_DOUBLE_EQ(ldouble_array_2_value, ldouble_array_2_t, 5);
    EXPECT_ARRAY_EQ(bool_array_2_value, bool_array_2_t, 5);
    EXPECT_ARRAY_EQ(wstring_array_2_value, wstring_array_2_t, 5);

    EXPECT_EQ(octet_vector_value, octet_vector_t);
    EXPECT_EQ(char_vector_value, char_vector_t);
    EXPECT_EQ(wchar_vector_value, wchar_vector_t);
    EXPECT_EQ(int8_vector_value, int8_vector_t);
    EXPECT_EQ(ushort_vector_value, ushort_vector_t);
    EXPECT_EQ(short_vector_value, short_vector_t);
    EXPECT_EQ(ulong_vector_value, ulong_vector_t);
    EXPECT_EQ(long_vector_value, long_vector_t);
    EXPECT_EQ(ulonglong_vector_value, ulonglong_vector_t);
    EXPECT_EQ(longlong_vector_value, longlong_vector_t);
    EXPECT_EQ(float_vector_value, float_vector_t);
    EXPECT_EQ(double_vector_value, double_vector_t);
    EXPECT_EQ(ldouble_vector_value, ldouble_vector_t);
    EXPECT_EQ(bool_vector_value, bool_vector_t);
    EXPECT_EQ(wstring_vector_value, wstring_vector_t);

    EXPECT_EQ(triple_ulong_array_t, triple_ulong_array_value);
    EXPECT_EQ(octet_seq_len, 5u);
    EXPECT_ARRAY_EQ(octet_seq_value, octet_seq_t, 5);
    EXPECT_EQ(char_seq_len, 5u);
    EXPECT_ARRAY_EQ(char_seq_value, char_seq_t, 5);
    EXPECT_EQ(wchar_seq_len, 5u);
    EXPECT_ARRAY_EQ(wchar_seq_value, wchar_seq_t, 5);
    EXPECT_EQ(int8_seq_len, 5u);
    EXPECT_ARRAY_EQ(int8_seq_value, int8_seq_t, 5);
    EXPECT_EQ(ushort_seq_len, 5u);
    EXPECT_ARRAY_EQ(ushort_seq_value, ushort_seq_t, 5);
    EXPECT_EQ(short_seq_len, 5u);
    EXPECT_ARRAY_EQ(short_seq_value, short_seq_t, 5);
    EXPECT_EQ(ulong_seq_len, 5u);
    EXPECT_ARRAY_EQ(ulong_seq_value, ulong_seq_t, 5);
    EXPECT_EQ(long_seq_len, 5u);
    EXPECT_ARRAY_EQ(long_seq_value, long_seq_t, 5);
    EXPECT_EQ(ulonglong_seq_len, 5u);
    EXPECT_ARRAY_EQ(ulonglong_seq_value, ulonglong_seq_t, 5);
    EXPECT_EQ(longlong_seq_len, 5u);
    EXPECT_ARRAY_EQ(longlong_seq_value, longlong_seq_t, 5);
    EXPECT_EQ(float_seq_len, 5u);
    EXPECT_ARRAY_FLOAT_EQ(float_seq_value, float_seq_t, 5);
    EXPECT_EQ(double_seq_len, 5u);
    EXPECT_ARRAY_DOUBLE_EQ(double_seq_value, double_seq_t, 5);
    EXPECT_EQ(ldouble_seq_len, 5u);
    EXPECT_ARRAY_LONG_DOUBLE_EQ(ldouble_seq_value, ldouble_seq_t, 5);
    EXPECT_EQ(wstring_seq_len, 5u);
    EXPECT_ARRAY_EQ(wstring_seq_value, wstring_seq_t, 5);
    EXPECT_EQ(string_seq_len, 5u);
    EXPECT_ARRAY_EQ(string_seq_value, string_seq_t, 5);

    EXPECT_EQ(strcmp(c_string_t, c_string_value), 0);
    EXPECT_EQ(wcscmp(c_wstring_t, c_wstring_value), 0);

    delete[] wstring_seq_value;

    delete[] string_seq_value;

    free(octet_seq_value);
    free(char_seq_value);
    free(wchar_seq_value);
    free(int8_seq_value);
    free(ushort_seq_value);
    free(short_seq_value);
    free(ulong_seq_value);
    free(long_seq_value);
    free(ulonglong_seq_value);
    free(longlong_seq_value);
    free(float_seq_value);
    free(double_seq_value);
    free(ldouble_seq_value);
    free(c_string_value);
    free(c_wstring_value);
}

TEST(FastCDRTests, Octet)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << octet_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint8_t octet_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> octet_value;
    });

    EXPECT_EQ(octet_value, octet_t);
}

TEST(FastCDRTests, Char)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << char_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    char char_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> char_value;
    });

    EXPECT_EQ(char_value, char_t);
}

TEST(FastCDRTests, WChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << wchar;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    wchar_t char_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> char_value;
    });

    EXPECT_EQ(char_value, wchar);
}

TEST(FastCDRTests, Int8)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << int8;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int8_t int8_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> int8_value;
    });

    EXPECT_EQ(int8_value, int8);
}

TEST(FastCDRTests, UnsignedShort)
{
    FAST_CDR_STREAM_HANDLE_ERROR(ushort_t);
}

TEST(FastCDRTests, Short)
{
    FAST_CDR_STREAM_HANDLE_ERROR(short_t);
}

TEST(FastCDRTests, UnsignedLong)
{
    FAST_CDR_STREAM_HANDLE_ERROR(ulong_t);
}

TEST(FastCDRTests, Long)
{
    FAST_CDR_STREAM_HANDLE_ERROR(long_t);
}

TEST(FastCDRTests, UnsignedLongLong)
{
    FAST_CDR_STREAM_HANDLE_ERROR(ulonglong_t);
}

TEST(FastCDRTests, LongLong)
{
    FAST_CDR_STREAM_HANDLE_ERROR(longlong_t);
}

TEST(FastCDRTests, Float)
{
    FAST_CDR_STREAM_HANDLE_ERROR(float_tt);
}

TEST(FastCDRTests, Double)
{
    FAST_CDR_STREAM_HANDLE_ERROR(double_tt);
}

TEST(FastCDRTests, LongDouble)
{
    FAST_CDR_STREAM_HANDLE_ERROR(ldouble_tt);
}

TEST(FastCDRTests, Boolean)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << bool_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    bool bool_value = false;

    EXPECT_NO_THROW(
    {
        cdr_des >> bool_value;
    });

    EXPECT_EQ(bool_value, bool_t);
}

TEST(FastCDRTests, String)
{
    FAST_CDR_STREAM_HANDLE_ERROR(string_t);
}

TEST(FastCDRTests, BinaryString)
{
    FAST_CDR_STREAM_HANDLE_ERROR(GET_BINARY_STRING());
}

TEST(FastCDRTests, WString)
{
    FAST_CDR_STREAM_HANDLE_ERROR(wstring_t);
}

TEST(FastCDRTests, BinaryWString)
{
    FAST_CDR_STREAM_HANDLE_ERROR(GET_BINARY_WSTRING());
}

TEST(FastCDRTests, EmptyString)
{
    FAST_CDR_STREAM_HANDLE_ERROR(emptystring_t);
}

TEST(FastCDRTests, EmptyWString)
{
    FAST_CDR_STREAM_HANDLE_ERROR(emptywstring_t);
}

TEST(FastCDRTests, STDArrayOctet)
{
    FAST_CDR_STREAM_HANDLE_ERROR(octet_array_t);
}

TEST(FastCDRTests, STDArrayChar)
{
    FAST_CDR_STREAM_HANDLE_ERROR(char_array_t);
}

TEST(FastCDRTests, STDArrayWChar)
{
    FAST_CDR_STREAM_HANDLE_ERROR(wchar_array_t);
}

TEST(FastCDRTests, STDArrayInt8)
{
    FAST_CDR_STREAM_HANDLE_ERROR(int8_array_t);
}

TEST(FastCDRTests, STDArrayUnsignedShort)
{
    FAST_CDR_STREAM_HANDLE_ERROR(ushort_array_t);
}

TEST(FastCDRTests, STDArrayShort)
{
    FAST_CDR_STREAM_HANDLE_ERROR(short_array_t);
}

TEST(FastCDRTests, STDArrayUnsignedLong)
{
    FAST_CDR_STREAM_HANDLE_ERROR(ulong_array_t);
}

TEST(FastCDRTests, STDArrayLong)
{
    FAST_CDR_STREAM_HANDLE_ERROR(long_array_t);
}

TEST(FastCDRTests, STDArrayUnsignedLongLong)
{
    FAST_CDR_STREAM_HANDLE_ERROR(ulonglong_array_t);
}

TEST(FastCDRTests, STDArrayLongLong)
{
    FAST_CDR_STREAM_HANDLE_ERROR(longlong_array_t);
}

TEST(FastCDRTests, STDArrayFloat)
{
    FAST_CDR_STREAM_HANDLE_ERROR(float_array_t);
}

TEST(FastCDRTests, STDArrayDouble)
{
    FAST_CDR_STREAM_HANDLE_ERROR(double_array_t);
}

TEST(FastCDRTests, STDArrayLongDouble)
{
    FAST_CDR_STREAM_HANDLE_ERROR(ldouble_array_t);
}

TEST(FastCDRTests, STDArrayBoolean)
{
    FAST_CDR_STREAM_HANDLE_ERROR(bool_array_t);
}

TEST(FastCDRTests, STDArrayString)
{
    FAST_CDR_STREAM_HANDLE_ERROR(string_array_t);
}

TEST(FastCDRTests, STDArrayWString)
{
    FAST_CDR_STREAM_HANDLE_ERROR(wstring_array_t);
}

TEST(FastCDRTests, ArrayOctet)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(octet_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint8_t octet_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(octet_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(octet_array_2_value, octet_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(octet_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(octet_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(char_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    char char_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(char_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(char_array_2_value, char_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(char_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(char_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayWChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(wchar_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    wchar_t char_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(char_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(char_array_2_value, wchar_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(wchar_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(char_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayInt8)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(int8_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int8_t int8_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(int8_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(int8_array_2_value, int8_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(int8_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(int8_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayUnsignedShort)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(ushort_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint16_t ushort_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(ushort_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(ushort_array_2_value, ushort_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(ushort_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(ushort_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayShort)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(short_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int16_t short_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(short_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(short_array_2_value, short_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(short_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(short_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayUnsignedLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(ulong_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint32_t ulong_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(ulong_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(ulong_array_2_value, ulong_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(ulong_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(ulong_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(long_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int32_t long_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(long_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(long_array_2_value, long_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(long_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(long_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayUnsignedLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(ulonglong_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint64_t ulonglong_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(ulonglong_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(ulonglong_array_2_value, ulonglong_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(ulonglong_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(ulonglong_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(longlong_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int64_t longlong_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(longlong_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(longlong_array_2_value, longlong_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(longlong_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(longlong_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayFloat)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(float_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    float float_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(float_array_2_value, 5);
    });

    EXPECT_ARRAY_FLOAT_EQ(float_array_2_value, float_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(float_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(float_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(double_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    double double_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(double_array_2_value, 5);
    });

    EXPECT_ARRAY_DOUBLE_EQ(double_array_2_value, double_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(double_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(double_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayLongDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(ldouble_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    long double ldouble_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(ldouble_array_2_value, 5);
    });

    EXPECT_ARRAY_LONG_DOUBLE_EQ(ldouble_array_2_value, ldouble_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(ldouble_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(ldouble_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayBoolean)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(bool_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    bool bool_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(bool_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(bool_array_2_value, bool_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(bool_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(bool_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(string_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::string string_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(string_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(string_array_2_value, string_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(string_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(string_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, ArrayWString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeArray(wstring_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::wstring string_array_2_value[5];

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeArray(string_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(string_array_2_value, wstring_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeArray(wstring_array_2_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeArray(string_array_2_value, 5);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorOctet)
{
    FAST_CDR_STREAM_HANDLE_ERROR(octet_vector_t);
}

TEST(FastCDRTests, STDVectorChar)
{
    FAST_CDR_STREAM_HANDLE_ERROR(char_vector_t);
}

TEST(FastCDRTests, STDVectorWChar)
{
    FAST_CDR_STREAM_HANDLE_ERROR(wchar_vector_t);
}

TEST(FastCDRTests, STDVectorInt8)
{
    FAST_CDR_STREAM_HANDLE_ERROR(int8_vector_t);
}

TEST(FastCDRTests, STDVectorUnsignedShort)
{
    FAST_CDR_STREAM_HANDLE_ERROR(ushort_vector_t);
}

TEST(FastCDRTests, STDVectorShort)
{
    FAST_CDR_STREAM_HANDLE_ERROR(short_vector_t);
}

TEST(FastCDRTests, STDVectorUnsignedLong)
{
    FAST_CDR_STREAM_HANDLE_ERROR(ulong_vector_t);
}

TEST(FastCDRTests, STDVectorLong)
{
    FAST_CDR_STREAM_HANDLE_ERROR(long_vector_t);
}

TEST(FastCDRTests, STDVectorUnsignedLongLong)
{
    FAST_CDR_STREAM_HANDLE_ERROR(ulonglong_vector_t);
}

TEST(FastCDRTests, STDVectorLongLong)
{
    FAST_CDR_STREAM_HANDLE_ERROR(longlong_vector_t);
}

TEST(FastCDRTests, STDVectorFloat)
{
    FAST_CDR_STREAM_HANDLE_ERROR(float_vector_t);
}

TEST(FastCDRTests, STDVectorDouble)
{
    FAST_CDR_STREAM_HANDLE_ERROR(double_vector_t);
}

TEST(FastCDRTests, STDVectorLongDouble)
{
    FAST_CDR_STREAM_HANDLE_ERROR(ldouble_vector_t);
}

TEST(FastCDRTests, STDVectorBoolean)
{
    FAST_CDR_STREAM_HANDLE_ERROR(bool_vector_t);
}

TEST(FastCDRTests, STDVectorString)
{
    FAST_CDR_STREAM_HANDLE_ERROR(string_vector_t);
}

TEST(FastCDRTests, STDVectorWString)
{
    FAST_CDR_STREAM_HANDLE_ERROR(wstring_vector_t);
}

TEST(FastCDRTests, STDTripleArrayUnsignedLong)
{
    FAST_CDR_STREAM_HANDLE_ERROR(triple_ulong_array_t);
}

TEST(FastCDRTests, SequenceOctet)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(octet_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint8_t *octet_seq_value = NULL; size_t octet_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(octet_seq_value, octet_seq_len);
    });

    EXPECT_EQ(octet_seq_len, 5u);
    EXPECT_ARRAY_EQ(octet_seq_value, octet_seq_t, octet_seq_len);

    free(octet_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(octet_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(octet_seq_value, octet_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(char_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    char *char_seq_value = NULL; size_t char_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(char_seq_value, char_seq_len);
    });

    EXPECT_EQ(char_seq_len, 5u);
    EXPECT_ARRAY_EQ(char_seq_value, char_seq_t, char_seq_len);

    free(char_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(char_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(char_seq_value, char_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceWChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(wchar_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    wchar_t *char_seq_value = NULL; size_t char_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(char_seq_value, char_seq_len);
    });

    EXPECT_EQ(char_seq_len, 5u);
    EXPECT_ARRAY_EQ(char_seq_value, wchar_seq_t, char_seq_len);

    free(char_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(wchar_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(char_seq_value, char_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceInt8)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(int8_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int8_t *int8_seq_value = NULL; size_t int8_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(int8_seq_value, int8_seq_len);
    });

    EXPECT_EQ(int8_seq_len, 5u);
    EXPECT_ARRAY_EQ(int8_seq_value, int8_seq_t, int8_seq_len);

    free(int8_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(int8_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(int8_seq_value, int8_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceUnsignedShort)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(ushort_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint16_t *ushort_seq_value = NULL; size_t ushort_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(ushort_seq_value, ushort_seq_len);
    });

    EXPECT_EQ(ushort_seq_len, 5u);
    EXPECT_ARRAY_EQ(ushort_seq_value, ushort_seq_t, ushort_seq_len);

    free(ushort_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(ushort_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(ushort_seq_value, ushort_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceShort)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(short_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int16_t *short_seq_value = NULL; size_t short_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(short_seq_value, short_seq_len);
    });

    EXPECT_EQ(short_seq_len, 5u);
    EXPECT_ARRAY_EQ(short_seq_value, short_seq_t, short_seq_len);

    free(short_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(short_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(short_seq_value, short_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceUnsignedLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(ulong_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint32_t *ulong_seq_value = NULL; size_t ulong_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(ulong_seq_value, ulong_seq_len);
    });

    EXPECT_EQ(ulong_seq_len, 5u);
    EXPECT_ARRAY_EQ(ulong_seq_value, ulong_seq_t, ulong_seq_len);

    free(ulong_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(ulong_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(ulong_seq_value, ulong_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(long_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int32_t *long_seq_value = NULL; size_t long_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(long_seq_value, long_seq_len);
    });

    EXPECT_EQ(long_seq_len, 5u);
    EXPECT_ARRAY_EQ(long_seq_value, long_seq_t, long_seq_len);

    free(long_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(long_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(long_seq_value, long_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceUnsignedLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(ulonglong_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint64_t *ulonglong_seq_value = NULL; size_t ulonglong_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(ulonglong_seq_value, ulonglong_seq_len);
    });

    EXPECT_EQ(ulonglong_seq_len, 5u);
    EXPECT_ARRAY_EQ(ulonglong_seq_value, ulonglong_seq_t, ulonglong_seq_len);

    free(ulonglong_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(ulonglong_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(ulonglong_seq_value, ulonglong_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(longlong_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int64_t *longlong_seq_value = NULL; size_t longlong_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(longlong_seq_value, longlong_seq_len);
    });

    EXPECT_EQ(longlong_seq_len, 5u);
    EXPECT_ARRAY_EQ(longlong_seq_value, longlong_seq_t, longlong_seq_len);

    free(longlong_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(longlong_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(longlong_seq_value, longlong_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceFloat)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(float_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    float *float_seq_value = NULL; size_t float_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(float_seq_value, float_seq_len);
    });

    EXPECT_EQ(float_seq_len, 5u);
    EXPECT_ARRAY_FLOAT_EQ(float_seq_value, float_seq_t, float_seq_len);

    free(float_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(float_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(float_seq_value, float_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(double_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    double *double_seq_value = NULL; size_t double_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(double_seq_value, double_seq_len);
    });

    EXPECT_EQ(double_seq_len, 5u);
    EXPECT_ARRAY_DOUBLE_EQ(double_seq_value, double_seq_t, double_seq_len);

    free(double_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(double_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(double_seq_value, double_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceLongDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(ldouble_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    long double *ldouble_seq_value = NULL; size_t ldouble_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(ldouble_seq_value, ldouble_seq_len);
    });

    EXPECT_EQ(ldouble_seq_len, 5u);
    EXPECT_ARRAY_LONG_DOUBLE_EQ(ldouble_seq_value, ldouble_seq_t, ldouble_seq_len);

    free(ldouble_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(ldouble_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(ldouble_seq_value, ldouble_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceBoolean)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(bool_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    bool *bool_seq_value = NULL; size_t bool_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(bool_seq_value, bool_seq_len);
    });

    EXPECT_EQ(bool_seq_len, 5u);
    EXPECT_ARRAY_EQ(bool_seq_value, bool_seq_t, bool_seq_len);

    free(bool_seq_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(bool_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(bool_seq_value, bool_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(string_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::string *string_seq_value = NULL; size_t string_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(string_seq_value, string_seq_len);
    });

    EXPECT_EQ(string_seq_len, 5u);
    EXPECT_ARRAY_EQ(string_seq_value, string_seq_t, string_seq_len);

    delete[] string_seq_value;

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(string_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(string_seq_value, string_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, SequenceWString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serializeSequence(wstring_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::wstring *string_seq_value = NULL; size_t string_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserializeSequence(string_seq_value, string_seq_len);
    });

    EXPECT_EQ(string_seq_len, 5u);
    EXPECT_ARRAY_EQ(string_seq_value, wstring_seq_t, string_seq_len);

    delete[] string_seq_value;

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serializeSequence(wstring_seq_t, 5);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserializeSequence(string_seq_value, string_seq_len);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, CString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serialize(c_string_t);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    char *c_string_value = NULL;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize(c_string_value);
    });

    EXPECT_EQ(strcmp(c_string_value, c_string_t) , 0);

    free(c_string_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize(c_string_t);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize(c_string_value);
    },
    NotEnoughMemoryException);
}

TEST(FastCDRTests, CWString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser.serialize(c_wstring_t);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    wchar_t *c_string_value = NULL;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize(c_string_value);
    });

    EXPECT_EQ(wcscmp(c_string_value, c_wstring_t) , 0);

    free(c_string_value);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize(c_wstring_t);
    },
    NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize(c_string_value);
    },
    NotEnoughMemoryException);
}


TEST(FastCDRTests, Complete)
{
    char buffer[BUFFER_LENGTH * 2];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH * 2);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << octet_t << char_t << int8 << ushort_t << short_t << ulong_t << long_t << ulonglong_t << longlong_t <<
            float_tt << double_tt << ldouble_tt << bool_t << string_t;
        cdr_ser << octet_array_t << char_array_t << int8_array_t << ushort_array_t << short_array_t << ulong_array_t <<
            long_array_t << ulonglong_array_t << longlong_array_t << float_array_t << double_array_t << ldouble_array_t << bool_array_t;
        cdr_ser.serializeArray(octet_array_2_t, 5).serializeArray(char_array_2_t, 5).serializeArray(int8_array_2_t, 5).
            serializeArray(ushort_array_2_t, 5).serializeArray(short_array_2_t, 5).serializeArray(ulong_array_2_t, 5).
            serializeArray(long_array_2_t, 5).serializeArray(ulonglong_array_2_t, 5).serializeArray(longlong_array_2_t, 5).
            serializeArray(float_array_2_t, 5).serializeArray(double_array_2_t, 5).serializeArray(ldouble_array_2_t, 5).
            serializeArray(bool_array_2_t, 5);
        cdr_ser << octet_vector_t << char_vector_t << int8_vector_t << ushort_vector_t << short_vector_t << ulong_vector_t <<
            long_vector_t << ulonglong_vector_t << longlong_vector_t << float_vector_t << double_vector_t <<
            ldouble_vector_t << bool_vector_t;
        cdr_ser << triple_ulong_array_t;
        cdr_ser.serializeSequence(octet_seq_t, 5).serializeSequence(char_seq_t, 5).serializeSequence(int8_seq_t, 5).
            serializeSequence(ushort_seq_t, 5).serializeSequence(short_seq_t, 5).serializeSequence(ulong_seq_t, 5).
            serializeSequence(long_seq_t, 5).serializeSequence(ulonglong_seq_t, 5).serializeSequence(longlong_seq_t, 5).
            serializeSequence(float_seq_t, 5).serializeSequence(double_seq_t, 5).serializeSequence(ldouble_seq_t, 5);
        cdr_ser.serialize(c_string_t);
        // Wstring and wchar
        cdr_ser << wchar << wstring_t;
        cdr_ser.serializeArray(wchar_array_2_t, 5).serializeArray(wstring_array_2_t, 5);
        cdr_ser << wchar_array_t << wstring_array_t;
        cdr_ser << wchar_vector_t << wstring_vector_t;
        cdr_ser.serializeSequence(wchar_seq_t, 5).serializeSequence(wstring_seq_t, 5);
        cdr_ser.serialize(c_wstring_t);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint8_t octet_value = 0;
    char char_value = 0;
    wchar_t wchar_value = 0;
    int8_t int8_value = 0;
    uint16_t ushort_value =0;
    int16_t short_value = 0;
    uint32_t ulong_value = 0;
    int32_t long_value = 0;
    uint64_t ulonglong_value = 0;
    int64_t longlong_value = 0;
    float float_value = 0;
    double double_value = 0;
    long double ldouble_value = 0;
    bool bool_value = false;
    std::string string_value = "";
    std::wstring wstring_value = L"";
    std::array<uint8_t, 5> octet_array_value;
    uint8_t octet_array_2_value[5];
    std::array<char, 5> char_array_value;
    std::array<wchar_t, 5> wchar_array_value;
    char char_array_2_value[5];
    wchar_t wchar_array_2_value[5];
    std::array<int8_t, 5> int8_array_value;
    int8_t int8_array_2_value[5];
    std::array<uint16_t, 5> ushort_array_value;
    uint16_t ushort_array_2_value[5];
    std::array<int16_t, 5> short_array_value;
    int16_t short_array_2_value[5];
    std::array<uint32_t, 5> ulong_array_value;
    uint32_t ulong_array_2_value[5];
    std::array<int32_t, 5> long_array_value;
    int32_t long_array_2_value[5];
    std::array<uint64_t, 5> ulonglong_array_value;
    uint64_t ulonglong_array_2_value[5];
    std::array<int64_t, 5> longlong_array_value;
    int64_t longlong_array_2_value[5];
    std::array<float, 5> float_array_value;
    float float_array_2_value[5];
    std::array<double, 5> double_array_value;
    double double_array_2_value[5];
    std::array<long double, 5> ldouble_array_value;
    long double ldouble_array_2_value[5];
    std::array<bool, 5> bool_array_value;
    bool bool_array_2_value[5];
    std::vector<uint8_t> octet_vector_value;
    std::vector<char> char_vector_value;
    std::vector<wchar_t> wchar_vector_value;
    std::vector<int8_t> int8_vector_value;
    std::vector<uint16_t> ushort_vector_value;
    std::vector<int16_t> short_vector_value;
    std::vector<uint32_t> ulong_vector_value;
    std::vector<int32_t> long_vector_value;
    std::vector<uint64_t> ulonglong_vector_value;
    std::vector<int64_t> longlong_vector_value;
    std::vector<float> float_vector_value;
    std::vector<double> double_vector_value;
    std::vector<long double> ldouble_vector_value;
    std::vector<bool> bool_vector_value;
    std::array<std::array<std::array<uint32_t, 3>, 2>, 2> triple_ulong_array_value;
    uint8_t *octet_seq_value = NULL; size_t octet_seq_len;
    char *char_seq_value = NULL; size_t char_seq_len;
    wchar_t *wchar_seq_value = NULL; size_t wchar_seq_len;
    int8_t *int8_seq_value = NULL; size_t int8_seq_len;
    uint16_t *ushort_seq_value = NULL; size_t ushort_seq_len;
    int16_t *short_seq_value = NULL; size_t short_seq_len;
    uint32_t *ulong_seq_value = NULL; size_t ulong_seq_len;
    int32_t *long_seq_value = NULL; size_t long_seq_len;
    uint64_t *ulonglong_seq_value = NULL; size_t ulonglong_seq_len;
    int64_t *longlong_seq_value = NULL; size_t longlong_seq_len;
    float *float_seq_value = NULL; size_t float_seq_len;
    double *double_seq_value = NULL; size_t double_seq_len;
    long double *ldouble_seq_value = NULL; size_t ldouble_seq_len;
    char *c_string_value = NULL;
    wchar_t *c_wstring_value = NULL;
    std::array<std::wstring, 5> wstring_array_value;
    std::wstring wstring_array_2_value[5];
    std::vector<std::wstring> wstring_vector_value;
    std::wstring *wstring_seq_value = NULL; size_t wstring_seq_len;

    EXPECT_NO_THROW(
    {
        cdr_des >> octet_value >> char_value >> int8_value >> ushort_value >> short_value >> ulong_value >> long_value >>
            ulonglong_value >> longlong_value >> float_value >> double_value >> ldouble_value >> bool_value >> string_value;
        cdr_des >> octet_array_value >> char_array_value >> int8_array_value >> ushort_array_value >> short_array_value >>
            ulong_array_value >> long_array_value >> ulonglong_array_value >> longlong_array_value >> float_array_value >>
            double_array_value >> ldouble_array_value >> bool_array_value;
        cdr_des.deserializeArray(octet_array_2_value, 5).deserializeArray(char_array_2_value, 5).deserializeArray(int8_array_2_value, 5).
            deserializeArray(ushort_array_2_value, 5).deserializeArray(short_array_2_value, 5).deserializeArray(ulong_array_2_value, 5).
            deserializeArray(long_array_2_value, 5).deserializeArray(ulonglong_array_2_value, 5).deserializeArray(longlong_array_2_value, 5).
            deserializeArray(float_array_2_value, 5).deserializeArray(double_array_2_value, 5).deserializeArray(ldouble_array_2_value, 5).deserializeArray(bool_array_2_value, 5);
        cdr_des >> octet_vector_value >> char_vector_value >> int8_vector_value >> ushort_vector_value >> short_vector_value >>
            ulong_vector_value >> long_vector_value >> ulonglong_vector_value >> longlong_vector_value >> float_vector_value >>
            double_vector_value >> ldouble_vector_value >> bool_vector_value;
        cdr_des >> triple_ulong_array_value;
        cdr_des.deserializeSequence(octet_seq_value, octet_seq_len).deserializeSequence(char_seq_value, char_seq_len).
            deserializeSequence(int8_seq_value, int8_seq_len).deserializeSequence(ushort_seq_value, ushort_seq_len).
            deserializeSequence(short_seq_value, short_seq_len).deserializeSequence(ulong_seq_value, ulong_seq_len).
            deserializeSequence(long_seq_value, long_seq_len).deserializeSequence(ulonglong_seq_value, ulonglong_seq_len).
            deserializeSequence(longlong_seq_value, longlong_seq_len).deserializeSequence(float_seq_value, float_seq_len).
            deserializeSequence(double_seq_value, double_seq_len).deserializeSequence(ldouble_seq_value, ldouble_seq_len);
        cdr_des.deserialize(c_string_value);
        // WString and wchar_t
        cdr_des >> wchar_value >> wstring_value;
        cdr_des.deserializeArray(wchar_array_2_value, 5).deserializeArray(wstring_array_2_value, 5);
        cdr_des >> wchar_array_value >> wstring_array_value;
        cdr_des >> wchar_vector_value >> wstring_vector_value;
        cdr_des.deserializeSequence(wchar_seq_value, wchar_seq_len).deserializeSequence(wstring_seq_value, wstring_seq_len);
        cdr_des.deserialize(c_wstring_value);
    });

    EXPECT_EQ(octet_value, octet_t);
    EXPECT_EQ(char_value, char_t);
    EXPECT_EQ(wchar_value, wchar);
    EXPECT_EQ(int8_value, int8);
    EXPECT_EQ(ushort_value, ushort_t);
    EXPECT_EQ(short_value, short_t);
    EXPECT_EQ(ulong_value, ulong_t);
    EXPECT_EQ(long_value, long_t);
    EXPECT_EQ(ulonglong_value, ulonglong_t);
    EXPECT_EQ(longlong_value, longlong_t);
    EXPECT_FLOAT_EQ(float_value, float_tt);
    EXPECT_DOUBLE_EQ(double_value, double_tt);
    EXPECT_LONG_DOUBLE_EQ(ldouble_value, ldouble_tt);
    EXPECT_EQ(bool_value, bool_t);
    EXPECT_EQ(string_value, string_t);
    EXPECT_EQ(wstring_value, wstring_t);

    EXPECT_EQ(octet_array_value, octet_array_t);
    EXPECT_EQ(char_array_value, char_array_t);
    EXPECT_EQ(wchar_array_value, wchar_array_t);
    EXPECT_EQ(int8_array_value, int8_array_t);
    EXPECT_EQ(ushort_array_value, ushort_array_t);
    EXPECT_EQ(short_array_value, short_array_t);
    EXPECT_EQ(ulong_array_value, ulong_array_t);
    EXPECT_EQ(long_array_value, long_array_t);
    EXPECT_EQ(ulonglong_array_value, ulonglong_array_t);
    EXPECT_EQ(longlong_array_value, longlong_array_t);
    EXPECT_EQ(float_array_value, float_array_t);
    EXPECT_EQ(double_array_value, double_array_t);
    EXPECT_EQ(ldouble_array_value, ldouble_array_t);
    EXPECT_EQ(bool_array_value, bool_array_t);
    EXPECT_EQ(wstring_array_value, wstring_array_t);

    EXPECT_ARRAY_EQ(octet_array_2_value, octet_array_2_t, 5);
    EXPECT_ARRAY_EQ(char_array_2_value, char_array_2_t, 5);
    EXPECT_ARRAY_EQ(wchar_array_2_value, wchar_array_2_t, 5);
    EXPECT_ARRAY_EQ(int8_array_2_value, int8_array_2_t, 5);
    EXPECT_ARRAY_EQ(ushort_array_2_value, ushort_array_2_t, 5);
    EXPECT_ARRAY_EQ(short_array_2_value, short_array_2_t, 5);
    EXPECT_ARRAY_EQ(ulong_array_2_value, ulong_array_2_t, 5);
    EXPECT_ARRAY_EQ(long_array_2_value, long_array_2_t, 5);
    EXPECT_ARRAY_EQ(ulonglong_array_2_value, ulonglong_array_2_t, 5);
    EXPECT_ARRAY_EQ(longlong_array_2_value, longlong_array_2_t, 5);
    EXPECT_ARRAY_FLOAT_EQ(float_array_2_value, float_array_2_t, 5);
    EXPECT_ARRAY_DOUBLE_EQ(double_array_2_value, double_array_2_t, 5);
    EXPECT_ARRAY_LONG_DOUBLE_EQ(ldouble_array_2_value, ldouble_array_2_t, 5);
    EXPECT_ARRAY_EQ(bool_array_2_value, bool_array_2_t, 5);
    EXPECT_ARRAY_EQ(wstring_array_2_value, wstring_array_2_t, 5);

    EXPECT_EQ(octet_vector_value, octet_vector_t);
    EXPECT_EQ(char_vector_value, char_vector_t);
    EXPECT_EQ(wchar_vector_value, wchar_vector_t);
    EXPECT_EQ(int8_vector_value, int8_vector_t);
    EXPECT_EQ(ushort_vector_value, ushort_vector_t);
    EXPECT_EQ(short_vector_value, short_vector_t);
    EXPECT_EQ(ulong_vector_value, ulong_vector_t);
    EXPECT_EQ(long_vector_value, long_vector_t);
    EXPECT_EQ(ulonglong_vector_value, ulonglong_vector_t);
    EXPECT_EQ(longlong_vector_value, longlong_vector_t);
    EXPECT_EQ(float_vector_value, float_vector_t);
    EXPECT_EQ(double_vector_value, double_vector_t);
    EXPECT_EQ(ldouble_vector_value, ldouble_vector_t);
    EXPECT_EQ(bool_vector_value, bool_vector_t);
    EXPECT_EQ(wstring_vector_value, wstring_vector_t);

    EXPECT_EQ(triple_ulong_array_t, triple_ulong_array_value);
    EXPECT_EQ(octet_seq_len, 5u);
    EXPECT_ARRAY_EQ(octet_seq_value, octet_seq_t, 5);
    EXPECT_EQ(char_seq_len, 5u);
    EXPECT_ARRAY_EQ(char_seq_value, char_seq_t, 5);
    EXPECT_EQ(wchar_seq_len, 5u);
    EXPECT_ARRAY_EQ(wchar_seq_value, wchar_seq_t, 5);
    EXPECT_EQ(int8_seq_len, 5u);
    EXPECT_ARRAY_EQ(int8_seq_value, int8_seq_t, 5);
    EXPECT_EQ(ushort_seq_len, 5u);
    EXPECT_ARRAY_EQ(ushort_seq_value, ushort_seq_t, 5);
    EXPECT_EQ(short_seq_len, 5u);
    EXPECT_ARRAY_EQ(short_seq_value, short_seq_t, 5);
    EXPECT_EQ(ulong_seq_len, 5u);
    EXPECT_ARRAY_EQ(ulong_seq_value, ulong_seq_t, 5);
    EXPECT_EQ(long_seq_len, 5u);
    EXPECT_ARRAY_EQ(long_seq_value, long_seq_t, 5);
    EXPECT_EQ(ulonglong_seq_len, 5u);
    EXPECT_ARRAY_EQ(ulonglong_seq_value, ulonglong_seq_t, 5);
    EXPECT_EQ(longlong_seq_len, 5u);
    EXPECT_ARRAY_EQ(longlong_seq_value, longlong_seq_t, 5);
    EXPECT_EQ(float_seq_len, 5u);
    EXPECT_ARRAY_FLOAT_EQ(float_seq_value, float_seq_t, 5);
    EXPECT_EQ(double_seq_len, 5u);
    EXPECT_ARRAY_DOUBLE_EQ(double_seq_value, double_seq_t, 5);
    EXPECT_EQ(ldouble_seq_len, 5u);
    EXPECT_ARRAY_LONG_DOUBLE_EQ(ldouble_seq_value, ldouble_seq_t, 5);
    EXPECT_EQ(wstring_seq_len, 5u);
    EXPECT_ARRAY_EQ(wstring_seq_value, wstring_seq_t, 5);

    EXPECT_EQ(strcmp(c_string_t, c_string_value), 0);
    EXPECT_EQ(wcscmp(c_wstring_t, c_wstring_value), 0);

    delete[] wstring_seq_value;

    free(octet_seq_value);
    free(char_seq_value);
    free(wchar_seq_value);
    free(int8_seq_value);
    free(ushort_seq_value);
    free(short_seq_value);
    free(ulong_seq_value);
    free(long_seq_value);
    free(ulonglong_seq_value);
    free(longlong_seq_value);
    free(float_seq_value);
    free(double_seq_value);
    free(ldouble_seq_value);
    free(c_string_value);
    free(c_wstring_value);
}

// Regression test
TEST(CDRTests, ZeroSequenceAtTheEnd)
{
    char buffer[12];
    std::vector<uint8_t> octet_zero_sequence;
    std::vector<char> char_zero_sequence;
    std::vector<int8_t> int8_zero_sequence;
    std::vector<uint16_t> ushort_zero_sequence;
    std::vector<int16_t> short_zero_sequence;
    std::vector<uint32_t> ulong_zero_sequence;
    std::vector<int32_t> long_zero_sequence;
    std::vector<uint64_t> ulonglong_zero_sequence;
    std::vector<int64_t> longlong_zero_sequence;
    std::vector<float> float_zero_sequence;
    std::vector<double> double_zero_sequence;
    std::vector<long double> longdouble_zero_sequence;
    std::vector<bool> bool_zero_sequence;

    // Serialization.
    FastBuffer cdrbuffer(buffer, 12);
    Cdr cdr_ser_octet(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_octet << double_tt << octet_zero_sequence;
    });

    // Deserialization.
    Cdr cdr_des_octet(cdrbuffer);

    double value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des_octet >> value >> octet_zero_sequence;
    });

    Cdr cdr_ser_char(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_char << double_tt << char_zero_sequence;
    });

    // Deserialization.
    Cdr cdr_des_char(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_char >> value >> char_zero_sequence;
    });

    Cdr cdr_ser_int8(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_int8 << double_tt << int8_zero_sequence;
    });

    // Deserialization.
    Cdr cdr_des_int8(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_int8 >> value >> int8_zero_sequence;
    });

    Cdr cdr_ser_ushort(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_ushort << double_tt << ushort_zero_sequence;
    });

    // Deserialization.
    Cdr cdr_des_ushort(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_ushort >> value >> ushort_zero_sequence;
    });

    Cdr cdr_ser_short(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_short << double_tt << short_zero_sequence;
    });

    // Deserialization.
    Cdr cdr_des_short(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_short >> value >> short_zero_sequence;
    });

    Cdr cdr_ser_ulong(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_ulong << double_tt << ulong_zero_sequence;
    });

    // Deserialization.
    Cdr cdr_des_ulong(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_ulong >> value >> ulong_zero_sequence;
    });

    Cdr cdr_ser_long(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_long << double_tt << long_zero_sequence;
    });

    // Deserialization.
    Cdr cdr_des_long(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_long >> value >> long_zero_sequence;
    });

    Cdr cdr_ser_ulonglong(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_ulonglong << double_tt << ulonglong_zero_sequence;
    });

    // Deserialization.
    Cdr cdr_des_ulonglong(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_ulonglong >> value >> ulonglong_zero_sequence;
    });

    Cdr cdr_ser_float(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_float << double_tt << float_zero_sequence;
    });

    // Deserialization.
    Cdr cdr_des_float(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_float >> value >> float_zero_sequence;
    });

    Cdr cdr_ser_double(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_double << double_tt << double_zero_sequence;
    });

    // Deserialization.
    Cdr cdr_des_double(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_double >> value >> double_zero_sequence;
    });

    Cdr cdr_ser_longdouble(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_longdouble << double_tt << longdouble_zero_sequence;
    });

    // Deserialization.
    Cdr cdr_des_longdouble(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_longdouble >> value >> longdouble_zero_sequence;
    });

    Cdr cdr_ser_bool(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_bool << double_tt << bool_zero_sequence;
    });

    // Deserialization.
    Cdr cdr_des_bool(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_bool >> value >> bool_zero_sequence;
    });
}

TEST(FastCDRTests, ZeroSequenceAtTheEnd)
{
    char buffer[12];
    std::vector<uint8_t> octet_zero_sequence;
    std::vector<char> char_zero_sequence;
    std::vector<int8_t> int8_zero_sequence;
    std::vector<uint16_t> ushort_zero_sequence;
    std::vector<int16_t> short_zero_sequence;
    std::vector<uint32_t> ulong_zero_sequence;
    std::vector<int32_t> long_zero_sequence;
    std::vector<uint64_t> ulonglong_zero_sequence;
    std::vector<int64_t> longlong_zero_sequence;
    std::vector<float> float_zero_sequence;
    std::vector<double> double_zero_sequence;
    std::vector<long double> longdouble_zero_sequence;
    std::vector<bool> bool_zero_sequence;

    // Serialization.
    FastBuffer cdrbuffer(buffer, 12);
    FastCdr cdr_ser_octet(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_octet << double_tt << octet_zero_sequence;
    });

    // Deserialization.
    FastCdr cdr_des_octet(cdrbuffer);

    double value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des_octet >> value >> octet_zero_sequence;
    });

    FastCdr cdr_ser_char(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_char << double_tt << char_zero_sequence;
    });

    // Deserialization.
    FastCdr cdr_des_char(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_char >> value >> char_zero_sequence;
    });

    FastCdr cdr_ser_int8(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_int8 << double_tt << int8_zero_sequence;
    });

    // Deserialization.
    FastCdr cdr_des_int8(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_int8 >> value >> int8_zero_sequence;
    });

    FastCdr cdr_ser_ushort(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_ushort << double_tt << ushort_zero_sequence;
    });

    // Deserialization.
    FastCdr cdr_des_ushort(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_ushort >> value >> ushort_zero_sequence;
    });

    FastCdr cdr_ser_short(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_short << double_tt << short_zero_sequence;
    });

    // Deserialization.
    FastCdr cdr_des_short(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_short >> value >> short_zero_sequence;
    });

    FastCdr cdr_ser_ulong(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_ulong << double_tt << ulong_zero_sequence;
    });

    // Deserialization.
    FastCdr cdr_des_ulong(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_ulong >> value >> ulong_zero_sequence;
    });

    FastCdr cdr_ser_long(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_long << double_tt << long_zero_sequence;
    });

    // Deserialization.
    FastCdr cdr_des_long(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_long >> value >> long_zero_sequence;
    });

    FastCdr cdr_ser_ulonglong(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_ulonglong << double_tt << ulonglong_zero_sequence;
    });

    // Deserialization.
    FastCdr cdr_des_ulonglong(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_ulonglong >> value >> ulonglong_zero_sequence;
    });

    FastCdr cdr_ser_float(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_float << double_tt << float_zero_sequence;
    });

    // Deserialization.
    FastCdr cdr_des_float(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_float >> value >> float_zero_sequence;
    });

    FastCdr cdr_ser_double(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_double << double_tt << double_zero_sequence;
    });

    // Deserialization.
    FastCdr cdr_des_double(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_double >> value >> double_zero_sequence;
    });

    FastCdr cdr_ser_longdouble(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_longdouble << double_tt << longdouble_zero_sequence;
    });

    // Deserialization.
    FastCdr cdr_des_longdouble(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_longdouble >> value >> longdouble_zero_sequence;
    });

    FastCdr cdr_ser_bool(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser_bool << double_tt << bool_zero_sequence;
    });

    // Deserialization.
    FastCdr cdr_des_bool(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_des_bool >> value >> bool_zero_sequence;
    });
}
