// Copyright 2025 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <fastcdr/detail/container_introspection_helpers.hpp>
#include <gtest/gtest.h>

using namespace eprosima::fastcdr;

struct ContainerIntrospectionTests : public ::testing::Test
{
    struct CustomType {};
};

TEST_F(ContainerIntrospectionTests, static_is_multi_array_primitive)
{
    /// Positive tests
    static_assert(static_is_multi_array_primitive<int const*>::value, "int* should be a multi primitive array");
    static_assert(static_is_multi_array_primitive<float const*>::value, "float* should be a multi primitive array");
    static_assert(static_is_multi_array_primitive<double const*>::value, "double* should be a multi primitive array");
    static_assert(static_is_multi_array_primitive<long double const*>::value,
            "long double* should be a multi primitive array");
    static_assert(static_is_multi_array_primitive<char const*>::value, "char* should be a multi primitive array");
    static_assert(static_is_multi_array_primitive<bool const*>::value, "bool* should be a multi primitive array");
    static_assert(static_is_multi_array_primitive<uint8_t const*>::value, "uint8_t* should be a multi primitive array");
    static_assert(static_is_multi_array_primitive<uint16_t const*>::value,
            "uint16_t* should be a multi primitive array");
    static_assert(static_is_multi_array_primitive<uint32_t const*>::value,
            "uint32_t* should be a multi primitive array");
    static_assert(static_is_multi_array_primitive<uint64_t const*>::value,
            "uint64_t* should be a multi primitive array");
    static_assert(static_is_multi_array_primitive<std::array<int, 3> const*>::value,
            "std::array<int, 3>* should be a multi primitive array");
    static_assert(static_is_multi_array_primitive<std::array<std::array<int, 3>, 3> const*>::value,
            "array of arrays should be a multi primitive array");

    /// Negative tests
    static_assert(!static_is_multi_array_primitive<CustomType const*>::value,
            "CustomType* should not be a multi primitive array");
    static_assert(!static_is_multi_array_primitive<std::string const*>::value,
            "std::string* should not be a multi primitive array");
    static_assert(!static_is_multi_array_primitive<std::vector<int> const*>::value,
            "std::vector<int>* should not be a multi primitive array");
    static_assert(!static_is_multi_array_primitive<std::map<int, int> const*>::value,
            "std::map<int, int>* should not be a multi primitive array");
    static_assert(!static_is_multi_array_primitive<std::array<std::vector<int>, 3> const*>::value,
            "array of vectors should be a multi primitive array");
    static_assert(!static_is_multi_array_primitive<std::array<std::map<int, int>, 3> const*>::value,
            "array of maps should be a multi primitive array");
}

TEST_F(ContainerIntrospectionTests, is_complex_array_or_string)
{
    /// Positive tests
    static_assert(is_complex_array_or_string<std::array<std::array<int, 10u>, 10u>>::value,
            "array of arrays should be a complex array or string");
    static_assert(is_complex_array_or_string<std::array<std::vector<int>, 10u>>::value,
            "array of vectors should be a complex array or string");
    static_assert(is_complex_array_or_string<std::array<std::map<int, int>, 10u>>::value,
            "array of maps should be a complex array or string");
    static_assert(is_complex_array_or_string<std::array<std::string, 10u>>::value,
            "array of strings should be a complex array or string");
    static_assert(is_complex_array_or_string<std::array<std::wstring, 10u>>::value,
            "array of wide strings should be a complex array or string");
    static_assert(is_complex_array_or_string<std::array<fixed_string<10u>, 10u>>::value,
            "array of fixed strings should be a complex array or string");

    static_assert(is_complex_array_or_string<std::vector<std::array<int, 10u>>>::value,
            "vector of arrays should be a complex array or string");
    static_assert(is_complex_array_or_string<std::vector<std::vector<int>>>::value,
            "vector of vectors should be a complex array or string");
    static_assert(is_complex_array_or_string<std::vector<std::map<int, int>>>::value,
            "vector of maps should be a complex array or string");
    static_assert(is_complex_array_or_string<std::vector<std::string>>::value,
            "vector of strings should be a complex array or string");
    static_assert(is_complex_array_or_string<std::vector<std::wstring>>::value,
            "vector of wide strings should be a complex array or string");
    static_assert(is_complex_array_or_string<std::vector<fixed_string<10u>>>::value,
            "vector of fixed strings should be a complex array or string");

    static_assert(is_complex_array_or_string<std::map<int, int>>::value,
            "map of ints should be a complex array or string");
    static_assert(is_complex_array_or_string<std::map<float, int>>::value,
            "map of float should be a complex array or string");
    static_assert(is_complex_array_or_string<std::map<CustomType, int>>::value,
            "map of CustomType should be a complex array or string");

    /// Negative tests
    static_assert(!is_complex_array_or_string<std::array<int, 10u>>::value,
            "array of ints should not be a complex array or string");
    static_assert(!is_complex_array_or_string<std::array<float, 10u>>::value,
            "array of float should not be a complex array or string");
    static_assert(!is_complex_array_or_string<std::array<CustomType, 10u>>::value,
            "array of CustomType should not be a complex array or string");

    static_assert(!is_complex_array_or_string<std::vector<int>>::value,
            "vector of ints should not be a complex array or string");
    static_assert(!is_complex_array_or_string<std::vector<float>>::value,
            "vector of float should not be a complex array or string");
    static_assert(!is_complex_array_or_string<std::vector<CustomType>>::value,
            "vector of CustomType should not be a complex array or string");
}
