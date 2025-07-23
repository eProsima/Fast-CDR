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

#ifndef _FASTCDR_DETAIL_CONTAINER_INTROSPECTION_HELPERS_HPP_
#define _FASTCDR_DETAIL_CONTAINER_INTROSPECTION_HELPERS_HPP_

#include <array>
#include <cstddef>
#include <map>
#include <type_traits>
#include <vector>

#include <fastcdr/cdr/fixed_size_string.hpp>

namespace eprosima {
namespace fastcdr {

/// Helper trait to deduce multi-array of primitives.
/// Primary template — false by default
template<typename T>
struct static_is_multi_array_primitive : std::false_type {};

/// Specialization: base case for arithmetic or enum types
template<typename T>
struct static_is_multi_array_primitive<T const*>
    : std::integral_constant<
        bool,
        std::is_arithmetic<T>::value || std::is_enum<T>::value
        > {};

/// Recursive case: std::array<T, N> → peel one layer and recurse
template<typename T, std::size_t N>
struct static_is_multi_array_primitive<std::array<T, N> const*>
    : static_is_multi_array_primitive<T const*> {};

/// Helper trait to deduce if a type is a string like
template<typename T>
struct is_string_like : std::false_type {};

template<>
struct is_string_like<std::string> : std::true_type {};

template<>
struct is_string_like<std::wstring> : std::true_type {};

template<std::size_t N>
struct is_string_like<fixed_string<N>> : std::true_type {};

/// Helper trait to deduce if a type is a container type
template<typename T>
struct is_container : std::false_type {};

template<typename T, typename Alloc>
struct is_container<std::vector<T, Alloc>> : std::true_type {};

template<typename K, typename V, typename Compare, typename Alloc>
struct is_container<std::map<K, V, Compare, Alloc>> : std::true_type {};

template<typename T, std::size_t N>
struct is_container<std::array<T, N>> : std::true_type {};

/// Helper trait to deduce if the type of
//  a std::array or std::vector is either
//      * non-array like (i.e std::map, for instance)
//      * contains nested containers
//      * is string like
// Default: always true for non-arrays
template<typename T>
struct is_complex_array_or_string : std::true_type {};

// Specialization for std::array
// Check if T is string-like or complex
template<typename T, std::size_t N>
struct is_complex_array_or_string<std::array<T, N>>
    : std::integral_constant<
        bool,
        is_string_like<T>::value || is_container<T>::value
        > {};

// Specialization for std::vector
// check if T is string-like or complex
template<typename T>
struct is_complex_array_or_string<std::vector<T>>
    : std::integral_constant<
        bool,
        is_string_like<T>::value || is_container<T>::value
        > {};

} // namespace fastcdr
} // namespace eprosima

#endif // _FASTCDR_DETAIL_CONTAINER_INTROSPECTION_HELPERS_HPP_
