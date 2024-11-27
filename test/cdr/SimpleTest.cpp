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

#include <fastcdr/exceptions/BadParamException.h>
#include <fastcdr/exceptions/Exception.h>
#include <fastcdr/exceptions/NotEnoughMemoryException.h>

#include <stdio.h>
#include <limits>
#include <iostream>

#include <gtest/gtest.h>

using namespace eprosima::fastcdr;
using namespace ::exception;

#define BUFFER_LENGTH 2000
#define N_ARR_ELEMENTS 5

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
static const std::array<uint8_t, N_ARR_ELEMENTS> octet_array_t = {{1, 2, 3, 4, 5}};
static const uint8_t octet_array_2_t[N_ARR_ELEMENTS] = {5, 4, 3, 2, 1};
static const std::array<char, N_ARR_ELEMENTS> char_array_t = {{'A', 'B', 'C', 'D', 'E'}};
static const std::array<wchar_t, N_ARR_ELEMENTS> wchar_array_t = {{'A', 'B', 'C', 'D', 'E'}};
static const char char_array_2_t[N_ARR_ELEMENTS] = {'E', 'D', 'C', 'B', 'A'};
static const wchar_t wchar_array_2_t[N_ARR_ELEMENTS] = {'E', 'D', 'C', 'B', 'A'};
static const std::array<int8_t, N_ARR_ELEMENTS> int8_array_t = {{6, 7, 8, 9, 10}};
static const int8_t int8_array_2_t[N_ARR_ELEMENTS] = {10, 9, 8, 7, 6};
static const std::array<uint16_t, N_ARR_ELEMENTS> ushort_array_t = {{65500, 65501, 65502, 65503, 65504}};
static const uint16_t ushort_array_2_t[N_ARR_ELEMENTS] = {65504, 65503, 65502, 65501, 65500};
static const std::array<int16_t, N_ARR_ELEMENTS> short_array_t = {{-32700, -32701, -32702, -32703, -32704}};
static const int16_t short_array_2_t[N_ARR_ELEMENTS] = {-32704, -32703, -32702, -32701, -32700};
static const std::array<uint32_t,
        N_ARR_ELEMENTS> ulong_array_t = {{4294967200, 4294967201, 4294967202, 4294967203, 4294967204}};
static const uint32_t ulong_array_2_t[N_ARR_ELEMENTS] = {4294967204, 4294967203, 4294967202, 4294967201, 4294967200};
static const std::array<int32_t,
        N_ARR_ELEMENTS> long_array_t = {{-2147483600, -2147483601, -2147483602, -2147483603, -2147483604}};
static const int32_t long_array_2_t[N_ARR_ELEMENTS] = {-2147483604, -2147483603, -2147483602, -2147483601, -2147483600};
static const std::array<uint64_t,
        5> ulonglong_array_t =
{{18446744073709551600u, 18446744073709551601u, 18446744073709551602u, 18446744073709551603u,
    18446744073709551604u}};
static const uint64_t ulonglong_array_2_t[N_ARR_ELEMENTS] =
{18446744073709551604u, 18446744073709551603u, 18446744073709551602u, 18446744073709551601u,
 18446744073709551600u};
static const std::array<int64_t,
        5> longlong_array_t = {{-9223372036800, -9223372036801, -9223372036802, -9223372036803, -9223372036804}};
static const int64_t longlong_array_2_t[N_ARR_ELEMENTS] =
{-9223372036804, -9223372036803, -9223372036802, -9223372036801, -9223372036800};
static const std::array<float,
        N_ARR_ELEMENTS> float_array_t = {{float_tt, float_tt + 1, float_tt + 2, float_tt + 3, float_tt + 4}};
static const float float_array_2_t[N_ARR_ELEMENTS] = {float_tt + 4, float_tt + 3, float_tt + 2, float_tt + 1, float_tt};
static const std::array<double,
        5> double_array_t = {{double_tt, double_tt + 1, double_tt + 2, double_tt + 3, double_tt + 4}};
static const double double_array_2_t[N_ARR_ELEMENTS] =
{double_tt + 4, double_tt + 3, double_tt + 2, double_tt + 1, double_tt};
static const std::array<long double,
        5> ldouble_array_t = {{ldouble_tt, ldouble_tt + 1, ldouble_tt + 2, ldouble_tt + 3, ldouble_tt + 4}};
static const long double ldouble_array_2_t[N_ARR_ELEMENTS] =
{ldouble_tt + 4, ldouble_tt + 3, ldouble_tt + 2, ldouble_tt + 1, ldouble_tt};
static const std::array<bool, N_ARR_ELEMENTS> bool_array_t = {{true, false, true, false, true}};
static const bool bool_array_2_t[N_ARR_ELEMENTS] = {false, true, false, true, false};
static const std::array<std::string, N_ARR_ELEMENTS> string_array_t = {{"HOLA", "ADIOS", "HELLO", "BYE", "GOODBYE"}};
static const std::array<std::wstring,
        N_ARR_ELEMENTS> wstring_array_t = {{L"HOLA", L"ADIOS", L"HELLO", L"BYE", L"GOODBYE"}};
static const std::string string_array_2_t[N_ARR_ELEMENTS] = {"HOLA", "ADIOS", "HELLO", "BYE", "GOODBYE"};
static const std::wstring wstring_array_2_t[N_ARR_ELEMENTS] = {L"HOLA", L"ADIOS", L"HELLO", L"BYE", L"GOODBYE"};
static const std::vector<uint8_t> octet_vector_t(octet_array_2_t,
        octet_array_2_t + sizeof(octet_array_2_t) / sizeof(uint8_t));
static const std::vector<char> char_vector_t(char_array_2_t, char_array_2_t + sizeof(char_array_2_t) / sizeof(char));
static const std::vector<wchar_t> wchar_vector_t(wchar_array_2_t,
        wchar_array_2_t + sizeof(wchar_array_2_t) / sizeof(wchar_t));
static const std::vector<int8_t> int8_vector_t(int8_array_2_t,
        int8_array_2_t + sizeof(int8_array_2_t) / sizeof(int8_t));
static const std::vector<uint16_t> ushort_vector_t(ushort_array_2_t,
        ushort_array_2_t + sizeof(ushort_array_2_t) / sizeof(uint16_t));
static const std::vector<int16_t> short_vector_t(short_array_2_t,
        short_array_2_t + sizeof(short_array_2_t) / sizeof(int16_t));
static const std::vector<uint32_t> ulong_vector_t(ulong_array_2_t,
        ulong_array_2_t + sizeof(ulong_array_2_t) / sizeof(uint32_t));
static const std::vector<int32_t> long_vector_t(long_array_2_t,
        long_array_2_t + sizeof(long_array_2_t) / sizeof(int32_t));
static const std::vector<uint64_t> ulonglong_vector_t(ulonglong_array_2_t,
        ulonglong_array_2_t + sizeof(ulonglong_array_2_t) /
        sizeof(uint64_t));
static const std::vector<int64_t> longlong_vector_t(longlong_array_2_t,
        longlong_array_2_t + sizeof(longlong_array_2_t) / sizeof(int64_t));
static const std::vector<float> float_vector_t(float_array_2_t,
        float_array_2_t + sizeof(float_array_2_t) / sizeof(float));
static const std::vector<double> double_vector_t(double_array_2_t,
        double_array_2_t + sizeof(double_array_2_t) / sizeof(double));
static const std::vector<long double> ldouble_vector_t(ldouble_array_2_t,
        ldouble_array_2_t + sizeof(ldouble_array_2_t) /
        sizeof(long double));
static const std::vector<bool> bool_vector_t(bool_array_2_t, bool_array_2_t + sizeof(bool_array_2_t) / sizeof(bool));
static const std::vector<std::string> string_vector_t(string_array_2_t,
        string_array_2_t + sizeof(string_array_2_t) /
        sizeof(std::string));
static const std::vector<std::wstring> wstring_vector_t(wstring_array_2_t,
        wstring_array_2_t + sizeof(wstring_array_2_t) /
        sizeof(std::wstring));
static const std::array<std::array<std::array<uint32_t, 3>, 2>,
        2> triple_ulong_array_t = {{ {{ {{1, 2, 3}}, {{4, 5, 6}} }}, {{ {{7, 8, 9}}, {{10, 11, 12}} }} }};
// Added because error 336.
static const uint8_t octet_seq_t[N_ARR_ELEMENTS] = {5, 4, 3, 2, 1};
static const char char_seq_t[N_ARR_ELEMENTS] = {'E', 'D', 'C', 'B', 'A'};
static const wchar_t wchar_seq_t[N_ARR_ELEMENTS] = {'E', 'D', 'C', 'B', 'A'};
static const int8_t int8_seq_t[N_ARR_ELEMENTS] = {10, 9, 8, 7, 6};
static const uint16_t ushort_seq_t[N_ARR_ELEMENTS] = {65504, 65503, 65502, 65501, 65500};
static const int16_t short_seq_t[N_ARR_ELEMENTS] = {-32704, -32703, -32702, -32701, -32700};
static const uint32_t ulong_seq_t[N_ARR_ELEMENTS] = {4294967204, 4294967203, 4294967202, 4294967201, 4294967200};
static const int32_t long_seq_t[N_ARR_ELEMENTS] = {-2147483604, -2147483603, -2147483602, -2147483601, -2147483600};
static const uint64_t ulonglong_seq_t[N_ARR_ELEMENTS] =
{18446744073709551604u, 18446744073709551603u, 18446744073709551602u, 18446744073709551601u,
 18446744073709551600u};
static const int64_t longlong_seq_t[N_ARR_ELEMENTS] =
{-9223372036804, -9223372036803, -9223372036802, -9223372036801, -9223372036800};
static const float float_seq_t[N_ARR_ELEMENTS] = {float_tt + 4, float_tt + 3, float_tt + 2, float_tt + 1, float_tt};
static const double double_seq_t[N_ARR_ELEMENTS] =
{double_tt + 4, double_tt + 3, double_tt + 2, double_tt + 1, double_tt};
static const long double ldouble_seq_t[N_ARR_ELEMENTS] =
{ldouble_tt + 4, ldouble_tt + 3, ldouble_tt + 2, ldouble_tt + 1, ldouble_tt};
static const bool bool_seq_t[N_ARR_ELEMENTS] = {true, true, false, false, true};
static const std::string string_seq_t[N_ARR_ELEMENTS] = {"HELLO", "BYE", "GOODBYE", "HOLA", "ADIOS"};
static const std::wstring wstring_seq_t[N_ARR_ELEMENTS] = {L"HELLO", L"BYE", L"GOODBYE", L"HOLA", L"ADIOS"};
// Added because error 337
static const char* c_string_t = "HOLA";
static const wchar_t* c_wstring_t = L"HOLA";

template<typename T>
void EXPECT_ARRAY_EQ(
        T* array1,
        const T* array2,
        size_t size)
{
    for (size_t count = 0; count < size; ++count)
    {
        EXPECT_EQ(array1[count], array2[count]);
    }
}

static void EXPECT_ARRAY_FLOAT_EQ(
        float* array1,
        const float* array2,
        size_t size)
{
    for (size_t count = 0; count < size; ++count)
    {
        EXPECT_FLOAT_EQ(array1[count], array2[count]);
    }
}

static void EXPECT_ARRAY_DOUBLE_EQ(
        double* array1,
        const double* array2,
        size_t size)
{
    for (size_t count = 0; count < size; ++count)
    {
        EXPECT_DOUBLE_EQ(array1[count], array2[count]);
    }
}

static void EXPECT_LONG_DOUBLE_EQ(
        const long double val1,
        const long double val2)
{
    EXPECT_TRUE(val1 == val2);
}

static void EXPECT_ARRAY_LONG_DOUBLE_EQ(
        long double* array1,
        const long double* array2,
        size_t size)
{
    for (size_t count = 0; count < size; ++count)
    {
        EXPECT_LONG_DOUBLE_EQ(array1[count], array2[count]);
    }
}

template<typename T>
static void check_bad_length_deserialization(
        const T& const_value)
{
    char buffer[sizeof(uint32_t) + 1];

    // Serialize wrong length (4GB)
    {
        FastBuffer buf(buffer, sizeof(buffer));
        Cdr cdr(buf);
        uint32_t bad_length = 0xFFFFFFFF;

        EXPECT_NO_THROW(cdr << bad_length);
    }

    // Deserializing should throw
    {
        FastBuffer buf(buffer, sizeof(buffer));
        Cdr cdr(buf);
        T value;

        EXPECT_THROW(cdr >> value, NotEnoughMemoryException);
        EXPECT_NE(value, const_value);
    }
}

template<typename T>
static void check_good_case(
        const T& input_value)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_ser(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::LITTLE_ENDIANNESS);
        EXPECT_NO_THROW(cdr_ser << input_value);
    }

    // Deserialization.
    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_des(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::LITTLE_ENDIANNESS);
        T output_value{};

        EXPECT_NO_THROW(cdr_des >> output_value);
        EXPECT_EQ(output_value, input_value);
    }

    // Serialization.
    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_ser(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);
        EXPECT_NO_THROW(cdr_ser << input_value);
    }

    // Deserialization.
    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_des(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);
        T output_value{};

        EXPECT_NO_THROW(cdr_des >> output_value);
        EXPECT_EQ(output_value, input_value);
    }
}

template<typename T>
static void check_no_space(
        const T& input_value,
        size_t buf_size = sizeof(T) - 1)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    {
        FastBuffer cdrbuffer(buffer, buf_size);
        Cdr cdr_ser(cdrbuffer);
        EXPECT_THROW(cdr_ser << input_value, NotEnoughMemoryException);
    }

    // Deserialization.
    {
        FastBuffer cdrbuffer(buffer, buf_size);
        Cdr cdr_des(cdrbuffer);
        T output_value;

        EXPECT_THROW(cdr_des >> output_value, NotEnoughMemoryException);
    }
}

static void check_no_space(
        const wchar_t& input_value,
        size_t buf_size = 1)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    {
        FastBuffer cdrbuffer(buffer, buf_size);
        Cdr cdr_ser(cdrbuffer);
        EXPECT_THROW(cdr_ser << input_value, NotEnoughMemoryException);
    }

    // Deserialization.
    {
        FastBuffer cdrbuffer(buffer, buf_size);
        Cdr cdr_des(cdrbuffer);
        wchar_t output_value;

        EXPECT_THROW(cdr_des >> output_value, NotEnoughMemoryException);
    }
}

template<size_t _N>
static void check_no_space(
        const std::array<wchar_t, _N>& input_value,
        size_t buf_size = sizeof(std::array<wchar_t, _N>) / 2 - 1)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    {
        FastBuffer cdrbuffer(buffer, buf_size);
        Cdr cdr_ser(cdrbuffer);
        EXPECT_THROW(cdr_ser << input_value, NotEnoughMemoryException);
    }

    // Deserialization.
    {
        FastBuffer cdrbuffer(buffer, buf_size);
        Cdr cdr_des(cdrbuffer);
        std::array<wchar_t, _N> output_value;

        EXPECT_THROW(cdr_des >> output_value, NotEnoughMemoryException);
    }
}

template<typename T, size_t N = N_ARR_ELEMENTS>
static void check_good_case_array(
        const T* input_value)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_ser(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::LITTLE_ENDIANNESS);
        EXPECT_NO_THROW(cdr_ser.serialize_array(input_value, N));
    }

    // Deserialization.
    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_des(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::LITTLE_ENDIANNESS);
        T output_value[N];

        EXPECT_NO_THROW(cdr_des.deserialize_array(output_value, N));
        EXPECT_ARRAY_EQ(output_value, input_value, N);
    }
    // Serialization.
    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_ser(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);
        EXPECT_NO_THROW(cdr_ser.serialize_array(input_value, N));
    }

    // Deserialization.
    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_des(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);
        T output_value[N];

        EXPECT_NO_THROW(cdr_des.deserialize_array(output_value, N));
        EXPECT_ARRAY_EQ(output_value, input_value, N);
    }
}

template<typename T, size_t N = N_ARR_ELEMENTS>
static void check_no_space_array(
        const T* input_value,
        size_t buf_size = sizeof(T) - 1)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    {
        FastBuffer cdrbuffer(buffer, buf_size);
        Cdr cdr_ser(cdrbuffer);
        EXPECT_THROW(cdr_ser.serialize_array(input_value, N), NotEnoughMemoryException);
    }

    // Deserialization.
    {
        FastBuffer cdrbuffer(buffer, buf_size);
        Cdr cdr_des(cdrbuffer);
        T output_value[N];

        EXPECT_THROW(cdr_des.deserialize_array(output_value, N), NotEnoughMemoryException);
    }
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

TEST(CDRTests, DDSEncapsulation)
{
    char encapsulation[4]{ 0, 0, 0, 0 };
    eprosima::fastcdr::FastBuffer buffer(encapsulation, 4);

    // First encapsulation byte should be 0
    {
        eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
                eprosima::fastcdr::CdrVersion::DDS_CDR);
        for (uint16_t i = 1; i < 256; ++i)
        {
            encapsulation[0] = static_cast<char>(i & 0xFF);
            EXPECT_THROW(cdr.read_encapsulation(), eprosima::fastcdr::exception::BadParamException);
        }

        cdr.reset();
        encapsulation[0] = 0;
        EXPECT_NO_THROW(cdr.read_encapsulation());
    }

    std::array<std::pair<bool, eprosima::fastcdr::CdrVersion>, 256> valid_encapsulations;
    valid_encapsulations.fill({false, eprosima::fastcdr::CdrVersion::DDS_CDR});

    // Valid representation identifiers from table 10.3
    valid_encapsulations[0 + EncodingAlgorithmFlag::PLAIN_CDR] = {true, eprosima::fastcdr::CdrVersion::XCDRv1};   // PLAIN_CDR_BE
    valid_encapsulations[1 + EncodingAlgorithmFlag::PLAIN_CDR] = {true, eprosima::fastcdr::CdrVersion::XCDRv1};   // PLAIN_CDR_LE
    valid_encapsulations[0 + EncodingAlgorithmFlag::PL_CDR] = {true, eprosima::fastcdr::CdrVersion::XCDRv1};      // PL_CDR_BE
    valid_encapsulations[1 + EncodingAlgorithmFlag::PL_CDR] = {true, eprosima::fastcdr::CdrVersion::XCDRv1};      // PL_CDR_LE
    valid_encapsulations[0 + EncodingAlgorithmFlag::PLAIN_CDR2] = {true, eprosima::fastcdr::CdrVersion::XCDRv2};  // PLAIN_CDR2_BE
    valid_encapsulations[1 + EncodingAlgorithmFlag::PLAIN_CDR2] = {true, eprosima::fastcdr::CdrVersion::XCDRv2};  // PLAIN_CDR2_LE
    valid_encapsulations[0 + EncodingAlgorithmFlag::DELIMIT_CDR2] = {true, eprosima::fastcdr::CdrVersion::XCDRv2};// DELIMIT_CDR2_BE
    valid_encapsulations[1 + EncodingAlgorithmFlag::DELIMIT_CDR2] = {true, eprosima::fastcdr::CdrVersion::XCDRv2};// DELIMIT_CDR2_LE
    valid_encapsulations[0 + EncodingAlgorithmFlag::PL_CDR2] = {true, eprosima::fastcdr::CdrVersion::XCDRv2};     // PL_CDR2_BE
    valid_encapsulations[1 + EncodingAlgorithmFlag::PL_CDR2] = {true, eprosima::fastcdr::CdrVersion::XCDRv2};     // PL_CDR2_LE

    for (uint16_t i = 0; i < 256; ++i)
    {
        eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, valid_encapsulations[i].second);
        encapsulation[1] = static_cast<char>(i & 0xFF);
        if (valid_encapsulations[i].first)
        {
            EXPECT_NO_THROW(cdr.read_encapsulation());
        }
        else
        {
            EXPECT_THROW(cdr.read_encapsulation(), eprosima::fastcdr::exception::BadParamException);
        }
    }
}

TEST(CDRTests, CorbaEncapsulation)
{
    char encapsulation[1]{ 0 };
    eprosima::fastcdr::FastBuffer buffer(encapsulation, 1);
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
            eprosima::fastcdr::CdrVersion::CORBA_CDR);

    std::array<bool, 256> valid_encapsulations;
    valid_encapsulations.fill(false);

    valid_encapsulations[0x00] = true;  // BIG_ENDIAN
    valid_encapsulations[0x01] = true;  // LITTLE_ENDIAN

    for (uint16_t i = 0; i < 256; ++i)
    {
        cdr.reset();
        encapsulation[0] = static_cast<char>(i & 0xFF);
        if (valid_encapsulations[i])
        {
            EXPECT_NO_THROW(cdr.read_encapsulation());
        }
        else
        {
            EXPECT_THROW(cdr.read_encapsulation(), eprosima::fastcdr::exception::BadParamException);
        }
    }
}

TEST(CDRTests, Octet)
{
    check_good_case(octet_t);
    check_no_space(octet_t);
}

TEST(CDRTests, Char)
{
    check_good_case(char_t);
    check_no_space(char_t);
}

TEST(CDRTests, WChar)
{
    check_good_case(wchar);
    check_no_space(wchar);
}

TEST(CDRTests, Int8)
{
    check_good_case(int8);
    check_no_space(int8);
}

TEST(CDRTests, UnsignedShort)
{
    check_good_case(ushort_t);
    check_no_space(ushort_t);
}

TEST(CDRTests, Short)
{
    check_good_case(short_t);
    check_no_space(short_t);
}

TEST(CDRTests, UnsignedLong)
{
    check_good_case(ulong_t);
    check_no_space(ulong_t);
}

TEST(CDRTests, Long)
{
    check_good_case(long_t);
    check_no_space(long_t);
}

TEST(CDRTests, UnsignedLongLong)
{
    check_good_case(ulonglong_t);
    check_no_space(ulonglong_t);
}

TEST(CDRTests, LongLong)
{
    check_good_case(longlong_t);
    check_no_space(longlong_t);
}

TEST(CDRTests, Float)
{
    check_good_case(float_tt);
    check_no_space(float_tt);
}

TEST(CDRTests, Double)
{
    check_good_case(double_tt);
    check_no_space(double_tt);
}

TEST(CDRTests, LongDouble)
{
    check_good_case(ldouble_tt);
    check_no_space(ldouble_tt);
}

TEST(CDRTests, Boolean)
{
    check_good_case(bool_t);
    check_no_space(bool_t);
}

TEST(CDRTests, String)
{
    check_good_case(string_t);
    check_no_space(string_t, 1);
    check_bad_length_deserialization(string_t);
}

TEST(CDRTests, WString)
{
    check_good_case(wstring_t);
    check_no_space(wstring_t, 1);
    check_bad_length_deserialization(wstring_t);
}

TEST(CDRTests, EmptyString)
{
    check_good_case(emptystring_t);
    check_no_space(emptystring_t, 1);
}

TEST(CDRTests, EmptyWString)
{
    check_good_case(emptywstring_t);
    check_no_space(emptywstring_t, 1);
}

TEST(CDRTests, STDArrayOctet)
{
    check_good_case(octet_array_t);
    check_no_space(octet_array_t);
}

TEST(CDRTests, STDArrayChar)
{
    check_good_case(char_array_t);
    check_no_space(char_array_t);
}

TEST(CDRTests, STDArrayWChar)
{
    check_good_case(wchar_array_t);
    check_no_space(wchar_array_t);
}

TEST(CDRTests, STDArrayInt8)
{
    check_good_case(int8_array_t);
    check_no_space(int8_array_t);
}

TEST(CDRTests, STDArrayUnsignedShort)
{
    check_good_case(ushort_array_t);
    check_no_space(ushort_array_t);
}

TEST(CDRTests, STDArrayShort)
{
    check_good_case(short_array_t);
    check_no_space(short_array_t);
}

TEST(CDRTests, STDArrayUnsignedLong)
{
    check_good_case(ulong_array_t);
    check_no_space(ulong_array_t);
}

TEST(CDRTests, STDArrayLong)
{
    check_good_case(long_array_t);
    check_no_space(long_array_t);
}

TEST(CDRTests, STDArrayUnsignedLongLong)
{
    check_good_case(ulonglong_array_t);
    check_no_space(ulonglong_array_t);
}

TEST(CDRTests, STDArrayLongLong)
{
    check_good_case(longlong_array_t);
    check_no_space(longlong_array_t);
}

TEST(CDRTests, STDArrayFloat)
{
    check_good_case(float_array_t);
    check_no_space(float_array_t);
}

TEST(CDRTests, STDArrayDouble)
{
    check_good_case(double_array_t);
    check_no_space(double_array_t);
}

TEST(CDRTests, STDArrayLongDouble)
{
    check_good_case(ldouble_array_t);
    check_no_space(ldouble_array_t);
}

TEST(CDRTests, STDArrayBoolean)
{
    check_good_case(bool_array_t);
    check_no_space(bool_array_t);
}

TEST(CDRTests, STDArrayString)
{
    check_good_case(string_array_t);
    check_no_space(string_array_t, 1);
}

TEST(CDRTests, STDArrayWString)
{
    check_good_case(wstring_array_t);
    check_no_space(wstring_array_t, 1);
}

TEST(CDRTests, ArrayOctet)
{
    check_good_case_array(octet_array_2_t);
    check_no_space_array(octet_array_2_t);
}

TEST(CDRTests, ArrayChar)
{
    check_good_case_array(char_array_2_t);
    check_no_space_array(char_array_2_t);
}

TEST(CDRTests, ArrayWChar)
{
    check_good_case_array(wchar_array_2_t);
    check_no_space_array(wchar_array_2_t);
}

TEST(CDRTests, ArrayInt8)
{
    check_good_case_array(int8_array_2_t);
    check_no_space_array(int8_array_2_t);
}

TEST(CDRTests, ArrayUnsignedShort)
{
    check_good_case_array(ushort_array_2_t);
    check_no_space_array(ushort_array_2_t);
}

TEST(CDRTests, ArrayShort)
{
    check_good_case_array(short_array_2_t);
    check_no_space_array(short_array_2_t);
}

TEST(CDRTests, ArrayUnsignedLong)
{
    check_good_case_array(ulong_array_2_t);
    check_no_space_array(ulong_array_2_t);
}

TEST(CDRTests, ArrayLong)
{
    check_good_case_array(long_array_2_t);
    check_no_space_array(long_array_2_t);
}

TEST(CDRTests, ArrayUnsignedLongLong)
{
    check_good_case_array(ulonglong_array_2_t);
    check_no_space_array(ulonglong_array_2_t);
}

TEST(CDRTests, ArrayLongLong)
{
    check_good_case_array(longlong_array_2_t);
    check_no_space_array(longlong_array_2_t);
}

TEST(CDRTests, ArrayFloat)
{
    check_good_case_array(float_array_2_t);
    check_no_space_array(float_array_2_t);
}

TEST(CDRTests, ArrayDouble)
{
    check_good_case_array(double_array_2_t);
    check_no_space_array(double_array_2_t);
}

TEST(CDRTests, ArrayLongDouble)
{
    check_good_case_array(ldouble_array_2_t);
    check_no_space_array(ldouble_array_2_t);
}

TEST(CDRTests, ArrayBoolean)
{
    check_good_case_array(bool_array_2_t);
    check_no_space_array(bool_array_2_t);
}

TEST(CDRTests, ArrayString)
{
    check_good_case_array(string_array_2_t);
    check_no_space_array(string_array_2_t, 1);
}

TEST(CDRTests, ArrayWString)
{
    check_good_case_array(wstring_array_2_t);
    check_no_space_array(wstring_array_2_t, 1);
}

TEST(CDRTests, STDVectorOctet)
{
    check_good_case(octet_vector_t);
    check_no_space(octet_vector_t, 1);
    check_bad_length_deserialization(octet_vector_t);
}

TEST(CDRTests, STDVectorChar)
{
    check_good_case(char_vector_t);
    check_no_space(char_vector_t, 1);
    check_bad_length_deserialization(char_vector_t);
}

TEST(CDRTests, STDVectorWChar)
{
    check_good_case(wchar_vector_t);
    check_no_space(wchar_vector_t, 1);
    check_bad_length_deserialization(wchar_vector_t);
}

TEST(CDRTests, STDVectorInt8)
{
    check_good_case(int8_vector_t);
    check_no_space(int8_vector_t, 1);
    check_bad_length_deserialization(int8_vector_t);
}

TEST(CDRTests, STDVectorUnsignedShort)
{
    check_good_case(ushort_vector_t);
    check_no_space(ushort_vector_t, 1);
    check_bad_length_deserialization(ushort_vector_t);
}

TEST(CDRTests, STDVectorShort)
{
    check_good_case(short_vector_t);
    check_no_space(short_vector_t, 1);
    check_bad_length_deserialization(short_vector_t);
}

TEST(CDRTests, STDVectorUnsignedLong)
{
    check_good_case(ulong_vector_t);
    check_no_space(ulong_vector_t, 1);
    check_bad_length_deserialization(ulong_vector_t);
}

TEST(CDRTests, STDVectorLong)
{
    check_good_case(long_vector_t);
    check_no_space(long_vector_t, 1);
    check_bad_length_deserialization(long_vector_t);
}

TEST(CDRTests, STDVectorUnsignedLongLong)
{
    check_good_case(ulonglong_vector_t);
    check_no_space(ulonglong_vector_t, 1);
    check_bad_length_deserialization(ulonglong_vector_t);
}

TEST(CDRTests, STDVectorLongLong)
{
    check_good_case(longlong_vector_t);
    check_no_space(longlong_vector_t, 1);
    check_bad_length_deserialization(longlong_vector_t);
}

TEST(CDRTests, STDVectorFloat)
{
    check_good_case(float_vector_t);
    check_no_space(float_vector_t, 1);
    check_bad_length_deserialization(float_vector_t);
}

TEST(CDRTests, STDVectorDouble)
{
    check_good_case(double_vector_t);
    check_no_space(double_vector_t, 1);
    check_bad_length_deserialization(double_vector_t);
}

TEST(CDRTests, STDVectorLongDouble)
{
    check_good_case(ldouble_vector_t);
    check_no_space(ldouble_vector_t, 1);
    check_bad_length_deserialization(ldouble_vector_t);
}

TEST(CDRTests, STDVectorBoolean)
{
    check_good_case(bool_vector_t);
    check_no_space(bool_vector_t, 1);
    check_bad_length_deserialization(bool_vector_t);
}

TEST(CDRTests, STDVectorString)
{
    check_good_case(string_vector_t);
    check_no_space(string_vector_t, 1);
    check_bad_length_deserialization(string_vector_t);
}

TEST(CDRTests, STDVectorWString)
{
    check_good_case(wstring_vector_t);
    check_no_space(wstring_vector_t, 1);
    check_bad_length_deserialization(wstring_vector_t);
}

TEST(CDRTests, STDTripleArrayUnsignedLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << triple_ulong_array_t;
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    std::array<std::array<std::array<uint32_t, 3>, 2>, 2> triple_ulong_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> triple_ulong_array_value;
    });

    EXPECT_EQ(triple_ulong_array_value, triple_ulong_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    Cdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << triple_ulong_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> triple_ulong_array_value;
    },
        NotEnoughMemoryException);
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
        cdr_ser.serialize_sequence(octet_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint8_t* octet_seq_value = NULL; size_t octet_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(octet_seq_value, octet_seq_len);
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
        cdr_ser_bad.serialize_sequence(octet_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(octet_seq_value, octet_seq_len);
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
        cdr_ser.serialize_sequence(char_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    char* char_seq_value = NULL; size_t char_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(char_seq_value, char_seq_len);
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
        cdr_ser_bad.serialize_sequence(char_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(char_seq_value, char_seq_len);
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
        cdr_ser.serialize_sequence(wchar_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    wchar_t* char_seq_value = NULL; size_t char_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(char_seq_value, char_seq_len);
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
        cdr_ser_bad.serialize_sequence(wchar_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(char_seq_value, char_seq_len);
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
        cdr_ser.serialize_sequence(int8_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    int8_t* int8_seq_value = NULL; size_t int8_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(int8_seq_value, int8_seq_len);
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
        cdr_ser_bad.serialize_sequence(int8_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(int8_seq_value, int8_seq_len);
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
        cdr_ser.serialize_sequence(ushort_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint16_t* ushort_seq_value = NULL; size_t ushort_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(ushort_seq_value, ushort_seq_len);
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
        cdr_ser_bad.serialize_sequence(ushort_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(ushort_seq_value, ushort_seq_len);
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
        cdr_ser.serialize_sequence(short_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    int16_t* short_seq_value = NULL; size_t short_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(short_seq_value, short_seq_len);
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
        cdr_ser_bad.serialize_sequence(short_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(short_seq_value, short_seq_len);
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
        cdr_ser.serialize_sequence(ulong_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint32_t* ulong_seq_value = NULL; size_t ulong_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(ulong_seq_value, ulong_seq_len);
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
        cdr_ser_bad.serialize_sequence(ulong_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(ulong_seq_value, ulong_seq_len);
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
        cdr_ser.serialize_sequence(long_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    int32_t* long_seq_value = NULL; size_t long_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(long_seq_value, long_seq_len);
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
        cdr_ser_bad.serialize_sequence(long_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(long_seq_value, long_seq_len);
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
        cdr_ser.serialize_sequence(ulonglong_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint64_t* ulonglong_seq_value = NULL; size_t ulonglong_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(ulonglong_seq_value, ulonglong_seq_len);
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
        cdr_ser_bad.serialize_sequence(ulonglong_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(ulonglong_seq_value, ulonglong_seq_len);
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
        cdr_ser.serialize_sequence(longlong_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    int64_t* longlong_seq_value = NULL; size_t longlong_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(longlong_seq_value, longlong_seq_len);
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
        cdr_ser_bad.serialize_sequence(longlong_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(longlong_seq_value, longlong_seq_len);
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
        cdr_ser.serialize_sequence(float_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    float* float_seq_value = NULL; size_t float_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(float_seq_value, float_seq_len);
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
        cdr_ser_bad.serialize_sequence(float_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(float_seq_value, float_seq_len);
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
        cdr_ser.serialize_sequence(double_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    double* double_seq_value = NULL; size_t double_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(double_seq_value, double_seq_len);
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
        cdr_ser_bad.serialize_sequence(double_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(double_seq_value, double_seq_len);
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
        cdr_ser.serialize_sequence(ldouble_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    long double* ldouble_seq_value = NULL; size_t ldouble_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(ldouble_seq_value, ldouble_seq_len);
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
        cdr_ser_bad.serialize_sequence(ldouble_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(ldouble_seq_value, ldouble_seq_len);
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
        cdr_ser.serialize_sequence(bool_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    bool* bool_seq_value = NULL; size_t bool_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(bool_seq_value, bool_seq_len);
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
        cdr_ser_bad.serialize_sequence(bool_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(bool_seq_value, bool_seq_len);
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
        cdr_ser.serialize_sequence(string_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    std::string* string_seq_value = NULL; size_t string_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(string_seq_value, string_seq_len);
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
        cdr_ser_bad.serialize_sequence(string_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(string_seq_value, string_seq_len);
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
        cdr_ser.serialize_sequence(wstring_seq_t, 5);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    std::wstring* string_seq_value = NULL; size_t string_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(string_seq_value, string_seq_len);
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
        cdr_ser_bad.serialize_sequence(wstring_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    Cdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(string_seq_value, string_seq_len);
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

    char* c_string_value = NULL;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize(c_string_value);
    });

    EXPECT_EQ(strcmp(c_string_value, c_string_t), 0);

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

    wchar_t* c_string_value = NULL;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize(c_string_value);
    });

    EXPECT_EQ(wcscmp(c_string_value, c_wstring_t), 0);

    free(c_string_value);

    // Check the big endianness case
    char buffer_bigendi[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer_bigendi(buffer_bigendi, BUFFER_LENGTH);
    Cdr cdr_ser_bigendi(cdrbuffer_bigendi, eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);

    EXPECT_NO_THROW(
    {
        cdr_ser_bigendi.serialize(c_wstring_t);
    });

    // Deserialization.
    Cdr cdr_des_bigendi(cdrbuffer_bigendi, eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);

    wchar_t* c_wstring_bigendi {nullptr};

    EXPECT_NO_THROW(
    {
        cdr_des_bigendi.deserialize(c_wstring_bigendi);
    });

    EXPECT_EQ(wcscmp(c_wstring_bigendi, c_wstring_t), 0);

    free(c_wstring_bigendi);

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
        cdr_ser.serialize_array(octet_array_2_t,
        5).serialize_array(char_array_2_t, 5).serialize_array(int8_array_2_t, 5).
                serialize_array(ushort_array_2_t, 5).serialize_array(short_array_2_t,
        5).serialize_array(ulong_array_2_t,
        5).
                serialize_array(long_array_2_t, 5).serialize_array(ulonglong_array_2_t,
        5).serialize_array(longlong_array_2_t, 5).
                serialize_array(float_array_2_t, 5).serialize_array(double_array_2_t,
        5).serialize_array(ldouble_array_2_t,
        5).serialize_array(
            bool_array_2_t,
            5);
        cdr_ser << octet_vector_t << char_vector_t << int8_vector_t << ushort_vector_t << short_vector_t << ulong_vector_t <<
            long_vector_t << ulonglong_vector_t << longlong_vector_t << float_vector_t << double_vector_t << ldouble_vector_t << bool_vector_t;
        cdr_ser << triple_ulong_array_t;
        cdr_ser.serialize_sequence(octet_seq_t, 5).serialize_sequence(char_seq_t, 5).serialize_sequence(int8_seq_t, 5).
                serialize_sequence(ushort_seq_t, 5).serialize_sequence(short_seq_t, 5).serialize_sequence(ulong_seq_t,
        5).
                serialize_sequence(long_seq_t, 5).serialize_sequence(ulonglong_seq_t,
        5).serialize_sequence(longlong_seq_t,
        5).
                serialize_sequence(float_seq_t, 5).serialize_sequence(double_seq_t, 5).serialize_sequence(ldouble_seq_t,
        5);
        cdr_ser.serialize(c_string_t);
        // Wstring and wchar
        cdr_ser << wchar << wstring_t;
        cdr_ser.serialize_array(wchar_array_2_t, 5).serialize_array(wstring_array_2_t, 5);
        cdr_ser << wchar_array_t << wstring_array_t;
        cdr_ser << wchar_vector_t << wstring_vector_t;
        cdr_ser.serialize_sequence(wchar_seq_t, 5);
        cdr_ser.serialize_sequence(wstring_seq_t, 5);
        cdr_ser.serialize_sequence(string_seq_t, 5);
        cdr_ser.serialize(c_wstring_t);
    });

    // Deserialization.
    Cdr cdr_des(cdrbuffer);

    uint8_t octet_value = 0;
    char char_value = 0;
    wchar_t wchar_value = 0;
    int8_t int8_value = 0;
    uint16_t ushort_value = 0;
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
    std::array<uint8_t, N_ARR_ELEMENTS> octet_array_value;
    uint8_t octet_array_2_value[N_ARR_ELEMENTS];
    std::array<char, N_ARR_ELEMENTS> char_array_value;
    std::array<wchar_t, N_ARR_ELEMENTS> wchar_array_value;
    char char_array_2_value[N_ARR_ELEMENTS];
    wchar_t wchar_array_2_value[N_ARR_ELEMENTS];
    std::array<int8_t, N_ARR_ELEMENTS> int8_array_value;
    int8_t int8_array_2_value[N_ARR_ELEMENTS];
    std::array<uint16_t, N_ARR_ELEMENTS> ushort_array_value;
    uint16_t ushort_array_2_value[N_ARR_ELEMENTS];
    std::array<int16_t, N_ARR_ELEMENTS> short_array_value;
    int16_t short_array_2_value[N_ARR_ELEMENTS];
    std::array<uint32_t, N_ARR_ELEMENTS> ulong_array_value;
    uint32_t ulong_array_2_value[N_ARR_ELEMENTS];
    std::array<int32_t, N_ARR_ELEMENTS> long_array_value;
    int32_t long_array_2_value[N_ARR_ELEMENTS];
    std::array<uint64_t, N_ARR_ELEMENTS> ulonglong_array_value;
    uint64_t ulonglong_array_2_value[N_ARR_ELEMENTS];
    std::array<int64_t, N_ARR_ELEMENTS> longlong_array_value;
    int64_t longlong_array_2_value[N_ARR_ELEMENTS];
    std::array<float, N_ARR_ELEMENTS> float_array_value;
    float float_array_2_value[N_ARR_ELEMENTS];
    std::array<double, N_ARR_ELEMENTS> double_array_value;
    double double_array_2_value[N_ARR_ELEMENTS];
    std::array<long double, N_ARR_ELEMENTS> ldouble_array_value;
    long double ldouble_array_2_value[N_ARR_ELEMENTS];
    std::array<bool, N_ARR_ELEMENTS> bool_array_value;
    bool bool_array_2_value[N_ARR_ELEMENTS];
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
    uint8_t* octet_seq_value = NULL; size_t octet_seq_len;
    char* char_seq_value = NULL; size_t char_seq_len;
    wchar_t* wchar_seq_value = NULL; size_t wchar_seq_len;
    int8_t* int8_seq_value = NULL; size_t int8_seq_len;
    uint16_t* ushort_seq_value = NULL; size_t ushort_seq_len;
    int16_t* short_seq_value = NULL; size_t short_seq_len;
    uint32_t* ulong_seq_value = NULL; size_t ulong_seq_len;
    int32_t* long_seq_value = NULL; size_t long_seq_len;
    uint64_t* ulonglong_seq_value = NULL; size_t ulonglong_seq_len;
    int64_t* longlong_seq_value = NULL; size_t longlong_seq_len;
    float* float_seq_value = NULL; size_t float_seq_len;
    double* double_seq_value = NULL; size_t double_seq_len;
    long double* ldouble_seq_value = NULL; size_t ldouble_seq_len;
    char* c_string_value = NULL;
    wchar_t* c_wstring_value = NULL;
    std::array<std::wstring, N_ARR_ELEMENTS> wstring_array_value;
    std::wstring wstring_array_2_value[N_ARR_ELEMENTS];
    std::vector<std::wstring> wstring_vector_value;
    std::wstring* wstring_seq_value = NULL; size_t wstring_seq_len;
    std::string* string_seq_value = NULL; size_t string_seq_len;

    EXPECT_NO_THROW(
    {
        cdr_des >> octet_value >> char_value >> int8_value >> ushort_value >> short_value >> ulong_value >> long_value >>
        ulonglong_value >> longlong_value >> float_value >> double_value >> ldouble_value  >> bool_value >> string_value;
        cdr_des >> octet_array_value >> char_array_value >> int8_array_value >> ushort_array_value >> short_array_value >>
        ulong_array_value >> long_array_value >> ulonglong_array_value >> longlong_array_value >> float_array_value >>
        double_array_value >> ldouble_array_value >> bool_array_value;
        cdr_des.deserialize_array(octet_array_2_value,
        5).deserialize_array(char_array_2_value, 5).deserialize_array(int8_array_2_value, 5).
                deserialize_array(ushort_array_2_value, 5).deserialize_array(short_array_2_value,
        5).deserialize_array(ulong_array_2_value, 5).
                deserialize_array(long_array_2_value, 5).deserialize_array(ulonglong_array_2_value,
        5).deserialize_array(longlong_array_2_value, 5).
                deserialize_array(float_array_2_value, 5).deserialize_array(double_array_2_value,
        5).deserialize_array(ldouble_array_2_value, 5).
                deserialize_array(bool_array_2_value,
        5);
        cdr_des >> octet_vector_value >> char_vector_value >> int8_vector_value >> ushort_vector_value >> short_vector_value >>
        ulong_vector_value >> long_vector_value >> ulonglong_vector_value >> longlong_vector_value >> float_vector_value >>
        double_vector_value >> ldouble_vector_value >> bool_vector_value;
        cdr_des >> triple_ulong_array_value;
        cdr_des.deserialize_sequence(octet_seq_value, octet_seq_len).deserialize_sequence(char_seq_value, char_seq_len).
                deserialize_sequence(int8_seq_value, int8_seq_len).deserialize_sequence(ushort_seq_value,
        ushort_seq_len).
                deserialize_sequence(short_seq_value, short_seq_len).deserialize_sequence(ulong_seq_value,
        ulong_seq_len).
                deserialize_sequence(long_seq_value, long_seq_len).deserialize_sequence(ulonglong_seq_value,
        ulonglong_seq_len).
                deserialize_sequence(longlong_seq_value, longlong_seq_len).deserialize_sequence(float_seq_value,
        float_seq_len).
                deserialize_sequence(double_seq_value, double_seq_len).deserialize_sequence(ldouble_seq_value,
        ldouble_seq_len);
        cdr_des.deserialize(c_string_value);
        // WString and wchar_t
        cdr_des >> wchar_value >> wstring_value;
        cdr_des.deserialize_array(wchar_array_2_value, 5).deserialize_array(wstring_array_2_value, 5);
        cdr_des >> wchar_array_value >> wstring_array_value;
        cdr_des >> wchar_vector_value >> wstring_vector_value;
        cdr_des.deserialize_sequence(wchar_seq_value, wchar_seq_len);
        cdr_des.deserialize_sequence(wstring_seq_value, wstring_seq_len);
        cdr_des.deserialize_sequence(string_seq_value, string_seq_len);
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

// Regression test for Fast DDS issue #5136
// A non-empty map should be cleared before deserializing in XCDRv1
TEST(CDRTests, DeserializeIntoANonEmptyMapInXCDRv1)
{
    char buffer[14] =
    {
        0x00, 0x00, 0x00, 0x01,  // Map length
        0x00, 0x02,              // Key
        0x00, 0x00,              // Alignment
        0x00, 0x00, 0x00, 0x01,  // Length
        65,  0x00                // 'A'
    };

    std::map<uint16_t, std::string> initialized_map{
        {1, "a"}
    };

    FastBuffer cdr_buffer(buffer, 14);
    Cdr cdr_ser_map(
        cdr_buffer,
        eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS,
        XCDRv1);

    // Deserialization in a non-empty map
    cdr_ser_map >> initialized_map;
    ASSERT_EQ(initialized_map.size(), 1u);
    ASSERT_EQ(initialized_map.at(2), "A");
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
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << ushort_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint16_t ushort_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> ushort_value;
    });

    EXPECT_EQ(ushort_value, ushort_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << ushort_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> ushort_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, Short)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << short_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int16_t short_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> short_value;
    });

    EXPECT_EQ(short_value, short_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << short_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> short_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, UnsignedLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << ulong_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint32_t ulong_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> ulong_value;
    });

    EXPECT_EQ(ulong_value, ulong_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << ulong_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> ulong_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, Long)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << long_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int32_t long_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> long_value;
    });

    EXPECT_EQ(long_value, long_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << long_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> long_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, UnsignedLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << ulonglong_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint64_t ulonglong_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> ulonglong_value;
    });

    EXPECT_EQ(ulonglong_value, ulonglong_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << ulonglong_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> ulonglong_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, LongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << longlong_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int64_t longlong_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> longlong_value;
    });

    EXPECT_EQ(longlong_value, longlong_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << longlong_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> longlong_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, Float)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << float_tt;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    float float_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> float_value;
    });

    EXPECT_FLOAT_EQ(float_value, float_tt);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << float_tt;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> float_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, Double)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << double_tt;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    double double_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> double_value;
    });

    EXPECT_DOUBLE_EQ(double_value, double_tt);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << double_tt;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> double_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, LongDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << ldouble_tt;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    long double ldouble_value = 0;

    EXPECT_NO_THROW(
    {
        cdr_des >> ldouble_value;
    });

    EXPECT_LONG_DOUBLE_EQ(ldouble_value, ldouble_tt);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << ldouble_tt;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> ldouble_value;
    },
        NotEnoughMemoryException);
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
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << string_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::string string_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> string_value;
    });

    EXPECT_EQ(string_value, string_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << string_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> string_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, WString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << wstring_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::wstring string_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> string_value;
    });

    EXPECT_EQ(string_value, wstring_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << wstring_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> string_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, EmptyString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << emptystring_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::string string_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> string_value;
    });

    EXPECT_EQ(string_value, emptystring_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << emptystring_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> string_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, EmptyWString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << emptywstring_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::wstring wstring_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> wstring_value;
    });

    EXPECT_EQ(wstring_value, emptywstring_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << emptywstring_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> wstring_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayOctet)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << octet_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<uint8_t, N_ARR_ELEMENTS> octet_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> octet_array_value;
    });

    EXPECT_EQ(octet_array_value, octet_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << octet_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> octet_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << char_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<char, N_ARR_ELEMENTS> char_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> char_array_value;
    });

    EXPECT_EQ(char_array_value, char_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << char_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> char_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayWChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << wchar_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<wchar_t, N_ARR_ELEMENTS> char_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> char_array_value;
    });

    EXPECT_EQ(char_array_value, wchar_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << wchar_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> char_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayInt8)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << int8_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<int8_t, N_ARR_ELEMENTS> int8_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> int8_array_value;
    });

    EXPECT_EQ(int8_array_value, int8_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << int8_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> int8_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayUnsignedShort)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << ushort_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<uint16_t, N_ARR_ELEMENTS> ushort_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> ushort_array_value;
    });

    EXPECT_EQ(ushort_array_value, ushort_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << ushort_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> ushort_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayShort)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << short_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<int16_t, N_ARR_ELEMENTS> short_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> short_array_value;
    });

    EXPECT_EQ(short_array_value, short_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << short_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> short_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayUnsignedLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << ulong_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<uint32_t, N_ARR_ELEMENTS> ulong_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> ulong_array_value;
    });

    EXPECT_EQ(ulong_array_value, ulong_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << ulong_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> ulong_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << long_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<int32_t, N_ARR_ELEMENTS> long_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> long_array_value;
    });

    EXPECT_EQ(long_array_value, long_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << long_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> long_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayUnsignedLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << ulonglong_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<uint64_t, N_ARR_ELEMENTS> ulonglong_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> ulonglong_array_value;
    });

    EXPECT_EQ(ulonglong_array_value, ulonglong_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << ulonglong_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> ulonglong_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << longlong_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<int64_t, N_ARR_ELEMENTS> longlong_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> longlong_array_value;
    });

    EXPECT_EQ(longlong_array_value, longlong_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << longlong_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> longlong_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayFloat)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << float_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<float, N_ARR_ELEMENTS> float_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> float_array_value;
    });

    EXPECT_EQ(float_array_value, float_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << float_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> float_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << double_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<double, N_ARR_ELEMENTS> double_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> double_array_value;
    });

    EXPECT_EQ(double_array_value, double_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << double_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> double_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayLongDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << ldouble_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<long double, N_ARR_ELEMENTS> ldouble_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> ldouble_array_value;
    });

    EXPECT_EQ(ldouble_array_value, ldouble_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << ldouble_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> ldouble_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayBoolean)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << bool_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<bool, N_ARR_ELEMENTS> bool_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> bool_array_value;
    });

    EXPECT_EQ(bool_array_value, bool_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << bool_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> bool_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << string_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<std::string, N_ARR_ELEMENTS> string_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> string_array_value;
    });

    EXPECT_EQ(string_array_value, string_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << string_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> string_array_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDArrayWString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << wstring_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<std::wstring, N_ARR_ELEMENTS> string_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> string_array_value;
    });

    EXPECT_EQ(string_array_value, wstring_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << wstring_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> string_array_value;
    },
        NotEnoughMemoryException);
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
        cdr_ser.serialize_array(octet_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint8_t octet_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(octet_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(octet_array_2_value, octet_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(octet_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(octet_array_2_value, 5);
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
        cdr_ser.serialize_array(char_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    char char_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(char_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(char_array_2_value, char_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(char_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(char_array_2_value, 5);
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
        cdr_ser.serialize_array(wchar_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    wchar_t char_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(char_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(char_array_2_value, wchar_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(wchar_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(char_array_2_value, 5);
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
        cdr_ser.serialize_array(int8_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int8_t int8_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(int8_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(int8_array_2_value, int8_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(int8_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(int8_array_2_value, 5);
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
        cdr_ser.serialize_array(ushort_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint16_t ushort_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(ushort_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(ushort_array_2_value, ushort_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(ushort_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(ushort_array_2_value, 5);
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
        cdr_ser.serialize_array(short_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int16_t short_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(short_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(short_array_2_value, short_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(short_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(short_array_2_value, 5);
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
        cdr_ser.serialize_array(ulong_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint32_t ulong_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(ulong_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(ulong_array_2_value, ulong_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(ulong_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(ulong_array_2_value, 5);
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
        cdr_ser.serialize_array(long_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int32_t long_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(long_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(long_array_2_value, long_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(long_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(long_array_2_value, 5);
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
        cdr_ser.serialize_array(ulonglong_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint64_t ulonglong_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(ulonglong_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(ulonglong_array_2_value, ulonglong_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(ulonglong_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(ulonglong_array_2_value, 5);
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
        cdr_ser.serialize_array(longlong_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int64_t longlong_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(longlong_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(longlong_array_2_value, longlong_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(longlong_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(longlong_array_2_value, 5);
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
        cdr_ser.serialize_array(float_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    float float_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(float_array_2_value, 5);
    });

    EXPECT_ARRAY_FLOAT_EQ(float_array_2_value, float_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(float_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(float_array_2_value, 5);
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
        cdr_ser.serialize_array(double_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    double double_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(double_array_2_value, 5);
    });

    EXPECT_ARRAY_DOUBLE_EQ(double_array_2_value, double_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(double_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(double_array_2_value, 5);
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
        cdr_ser.serialize_array(ldouble_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    long double ldouble_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(ldouble_array_2_value, 5);
    });

    EXPECT_ARRAY_LONG_DOUBLE_EQ(ldouble_array_2_value, ldouble_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(ldouble_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(ldouble_array_2_value, 5);
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
        cdr_ser.serialize_array(bool_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    bool bool_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(bool_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(bool_array_2_value, bool_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(bool_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(bool_array_2_value, 5);
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
        cdr_ser.serialize_array(string_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::string string_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(string_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(string_array_2_value, string_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(string_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(string_array_2_value, 5);
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
        cdr_ser.serialize_array(wstring_array_2_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::wstring string_array_2_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_array(string_array_2_value, 5);
    });

    EXPECT_ARRAY_EQ(string_array_2_value, wstring_array_2_t, 5);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad.serialize_array(wstring_array_2_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_array(string_array_2_value, 5);
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorOctet)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << octet_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<uint8_t> octet_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> octet_vector_value;
    });

    EXPECT_EQ(octet_vector_value, octet_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << octet_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> octet_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << char_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<char> char_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> char_vector_value;
    });

    EXPECT_EQ(char_vector_value, char_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << char_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> char_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorWChar)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << wchar_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<wchar_t> char_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> char_vector_value;
    });

    EXPECT_EQ(char_vector_value, wchar_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << wchar_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> char_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorInt8)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << int8_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<int8_t> int8_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> int8_vector_value;
    });

    EXPECT_EQ(int8_vector_value, int8_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << int8_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> int8_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorUnsignedShort)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << ushort_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<uint16_t> ushort_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> ushort_vector_value;
    });

    EXPECT_EQ(ushort_vector_value, ushort_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << ushort_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> ushort_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorShort)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << short_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<int16_t> short_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> short_vector_value;
    });

    EXPECT_EQ(short_vector_value, short_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << short_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> short_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorUnsignedLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << ulong_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<uint32_t> ulong_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> ulong_vector_value;
    });

    EXPECT_EQ(ulong_vector_value, ulong_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << ulong_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> ulong_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << long_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<int32_t> long_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> long_vector_value;
    });

    EXPECT_EQ(long_vector_value, long_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << long_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> long_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorUnsignedLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << ulonglong_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<uint64_t> ulonglong_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> ulonglong_vector_value;
    });

    EXPECT_EQ(ulonglong_vector_value, ulonglong_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << ulonglong_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> ulonglong_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorLongLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << longlong_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<int64_t> longlong_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> longlong_vector_value;
    });

    EXPECT_EQ(longlong_vector_value, longlong_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << longlong_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> longlong_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorFloat)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << float_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<float> float_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> float_vector_value;
    });

    EXPECT_EQ(float_vector_value, float_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << float_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> float_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << double_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<double> double_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> double_vector_value;
    });

    EXPECT_EQ(double_vector_value, double_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << double_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> double_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorLongDouble)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << ldouble_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<long double> ldouble_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> ldouble_vector_value;
    });

    EXPECT_EQ(ldouble_vector_value, ldouble_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << ldouble_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> ldouble_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorBoolean)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << bool_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<bool> bool_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> bool_vector_value;
    });

    EXPECT_EQ(bool_vector_value, bool_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << bool_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> bool_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << string_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<std::string> string_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> string_vector_value;
    });

    EXPECT_EQ(string_vector_value, string_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << string_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> string_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDVectorWString)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << wstring_vector_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::vector<std::wstring> string_vector_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> string_vector_value;
    });

    EXPECT_EQ(string_vector_value, wstring_vector_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << wstring_vector_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> string_vector_value;
    },
        NotEnoughMemoryException);
}

TEST(FastCDRTests, STDTripleArrayUnsignedLong)
{
    // Check good case.
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(
    {
        cdr_ser << triple_ulong_array_t;
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::array<std::array<std::array<uint32_t, 3>, 2>, 2> triple_ulong_array_value;

    EXPECT_NO_THROW(
    {
        cdr_des >> triple_ulong_array_value;
    });

    EXPECT_EQ(triple_ulong_array_value, triple_ulong_array_t);

    // Check bad case without space
    char buffer_bad[1];

    // Serialization.
    FastBuffer cdrbuffer_bad(buffer_bad, 1);
    FastCdr cdr_ser_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_ser_bad << triple_ulong_array_t;
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad >> triple_ulong_array_value;
    },
        NotEnoughMemoryException);
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
        cdr_ser.serialize_sequence(octet_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint8_t* octet_seq_value = NULL; size_t octet_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(octet_seq_value, octet_seq_len);
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
        cdr_ser_bad.serialize_sequence(octet_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(octet_seq_value, octet_seq_len);
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
        cdr_ser.serialize_sequence(char_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    char* char_seq_value = NULL; size_t char_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(char_seq_value, char_seq_len);
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
        cdr_ser_bad.serialize_sequence(char_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(char_seq_value, char_seq_len);
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
        cdr_ser.serialize_sequence(wchar_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    wchar_t* char_seq_value = NULL; size_t char_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(char_seq_value, char_seq_len);
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
        cdr_ser_bad.serialize_sequence(wchar_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(char_seq_value, char_seq_len);
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
        cdr_ser.serialize_sequence(int8_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int8_t* int8_seq_value = NULL; size_t int8_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(int8_seq_value, int8_seq_len);
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
        cdr_ser_bad.serialize_sequence(int8_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(int8_seq_value, int8_seq_len);
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
        cdr_ser.serialize_sequence(ushort_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint16_t* ushort_seq_value = NULL; size_t ushort_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(ushort_seq_value, ushort_seq_len);
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
        cdr_ser_bad.serialize_sequence(ushort_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(ushort_seq_value, ushort_seq_len);
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
        cdr_ser.serialize_sequence(short_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int16_t* short_seq_value = NULL; size_t short_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(short_seq_value, short_seq_len);
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
        cdr_ser_bad.serialize_sequence(short_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(short_seq_value, short_seq_len);
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
        cdr_ser.serialize_sequence(ulong_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint32_t* ulong_seq_value = NULL; size_t ulong_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(ulong_seq_value, ulong_seq_len);
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
        cdr_ser_bad.serialize_sequence(ulong_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(ulong_seq_value, ulong_seq_len);
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
        cdr_ser.serialize_sequence(long_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int32_t* long_seq_value = NULL; size_t long_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(long_seq_value, long_seq_len);
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
        cdr_ser_bad.serialize_sequence(long_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(long_seq_value, long_seq_len);
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
        cdr_ser.serialize_sequence(ulonglong_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint64_t* ulonglong_seq_value = NULL; size_t ulonglong_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(ulonglong_seq_value, ulonglong_seq_len);
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
        cdr_ser_bad.serialize_sequence(ulonglong_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(ulonglong_seq_value, ulonglong_seq_len);
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
        cdr_ser.serialize_sequence(longlong_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    int64_t* longlong_seq_value = NULL; size_t longlong_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(longlong_seq_value, longlong_seq_len);
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
        cdr_ser_bad.serialize_sequence(longlong_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(longlong_seq_value, longlong_seq_len);
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
        cdr_ser.serialize_sequence(float_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    float* float_seq_value = NULL; size_t float_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(float_seq_value, float_seq_len);
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
        cdr_ser_bad.serialize_sequence(float_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(float_seq_value, float_seq_len);
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
        cdr_ser.serialize_sequence(double_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    double* double_seq_value = NULL; size_t double_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(double_seq_value, double_seq_len);
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
        cdr_ser_bad.serialize_sequence(double_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(double_seq_value, double_seq_len);
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
        cdr_ser.serialize_sequence(ldouble_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    long double* ldouble_seq_value = NULL; size_t ldouble_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(ldouble_seq_value, ldouble_seq_len);
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
        cdr_ser_bad.serialize_sequence(ldouble_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(ldouble_seq_value, ldouble_seq_len);
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
        cdr_ser.serialize_sequence(bool_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    bool* bool_seq_value = NULL; size_t bool_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(bool_seq_value, bool_seq_len);
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
        cdr_ser_bad.serialize_sequence(bool_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(bool_seq_value, bool_seq_len);
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
        cdr_ser.serialize_sequence(string_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::string* string_seq_value = NULL; size_t string_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(string_seq_value, string_seq_len);
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
        cdr_ser_bad.serialize_sequence(string_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(string_seq_value, string_seq_len);
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
        cdr_ser.serialize_sequence(wstring_seq_t, 5);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    std::wstring* string_seq_value = NULL; size_t string_seq_len = 0;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize_sequence(string_seq_value, string_seq_len);
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
        cdr_ser_bad.serialize_sequence(wstring_seq_t, 5);
    },
        NotEnoughMemoryException);

    // Deserialization.
    FastCdr cdr_des_bad(cdrbuffer_bad);

    EXPECT_THROW(
    {
        cdr_des_bad.deserialize_sequence(string_seq_value, string_seq_len);
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

    char* c_string_value = NULL;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize(c_string_value);
    });

    EXPECT_EQ(strcmp(c_string_value, c_string_t), 0);

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

    wchar_t* c_string_value = NULL;

    EXPECT_NO_THROW(
    {
        cdr_des.deserialize(c_string_value);
    });

    EXPECT_EQ(wcscmp(c_string_value, c_wstring_t), 0);

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
        cdr_ser.serialize_array(octet_array_2_t,
        5).serialize_array(char_array_2_t, 5).serialize_array(int8_array_2_t, 5).
                serialize_array(ushort_array_2_t, 5).serialize_array(short_array_2_t,
        5).serialize_array(ulong_array_2_t,
        5).
                serialize_array(long_array_2_t, 5).serialize_array(ulonglong_array_2_t,
        5).serialize_array(longlong_array_2_t, 5).
                serialize_array(float_array_2_t, 5).serialize_array(double_array_2_t,
        5).serialize_array(ldouble_array_2_t,
        5).
                serialize_array(bool_array_2_t,
        5);
        cdr_ser << octet_vector_t << char_vector_t << int8_vector_t << ushort_vector_t << short_vector_t << ulong_vector_t <<
            long_vector_t << ulonglong_vector_t << longlong_vector_t << float_vector_t << double_vector_t <<
            ldouble_vector_t << bool_vector_t;
        cdr_ser << triple_ulong_array_t;
        cdr_ser.serialize_sequence(octet_seq_t, 5).serialize_sequence(char_seq_t, 5).serialize_sequence(int8_seq_t, 5).
                serialize_sequence(ushort_seq_t, 5).serialize_sequence(short_seq_t, 5).serialize_sequence(ulong_seq_t,
        5).
                serialize_sequence(long_seq_t, 5).serialize_sequence(ulonglong_seq_t,
        5).serialize_sequence(longlong_seq_t,
        5).
                serialize_sequence(float_seq_t, 5).serialize_sequence(double_seq_t, 5).serialize_sequence(ldouble_seq_t,
        5);
        cdr_ser.serialize(c_string_t);
        // Wstring and wchar
        cdr_ser << wchar << wstring_t;
        cdr_ser.serialize_array(wchar_array_2_t, 5).serialize_array(wstring_array_2_t, 5);
        cdr_ser << wchar_array_t << wstring_array_t;
        cdr_ser << wchar_vector_t << wstring_vector_t;
        cdr_ser.serialize_sequence(wchar_seq_t, 5).serialize_sequence(wstring_seq_t, 5);
        cdr_ser.serialize(c_wstring_t);
    });

    // Deserialization.
    FastCdr cdr_des(cdrbuffer);

    uint8_t octet_value = 0;
    char char_value = 0;
    wchar_t wchar_value = 0;
    int8_t int8_value = 0;
    uint16_t ushort_value = 0;
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
    std::array<uint8_t, N_ARR_ELEMENTS> octet_array_value;
    uint8_t octet_array_2_value[N_ARR_ELEMENTS];
    std::array<char, N_ARR_ELEMENTS> char_array_value;
    std::array<wchar_t, N_ARR_ELEMENTS> wchar_array_value;
    char char_array_2_value[N_ARR_ELEMENTS];
    wchar_t wchar_array_2_value[N_ARR_ELEMENTS];
    std::array<int8_t, N_ARR_ELEMENTS> int8_array_value;
    int8_t int8_array_2_value[N_ARR_ELEMENTS];
    std::array<uint16_t, N_ARR_ELEMENTS> ushort_array_value;
    uint16_t ushort_array_2_value[N_ARR_ELEMENTS];
    std::array<int16_t, N_ARR_ELEMENTS> short_array_value;
    int16_t short_array_2_value[N_ARR_ELEMENTS];
    std::array<uint32_t, N_ARR_ELEMENTS> ulong_array_value;
    uint32_t ulong_array_2_value[N_ARR_ELEMENTS];
    std::array<int32_t, N_ARR_ELEMENTS> long_array_value;
    int32_t long_array_2_value[N_ARR_ELEMENTS];
    std::array<uint64_t, N_ARR_ELEMENTS> ulonglong_array_value;
    uint64_t ulonglong_array_2_value[N_ARR_ELEMENTS];
    std::array<int64_t, N_ARR_ELEMENTS> longlong_array_value;
    int64_t longlong_array_2_value[N_ARR_ELEMENTS];
    std::array<float, N_ARR_ELEMENTS> float_array_value;
    float float_array_2_value[N_ARR_ELEMENTS];
    std::array<double, N_ARR_ELEMENTS> double_array_value;
    double double_array_2_value[N_ARR_ELEMENTS];
    std::array<long double, N_ARR_ELEMENTS> ldouble_array_value;
    long double ldouble_array_2_value[N_ARR_ELEMENTS];
    std::array<bool, N_ARR_ELEMENTS> bool_array_value;
    bool bool_array_2_value[N_ARR_ELEMENTS];
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
    uint8_t* octet_seq_value = NULL; size_t octet_seq_len;
    char* char_seq_value = NULL; size_t char_seq_len;
    wchar_t* wchar_seq_value = NULL; size_t wchar_seq_len;
    int8_t* int8_seq_value = NULL; size_t int8_seq_len;
    uint16_t* ushort_seq_value = NULL; size_t ushort_seq_len;
    int16_t* short_seq_value = NULL; size_t short_seq_len;
    uint32_t* ulong_seq_value = NULL; size_t ulong_seq_len;
    int32_t* long_seq_value = NULL; size_t long_seq_len;
    uint64_t* ulonglong_seq_value = NULL; size_t ulonglong_seq_len;
    int64_t* longlong_seq_value = NULL; size_t longlong_seq_len;
    float* float_seq_value = NULL; size_t float_seq_len;
    double* double_seq_value = NULL; size_t double_seq_len;
    long double* ldouble_seq_value = NULL; size_t ldouble_seq_len;
    char* c_string_value = NULL;
    wchar_t* c_wstring_value = NULL;
    std::array<std::wstring, N_ARR_ELEMENTS> wstring_array_value;
    std::wstring wstring_array_2_value[N_ARR_ELEMENTS];
    std::vector<std::wstring> wstring_vector_value;
    std::wstring* wstring_seq_value = NULL; size_t wstring_seq_len;

    EXPECT_NO_THROW(
    {
        cdr_des >> octet_value >> char_value >> int8_value >> ushort_value >> short_value >> ulong_value >> long_value >>
        ulonglong_value >> longlong_value >> float_value >> double_value >> ldouble_value >> bool_value >> string_value;
        cdr_des >> octet_array_value >> char_array_value >> int8_array_value >> ushort_array_value >> short_array_value >>
        ulong_array_value >> long_array_value >> ulonglong_array_value >> longlong_array_value >> float_array_value >>
        double_array_value >> ldouble_array_value >> bool_array_value;
        cdr_des.deserialize_array(octet_array_2_value,
        5).deserialize_array(char_array_2_value, 5).deserialize_array(int8_array_2_value, 5).
                deserialize_array(ushort_array_2_value, 5).deserialize_array(short_array_2_value,
        5).deserialize_array(ulong_array_2_value, 5).
                deserialize_array(long_array_2_value, 5).deserialize_array(ulonglong_array_2_value,
        5).deserialize_array(longlong_array_2_value, 5).
                deserialize_array(float_array_2_value, 5).deserialize_array(double_array_2_value,
        5).deserialize_array(ldouble_array_2_value, 5).deserialize_array(bool_array_2_value,
        5);
        cdr_des >> octet_vector_value >> char_vector_value >> int8_vector_value >> ushort_vector_value >> short_vector_value >>
        ulong_vector_value >> long_vector_value >> ulonglong_vector_value >> longlong_vector_value >> float_vector_value >>
        double_vector_value >> ldouble_vector_value >> bool_vector_value;
        cdr_des >> triple_ulong_array_value;
        cdr_des.deserialize_sequence(octet_seq_value, octet_seq_len).deserialize_sequence(char_seq_value, char_seq_len).
                deserialize_sequence(int8_seq_value, int8_seq_len).deserialize_sequence(ushort_seq_value,
        ushort_seq_len).
                deserialize_sequence(short_seq_value, short_seq_len).deserialize_sequence(ulong_seq_value,
        ulong_seq_len).
                deserialize_sequence(long_seq_value, long_seq_len).deserialize_sequence(ulonglong_seq_value,
        ulonglong_seq_len).
                deserialize_sequence(longlong_seq_value, longlong_seq_len).deserialize_sequence(float_seq_value,
        float_seq_len).
                deserialize_sequence(double_seq_value, double_seq_len).deserialize_sequence(ldouble_seq_value,
        ldouble_seq_len);
        cdr_des.deserialize(c_string_value);
        // WString and wchar_t
        cdr_des >> wchar_value >> wstring_value;
        cdr_des.deserialize_array(wchar_array_2_value, 5).deserialize_array(wstring_array_2_value, 5);
        cdr_des >> wchar_array_value >> wstring_array_value;
        cdr_des >> wchar_vector_value >> wstring_vector_value;
        cdr_des.deserialize_sequence(wchar_seq_value, wchar_seq_len).deserialize_sequence(wstring_seq_value,
        wstring_seq_len);
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

TEST(CDRTests, StringWithNullChars)
{
    std::string str{ "Hello World" };
    str[5] = '\0';
    char buffer[256];
    FastBuffer cdrbuffer(buffer, 256);
    Cdr cdr_ser(cdrbuffer);

    EXPECT_THROW(
    {
        cdr_ser << str;
    },
        BadParamException);
}

TEST(FastCDRTests, StringWithNullChars)
{
    std::string str{ "Hello World" };
    str[5] = '\0';
    char buffer[256];
    FastBuffer cdrbuffer(buffer, 256);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_THROW(
    {
        cdr_ser << str;
    },
        BadParamException);
}

TEST(CDRTests, EmptyStringSerializationSize)
{
    std::string str;
    char buffer[256];
    FastBuffer cdrbuffer(buffer, 256);
    Cdr cdr_ser(cdrbuffer);
    EXPECT_NO_THROW(
    {
        cdr_ser << str;
    });
    EXPECT_EQ(cdr_ser.get_serialized_data_length(), 5u);
}

TEST(FastCDRTests, EmptyStringSerializationSize)
{
    std::string str;
    char buffer[256];
    FastBuffer cdrbuffer(buffer, 256);
    FastCdr cdr_ser(cdrbuffer);
    EXPECT_NO_THROW(
    {
        cdr_ser << str;
    });
    EXPECT_EQ(cdr_ser.get_serialized_data_length(), 5u);
}
