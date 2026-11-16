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

#include <cstddef>
#include <fastcdr/Cdr.h>
#include <fastcdr/FastCdr.h>

#include <gtest/gtest.h>

using namespace eprosima::fastcdr;

#define BUFFER_LENGTH 2000
#define N_ARR_ELEMENTS 5

static const bool expected_bool_value{true};
static const bool expected_array_value[N_ARR_ELEMENTS]{true, false, true, false, true};
static const std::array<bool, N_ARR_ELEMENTS> expected_std_array_value{true, false, true, false, true};
static const std::vector<bool> expected_std_vector_value{true, false, true, false, true};

template<typename T>
void EXPECT_ARRAY_EQ(
        T * array1,
        const T * array2,
        size_t size)
{
    for (size_t count = 0; count < size; ++count)
    {
        EXPECT_EQ(array1[count], array2[count]);
    }
}

bool make_bool_truthy(
        uint8_t raw = 0xFF)
{
    bool bool_t{};
    std::memcpy(&bool_t, &raw, 1);
    return bool_t;
}

void make_bool_array_truthy(
        bool * bool_array,
        size_t len,
        uint8_t raw = 0xFF)
{
    for (size_t i = 0; i < len; i += 2)
    {
        std::memcpy(&bool_array[i], &raw, 1);
    }
}

std::array<bool, N_ARR_ELEMENTS> make_bool_std_array_truthy(
        uint8_t raw = 0xFF)
{
    std::array<bool, N_ARR_ELEMENTS> bool_array{};
    make_bool_array_truthy(bool_array.data(), bool_array.size(), raw);

    return bool_array;
}

std::vector<bool> make_bool_std_vector_truthy(
        uint8_t raw = 0xFF)
{
    std::vector<bool> bool_vector(N_ARR_ELEMENTS, false);
    bool value{false};
    std::memcpy(&value, &raw, 1);
    for (size_t i = 0; i < N_ARR_ELEMENTS; i += 2)
    {
        bool_vector[i] = value;
    }
    return bool_vector;
}

template<typename T>
static void check_good_case(
        const T& input_value,
        const T& expected_value)
{
    char buffer[BUFFER_LENGTH];
    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_ser(cdrbuffer,
                eprosima::fastcdr::Cdr::Endianness::LITTLE_ENDIANNESS);
        EXPECT_NO_THROW(cdr_ser << input_value);
    }

    {
        T output_value{};
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_des(cdrbuffer,
                eprosima::fastcdr::Cdr::Endianness::LITTLE_ENDIANNESS);
        EXPECT_NO_THROW(cdr_des >> output_value);
        EXPECT_EQ(output_value, expected_value);
    }

    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_ser(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);
        EXPECT_NO_THROW(cdr_ser << input_value);
    }

    {
        T output_value{};
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_des(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);
        EXPECT_NO_THROW(cdr_des >> output_value);
        EXPECT_EQ(output_value, expected_value);
    }
}

template<typename T, size_t N = N_ARR_ELEMENTS>
static void check_good_case_array(
        const T& input_value,
        const T& expected_value)
{
    char buffer[BUFFER_LENGTH];
    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_ser(cdrbuffer,
                eprosima::fastcdr::Cdr::Endianness::LITTLE_ENDIANNESS);
        EXPECT_NO_THROW(cdr_ser.serialize_array(input_value, N));
    }

    {
        T output_value{};
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_des(cdrbuffer,
                eprosima::fastcdr::Cdr::Endianness::LITTLE_ENDIANNESS);
        EXPECT_NO_THROW(cdr_des.deserialize_array(output_value, N));
        EXPECT_ARRAY_EQ(output_value, expected_value, N);
    }

    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_ser(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);
        EXPECT_NO_THROW(cdr_ser.serialize_array(input_value, N));
    }

    {
        T output_value{};
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_des(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);
        EXPECT_NO_THROW(cdr_des.deserialize_array(output_value, N));
        EXPECT_ARRAY_EQ(output_value, expected_value, N);
    }
}

TEST(CDRBoolStrictTests, BooleanTruthy)
{
    const bool bool_t{make_bool_truthy()};
    check_good_case(bool_t, expected_bool_value);
}

TEST(CDRBoolStrictTests, STDArrayBooleanTruthy)
{
    std::array<bool, N_ARR_ELEMENTS> bool_array_t{make_bool_std_array_truthy()};
    check_good_case(bool_array_t, expected_std_array_value);
}

TEST(CDRBoolStrictTests, ArrayBooleanTruthy)
{
    bool bool_array_t[N_ARR_ELEMENTS]{};
    make_bool_array_truthy(bool_array_t, N_ARR_ELEMENTS);
    check_good_case_array(bool_array_t, expected_array_value);

    char buffer[BUFFER_LENGTH];
    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_ser(cdrbuffer,
                eprosima::fastcdr::Cdr::Endianness::LITTLE_ENDIANNESS);
        EXPECT_NO_THROW(cdr_ser.serialize_array(bool_array_t, N_ARR_ELEMENTS));
    }

    {
        bool output_value[N_ARR_ELEMENTS]{};
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_des(cdrbuffer,
                eprosima::fastcdr::Cdr::Endianness::LITTLE_ENDIANNESS);
        EXPECT_NO_THROW(cdr_des.deserialize_array(output_value, N_ARR_ELEMENTS));
        EXPECT_ARRAY_EQ(output_value, expected_array_value, N_ARR_ELEMENTS);
    }

    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_ser(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);
        EXPECT_NO_THROW(cdr_ser.serialize_array(bool_array_t, N_ARR_ELEMENTS));
    }

    {
        bool output_value[N_ARR_ELEMENTS]{};
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_des(cdrbuffer, eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);
        EXPECT_NO_THROW(cdr_des.deserialize_array(output_value, N_ARR_ELEMENTS));
        EXPECT_ARRAY_EQ(output_value, expected_array_value, N_ARR_ELEMENTS);
    }
}

TEST(CDRBoolStrictTests, STDVectorBooleanTruthy)
{
    std::vector<bool> bool_vector_t{make_bool_std_vector_truthy()};
    check_good_case(bool_vector_t, expected_std_vector_value);
}

TEST(CDRBoolStrictTests, SequenceBooleanTruthy)
{
    bool bool_array_t[N_ARR_ELEMENTS]{};
    make_bool_array_truthy(bool_array_t, N_ARR_ELEMENTS);

    char buffer[BUFFER_LENGTH];
    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_ser(cdrbuffer,
                eprosima::fastcdr::Cdr::Endianness::LITTLE_ENDIANNESS);
        EXPECT_NO_THROW(cdr_ser.serialize_sequence(bool_array_t, N_ARR_ELEMENTS));
    }

    {
        bool * output_value{nullptr};
        size_t seq_len{0};
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_des(cdrbuffer,
                eprosima::fastcdr::Cdr::Endianness::LITTLE_ENDIANNESS);
        EXPECT_NO_THROW(cdr_des.deserialize_sequence(output_value, seq_len));
        EXPECT_EQ(seq_len, N_ARR_ELEMENTS);
        EXPECT_ARRAY_EQ(output_value, expected_array_value, seq_len);
        free(output_value);
    }

    {
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_ser(cdrbuffer,
                eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);
        EXPECT_NO_THROW(cdr_ser.serialize_sequence(bool_array_t, N_ARR_ELEMENTS));
    }

    {
        bool * output_value{nullptr};
        size_t seq_len{0};
        FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
        Cdr cdr_des(cdrbuffer,
                eprosima::fastcdr::Cdr::Endianness::BIG_ENDIANNESS);
        EXPECT_NO_THROW(cdr_des.deserialize_sequence(output_value, seq_len));
        EXPECT_EQ(seq_len, N_ARR_ELEMENTS);
        EXPECT_ARRAY_EQ(output_value, expected_array_value, seq_len);
        free(output_value);
    }
}

TEST(FastCDRBoolStrictTests, BooleanTruthy)
{
    bool bool_t{make_bool_truthy()};

    char buffer[BUFFER_LENGTH];

    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW({ cdr_ser << bool_t; });

    FastCdr cdr_des(cdrbuffer);

    bool output_value{};

    EXPECT_NO_THROW({ cdr_des >> output_value; });

    EXPECT_EQ(output_value, expected_bool_value);
}

TEST(FastCDRBoolStrictTests, STDArrayBooleanTruthy)
{
    std::array<bool, N_ARR_ELEMENTS> bool_array_t{make_bool_std_array_truthy()};

    char buffer[BUFFER_LENGTH];

    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW({ cdr_ser << bool_array_t; });

    FastCdr cdr_des(cdrbuffer);

    std::array<bool, N_ARR_ELEMENTS> bool_array_value;

    EXPECT_NO_THROW({ cdr_des >> bool_array_value; });

    EXPECT_EQ(bool_array_value, expected_std_array_value);
}

TEST(FastCDRBoolStrictTests, ArrayBooleanTruthy)
{
    bool bool_array_t[N_ARR_ELEMENTS]{};
    make_bool_array_truthy(bool_array_t, N_ARR_ELEMENTS);

    char buffer[BUFFER_LENGTH];

    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW({ cdr_ser.serialize_array(bool_array_t, N_ARR_ELEMENTS); });

    FastCdr cdr_des(cdrbuffer);

    bool bool_array_value[N_ARR_ELEMENTS];

    EXPECT_NO_THROW({ cdr_des.deserialize_array(bool_array_value, N_ARR_ELEMENTS); });

    EXPECT_ARRAY_EQ(bool_array_value, expected_array_value, N_ARR_ELEMENTS);
}

TEST(FastCDRBoolStrictTests, STDVectorBooleanTruthy)
{
    std::vector<bool> bool_vector_t{make_bool_std_vector_truthy()};

    char buffer[BUFFER_LENGTH];

    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW({ cdr_ser << bool_vector_t; });

    FastCdr cdr_des(cdrbuffer);

    std::vector<bool> bool_vector_value;

    EXPECT_NO_THROW({ cdr_des >> bool_vector_value; });

    EXPECT_EQ(bool_vector_value, expected_std_vector_value);
}

TEST(FastCDRBoolStrictTests, SequenceBooleanTruthy)
{
    bool bool_array_t[N_ARR_ELEMENTS]{};
    make_bool_array_truthy(bool_array_t, N_ARR_ELEMENTS);

    char buffer[BUFFER_LENGTH];

    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    EXPECT_NO_THROW(cdr_ser.serialize_sequence(bool_array_t, N_ARR_ELEMENTS));

    bool * output_value{nullptr};
    size_t seq_len{0};

    FastCdr cdr_des(cdrbuffer);

    EXPECT_NO_THROW(cdr_des.deserialize_sequence(output_value, seq_len));
    EXPECT_EQ(seq_len, N_ARR_ELEMENTS);
    EXPECT_ARRAY_EQ(output_value, expected_array_value, seq_len);

    free(output_value);
}
