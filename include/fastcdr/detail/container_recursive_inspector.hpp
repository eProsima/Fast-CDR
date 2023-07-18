#ifndef _FASTCDR_DETAIL_CONTAINERRECURSIVEINSPECTOR_HPP_
#define _FASTCDR_DETAIL_CONTAINERRECURSIVEINSPECTOR_HPP_

#include <array>
#include <cstddef>
#include <type_traits>

namespace eprosima {
namespace fastcdr {

// Helpers to deduce multi-array of primitives.
/// Basis
constexpr bool is_multi_array_primitive(
        ...)
{
    return false;
}

/// Specializations
template <typename _T,
        typename std::enable_if<std::is_enum<_T>::value ||
        std::is_arithmetic<_T>::value>::type* = nullptr>
constexpr bool is_multi_array_primitive(
        _T const*)
{
    return true;
}

template <typename _T, size_t _N>
constexpr bool is_multi_array_primitive(
        std::array<_T, _N> const*)
{
    return is_multi_array_primitive(static_cast<_T const*>(nullptr));
}

} // namespace fastcdr
} // namespace eprosima

#endif // _FASTCDR_DETAIL_CONTAINERRECURSIVEINSPECTOR_HPP_
