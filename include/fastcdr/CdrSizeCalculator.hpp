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

#ifndef _FASTCDR_CDRSIZECALCULATOR_HPP_
#define _FASTCDR_CDRSIZECALCULATOR_HPP_

#include <array>
#include <cstdint>
#include <limits>
#include <map>
#include <vector>

#include "CdrEncoding.hpp"
#include "fastcdr_dll.h"
#include "cdr/fixed_size_string.hpp"
#include "xcdr/MemberId.hpp"
#include "xcdr/optional.hpp"

namespace eprosima {
namespace fastcdr {

class Cdr_DllAPI CdrSizeCalculator
{
public:

    CdrSizeCalculator(
            CdrVersion cdr_version)
        : cdr_version_(cdr_version)
    {
        switch (cdr_version_)
        {
            case CdrVersion::XCDRv2:
                break;
            default:
                current_encoding_ = EncodingAlgorithmFlag::PLAIN_CDR;
                break;
        }
    }

    CdrVersion get_cdr_version() const
    {
        return cdr_version_;
    }

    EncodingAlgorithmFlag get_encoding() const
    {
        return current_encoding_;
    }

    template<class _T, typename std::enable_if<!std::is_enum<_T>::value>::type* = nullptr>
    inline size_t calculate_serialized_size(
            const _T& data,
            size_t current_alignment = 0)
    {
        return _T::calculate_serialized_size(*this, data, current_alignment);
    }

    template<class _T, typename std::enable_if<std::is_enum<_T>::value &&
            std::is_same<typename std::underlying_type<_T>::type, int32_t>::value>::type* = nullptr>
    inline size_t calculate_serialized_size(
            const _T& data,
            size_t current_alignment = 0)
    {
        return calculate_serialized_size(static_cast<int32_t>(data), current_alignment);
    }

    template<class _T, typename std::enable_if<std::is_enum<_T>::value &&
            std::is_same<typename std::underlying_type<_T>::type, uint32_t>::value>::type* = nullptr>
    inline size_t calculate_serialized_size(
            const _T& data,
            size_t current_alignment = 0)
    {
        return calculate_serialized_size(static_cast<uint32_t>(data), current_alignment);
    }

    template<class _T, typename std::enable_if<std::is_enum<_T>::value &&
            std::is_same<typename std::underlying_type<_T>::type, int16_t>::value>::type* = nullptr>
    inline size_t calculate_serialized_size(
            const _T& data,
            size_t current_alignment = 0)
    {
        return calculate_serialized_size(static_cast<int16_t>(data), current_alignment);
    }

    template<class _T, typename std::enable_if<std::is_enum<_T>::value &&
            std::is_same<typename std::underlying_type<_T>::type, uint16_t>::value>::type* = nullptr>
    inline size_t calculate_serialized_size(
            const _T& data,
            size_t current_alignment = 0)
    {
        return calculate_serialized_size(static_cast<uint16_t>(data), current_alignment);
    }

    template<class _T, typename std::enable_if<std::is_enum<_T>::value &&
            std::is_same<typename std::underlying_type<_T>::type, int8_t>::value>::type* = nullptr>
    inline size_t calculate_serialized_size(
            const _T& data,
            size_t current_alignment = 0)
    {
        return calculate_serialized_size(static_cast<int8_t>(data), current_alignment);
    }

    template<class _T, typename std::enable_if<std::is_enum<_T>::value &&
            std::is_same<typename std::underlying_type<_T>::type, uint8_t>::value>::type* = nullptr>
    inline size_t calculate_serialized_size(
            const _T& data,
            size_t current_alignment = 0)
    {
        return calculate_serialized_size(static_cast<uint8_t>(data), current_alignment);
    }

    inline size_t calculate_serialized_size(
            const int8_t&,
            size_t = 0)
    {
        return 1;
    }

    inline size_t calculate_serialized_size(
            const uint8_t&,
            size_t = 0)
    {
        return 1;
    }

    inline size_t calculate_serialized_size(
            const char&,
            size_t = 0)
    {
        return 1;
    }

    inline size_t calculate_serialized_size(
            const bool&,
            size_t = 0)
    {
        return 1;
    }

    inline size_t calculate_serialized_size(
            const wchar_t&,
            size_t current_alignment = 0)
    {
        return 4 + alignment(current_alignment, 4);
    }

    inline size_t calculate_serialized_size(
            const int16_t&,
            size_t current_alignment = 0)
    {
        return 2 + alignment(current_alignment, 2);
    }

    inline size_t calculate_serialized_size(
            const uint16_t&,
            size_t current_alignment = 0)
    {
        return 2 + alignment(current_alignment, 2);
    }

    inline size_t calculate_serialized_size(
            const int32_t&,
            size_t current_alignment = 0)
    {
        return 4 + alignment(current_alignment, 4);
    }

    inline size_t calculate_serialized_size(
            const uint32_t&,
            size_t current_alignment = 0)
    {
        return 4 + alignment(current_alignment, 4);
    }

    inline size_t calculate_serialized_size(
            const int64_t&,
            size_t current_alignment = 0)
    {
        return 8 + alignment(current_alignment, CdrVersion::XCDRv2 == cdr_version_ ? 4 : 8);
    }

    inline size_t calculate_serialized_size(
            const uint64_t&,
            size_t current_alignment = 0)
    {
        return 8 + alignment(current_alignment, CdrVersion::XCDRv2 == cdr_version_ ? 4 : 8);
    }

    inline size_t calculate_serialized_size(
            const float&,
            size_t current_alignment = 0)
    {
        return 4 + alignment(current_alignment, 4);
    }

    inline size_t calculate_serialized_size(
            const double&,
            size_t current_alignment = 0)
    {
        return 8 + alignment(current_alignment, CdrVersion::XCDRv2 == cdr_version_ ? 4 : 8);
    }

    inline size_t calculate_serialized_size(
            const long double&,
            size_t current_alignment = 0)
    {
        return 16 + alignment(current_alignment, CdrVersion::XCDRv2 == cdr_version_ ? 4 : 8);
    }

    inline size_t calculate_serialized_size(
            const std::string& data,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;

        current_alignment += 4 + alignment(current_alignment, 4) + data.size() + 1;

        return current_alignment - initial_alignment;
    }

    inline size_t calculate_serialized_size(
            const std::wstring& data,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;

        current_alignment += 4 + alignment(current_alignment, 4) + data.size() * 4;

        return current_alignment - initial_alignment;
    }

    template <size_t MAX_CHARS>
    inline size_t calculate_serialized_size(
            const fixed_string<MAX_CHARS>& data,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;

        current_alignment += 4 + alignment(current_alignment, 4) + data.size() + 1;

        return current_alignment - initial_alignment;
    }

#if !defined(_MSC_VER)
    template<class _T = bool>
    inline size_t calculate_serialized_size(
            const std::vector<bool>& data,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;

        current_alignment += data.size() + 4 + alignment(current_alignment, 4);

        return current_alignment - initial_alignment;
    }

#endif // if !defined(_MSC_VER)

    template<class _T>
    inline size_t calculate_serialized_size(
            const std::vector<_T>& data,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;

        current_alignment += 4 + alignment(current_alignment, 4);

        current_alignment += calculate_array_serialized_size(data.data(), data.size(), current_alignment);

        return current_alignment - initial_alignment;
    }

    template<class _T, size_t _Size>
    inline size_t calculate_serialized_size(
            const std::array<_T, _Size>& data,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;

        current_alignment += calculate_array_serialized_size(data.data(), data.size(), current_alignment);

        return current_alignment - initial_alignment;
    }

    template<class _K, class _V>
    inline size_t calculate_serialized_size(
            const std::map<_K, _V>& data,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;

        current_alignment += 4 + alignment(current_alignment, 4);

        for (auto it = data.begin(); it != data.end(); ++it)
        {
            current_alignment += calculate_serialized_size(it->first, current_alignment);
            current_alignment += calculate_serialized_size(it->second, current_alignment);
        }

        return current_alignment - initial_alignment;
    }

    template<class _T>
    inline size_t calculate_serialized_size(
            const optional<_T>& data,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;

        if (CdrVersion::XCDRv2 == cdr_version_ &&
                EncodingAlgorithmFlag::PL_CDR2 != current_encoding_)
        {
            // Take into account the boolean is_present;
            ++current_alignment;
        }

        size_t calculated_size = 0;

        if (data.has_value())
        {
            calculated_size = calculate_serialized_size(data.value(),
                            CdrVersion::XCDRv1 == cdr_version_ ? 0 : current_alignment);
        }

        return (current_alignment  + calculated_size) - initial_alignment;
    }

    template<class _T>
    inline size_t calculate_array_serialized_size(
            const _T* data,
            size_t num_elements,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;

        for (size_t count = 0; count < num_elements; ++count)
        {
            current_alignment += calculate_serialized_size(data[count], current_alignment);
        }

        return current_alignment - initial_alignment;
    }

    inline size_t calculate_array_serialized_size(
            const int8_t*,
            size_t num_elements,
            size_t = 0)
    {

        return num_elements;
    }

    inline size_t calculate_array_serialized_size(
            const uint8_t*,
            size_t num_elements,
            size_t = 0)
    {

        return num_elements;
    }

    inline size_t calculate_array_serialized_size(
            const char*,
            size_t num_elements,
            size_t = 0)
    {

        return num_elements;
    }

    inline size_t calculate_array_serialized_size(
            const wchar_t*,
            size_t num_elements,
            size_t current_alignment = 0)
    {

        return num_elements * 4 + alignment(current_alignment, 4);
    }

    inline size_t calculate_array_serialized_size(
            const int16_t*,
            size_t num_elements,
            size_t current_alignment = 0)
    {

        return num_elements * 2 + alignment(current_alignment, 2);
    }

    inline size_t calculate_array_serialized_size(
            const uint16_t*,
            size_t num_elements,
            size_t current_alignment = 0)
    {

        return num_elements * 2 + alignment(current_alignment, 2);
    }

    inline size_t calculate_array_serialized_size(
            const int32_t*,
            size_t num_elements,
            size_t current_alignment = 0)
    {

        return num_elements * 4 + alignment(current_alignment, 4);
    }

    inline size_t calculate_array_serialized_size(
            const uint32_t*,
            size_t num_elements,
            size_t current_alignment = 0)
    {

        return num_elements * 4 + alignment(current_alignment, 4);
    }

    inline size_t calculate_array_serialized_size(
            const int64_t*,
            size_t num_elements,
            size_t current_alignment = 0)
    {

        return num_elements * 8 + alignment(current_alignment, CdrVersion::XCDRv2 == cdr_version_ ? 4 : 8);
    }

    inline size_t calculate_array_serialized_size(
            const uint64_t*,
            size_t num_elements,
            size_t current_alignment = 0)
    {

        return num_elements * 8 + alignment(current_alignment, CdrVersion::XCDRv2 == cdr_version_ ? 4 : 8);
    }

    inline size_t calculate_array_serialized_size(
            const float*,
            size_t num_elements,
            size_t current_alignment = 0)
    {

        return num_elements * 4 + alignment(current_alignment, 4);
    }

    inline size_t calculate_array_serialized_size(
            const double*,
            size_t num_elements,
            size_t current_alignment = 0)
    {

        return num_elements * 8 + alignment(current_alignment, CdrVersion::XCDRv2 == cdr_version_ ? 4 : 8);
    }

    inline size_t calculate_array_serialized_size(
            const long double*,
            size_t num_elements,
            size_t current_alignment = 0)
    {

        return num_elements * 16 + alignment(current_alignment, CdrVersion::XCDRv2 == cdr_version_ ? 4 : 8);
    }

    template<class _T>
    inline size_t calculate_member_serialized_size(
            const MemberId& id,
            const _T& data,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;

        if (EncodingAlgorithmFlag::PL_CDR == current_encoding_ ||
                EncodingAlgorithmFlag::PL_CDR2 == current_encoding_)
        {
            // Align to 4 because the XCDR header before calculate the data serialized size.
            current_alignment += alignment(current_alignment, 4);
        }

        size_t calculated_size = 0;

        calculated_size = calculate_serialized_size(data,
                        EncodingAlgorithmFlag::PL_CDR == current_encoding_ ? 0 : current_alignment);

        if (CdrVersion::XCDRv2 == cdr_version_ && EncodingAlgorithmFlag::PL_CDR2 == current_encoding_ &&
                0 < calculated_size)
        {

            if (8 < calculated_size)
            {
                calculated_size += 8; // Long EMHEADER.
            }
            else
            {
                calculated_size += 4; // EMHEADER;
            }
        }
        else if (CdrVersion::XCDRv1 == cdr_version_ && EncodingAlgorithmFlag::PL_CDR == current_encoding_ &&
                0 < calculated_size)
        {
            calculated_size += 4; // ShortMemberHeader

            if (0x3F00 < id.id || calculated_size > std::numeric_limits<uint16_t>::max())
            {
                calculated_size += 8; // LongMemberHeader
            }

        }


        return (current_alignment  + calculated_size) - initial_alignment;
    }

    template<class _T>
    inline size_t calculate_member_serialized_size(
            const MemberId& id,
            const optional<_T>& data,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;

        if (CdrVersion::XCDRv2 != cdr_version_ ||
                EncodingAlgorithmFlag::PL_CDR2 == current_encoding_)
        {
            if (data.has_value() || EncodingAlgorithmFlag::PLAIN_CDR == current_encoding_)
            {
                // Align to 4 because the XCDR header before calculate the data serialized size.
                current_alignment += alignment(current_alignment, 4);
            }
        }

        size_t calculated_size = 0;
        calculated_size = calculate_serialized_size(data,
                        CdrVersion::XCDRv1 == cdr_version_ ? 0 : current_alignment);

        if (CdrVersion::XCDRv2 == cdr_version_ && EncodingAlgorithmFlag::PL_CDR2 == current_encoding_ &&
                0 < calculated_size)
        {
            if (8 < calculated_size)
            {
                calculated_size += 8; // Long EMHEADER.
            }
            else
            {
                calculated_size += 4; // EMHEADER;
            }
        }
        else if (CdrVersion::XCDRv1 == cdr_version_ &&
                (0 < calculated_size || EncodingAlgorithmFlag::PLAIN_CDR == current_encoding_))
        {
            calculated_size += 4; // ShortMemberHeader

            if (0x3F00 < id.id || calculated_size > std::numeric_limits<uint16_t>::max())
            {
                calculated_size += 8; // LongMemberHeader
            }

        }


        return (current_alignment  + calculated_size) - initial_alignment;
    }

    inline size_t begin_calculate_type_serialized_size(
            EncodingAlgorithmFlag new_encoding,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;
        current_encoding_ = new_encoding;

        if (CdrVersion::XCDRv2 == cdr_version_ && EncodingAlgorithmFlag::PLAIN_CDR2 != current_encoding_)
        {
            current_alignment += 4 + alignment(current_alignment, 4); // DHEADER
        }

        return current_alignment - initial_alignment;
    }

    inline size_t end_calculate_type_serialized_size(
            EncodingAlgorithmFlag new_encoding,
            size_t current_alignment = 0)
    {
        size_t initial_alignment = current_alignment;

        if (CdrVersion::XCDRv1 == cdr_version_ && EncodingAlgorithmFlag::PL_CDR == current_encoding_)
        {
            current_alignment += 4 + alignment(current_alignment, 4); // Sentinel
        }

        current_encoding_ = new_encoding;
        return current_alignment - initial_alignment;
    }

private:

    CdrSizeCalculator() = delete;

    CdrVersion cdr_version_;

    EncodingAlgorithmFlag current_encoding_ {EncodingAlgorithmFlag::PLAIN_CDR2};

    inline size_t alignment(
            size_t current_alignment,
            size_t dataSize) const
    {
        return (dataSize - (current_alignment % dataSize)) & (dataSize - 1);
    }

};

} // namespace fastcdr
} // namespace eprosima

#endif // _FASTCDR_CDRSIZECALCULATOR_HPP_
