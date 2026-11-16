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

#ifndef _SRC_CPP_HELPERS_MEMORYHELPERS_HPP_
#define _SRC_CPP_HELPERS_MEMORYHELPERS_HPP_

#include <atomic>
#include <type_traits>
#include <utility>
#include <cstring>

namespace eprosima {
namespace fastcdr {

namespace internal {
void use_char_pointer(
        char const volatile *const v);
} // namespace internal

#if (!defined(__GNUC__) && !defined(__clang__)) || defined(__pnacl__) || \
    defined(__EMSCRIPTEN__)
#define HAS_NO_INLINE_ASSEMBLY
#endif // if (!defined(__GNUC__) && !defined(__clang__)) || defined(__pnacl__) || defined(__EMSCRIPTEN__)

inline void clobber_memory()
{
    std::atomic_signal_fence(std::memory_order_acq_rel);
}

#ifndef HAS_NO_INLINE_ASSEMBLY
#if !defined(__GNUC__) || defined(__llvm__) || defined(__INTEL_COMPILER)
template<class Tp>
inline void do_not_optimize(
        Tp& value)
{
#if defined(__clang__)
    asm volatile ("" : "+r,m" (value) : : "memory");
#else
    asm volatile ("" : "+m,r" (value) : : "memory");
#endif // if defined(__clang__)
}

template<class Tp>
inline void do_not_optimize(
        Tp&& value)
{
#if defined(__clang__)
    asm volatile ("" : "+r,m" (value) : : "memory");
#else
    asm volatile ("" : "+m,r" (value) : : "memory");
#endif // if defined(__clang__)
}

#elif (__GNUC__ >= 5)
template<class Tp>
inline typename std::enable_if<std::is_trivially_copyable<Tp>::value&&
        (sizeof(Tp) <= sizeof(Tp*))>::type
do_not_optimize(
        Tp& value)
{
    asm volatile ("" : "+m,r" (value) : : "memory");
}

template<class Tp>
inline typename std::enable_if<!std::is_trivially_copyable<Tp>::value ||
        (sizeof(Tp) > sizeof(Tp*))>::type
do_not_optimize(
        Tp& value)
{
    asm volatile ("" : "+m" (value) : : "memory");
}

template<class Tp>
inline typename std::enable_if<std::is_trivially_copyable<Tp>::value&&
        (sizeof(Tp) <= sizeof(Tp*))>::type
do_not_optimize(
        Tp&& value)
{
    asm volatile ("" : "+m,r" (value) : : "memory");
}

template<class Tp>
inline typename std::enable_if<!std::is_trivially_copyable<Tp>::value ||
        (sizeof(Tp) > sizeof(Tp*))>::type
do_not_optimize(
        Tp&& value)
{
    asm volatile ("" : "+m" (value) : : "memory");
}

#endif // if !defined(__GNUC__) || defined(__llvm__) || defined(__INTEL_COMPILER)

#elif defined(_MSC_VER)
template<class Tp>
inline void do_not_optimize(
        Tp& value)
{
    internal::use_char_pointer(&reinterpret_cast<char const volatile&>(value));
    clobber_memory();
}

template<class Tp>
inline void do_not_optimize(
        Tp&& value)
{
    internal::use_char_pointer(&reinterpret_cast<char const volatile&>(value));
    clobber_memory();
}

#else
template<class Tp>
inline void do_not_optimize(
        Tp&& value)
{
    internal::use_char_pointer(&reinterpret_cast<char const volatile&>(value));
}

#endif // ifndef HAS_NO_INLINE_ASSEMBLY

/*!
 * @brief Normalizes a boolean value to a canonical 0 or 1 representation.
 *
 * In release builds, the compiler may optimize away explicit normalization
 * of bool values (e.g. `if (b) { write 1; } else { write 0; }`), resulting
 * in the raw underlying byte being written directly to the serialization
 * buffer. If that byte contains garbage (e.g. from an uninitialized or
 * improperly assigned field), a non-canonical value ends up on the wire,
 * causing deserialization failures on the receiving side.
 *
 * This function forces normalization by reading the bool's raw byte via
 * @c memcpy and passing it through a compiler barrier (@c do_not_optimize)
 * preventing the optimization from eliding the check.
 *
 * @param b The boolean value to normalize.
 * @return @c true if the underlying byte is non-zero, @c false otherwise.
 */
inline bool normalize_bool(
        bool b) noexcept
{
    uint8_t raw;
    std::memcpy(&raw, &b, 1);
    do_not_optimize(raw);
    return static_cast<bool>(raw);
}

} // namespace fastcdr
} // namespace eprosima

#endif // _SRC_CPP_HELPERS_MEMORYHELPERS_HPP_
