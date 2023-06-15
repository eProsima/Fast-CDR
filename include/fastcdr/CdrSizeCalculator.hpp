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

#include <cstdint>
#include <limits>

#include "CdrEncoding.hpp"
#include "fastcdr_dll.h"
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

    void set_enconding(
            EncodingAlgorithmFlag encoding)
    {
        current_encoding_ = encoding;
    }

    template<class _T>
    inline size_t calculate_serialized_size(
            const _T& data,
            size_t current_alignment = 0) const
    {
        return _T::calculate_serialized_size(*this, data, current_alignment);
    }

    inline constexpr size_t calculate_serialized_size(
            const uint8_t&,
            size_t = 0) const
    {
        return 1;
    }

    inline constexpr size_t calculate_serialized_size(
            const int8_t&,
            size_t = 0) const
    {
        return 1;
    }

    inline constexpr size_t calculate_serialized_size(
            const char&,
            size_t = 0) const
    {
        return 1;
    }

    inline constexpr size_t calculate_serialized_size(
            const bool&,
            size_t = 0) const
    {
        return 1;
    }

    inline size_t calculate_serialized_size(
            const wchar_t&,
            size_t current_alignment = 0) const
    {
        return 4 + alignment(current_alignment, 4);
    }

    inline size_t calculate_serialized_size(
            const int16_t&,
            size_t current_alignment = 0) const
    {
        return 2 + alignment(current_alignment, 2);
    }

    inline size_t calculate_serialized_size(
            const uint16_t&,
            size_t current_alignment = 0) const
    {
        return 2 + alignment(current_alignment, 2);
    }

    inline size_t calculate_serialized_size(
            const int32_t&,
            size_t current_alignment = 0) const
    {
        return 4 + alignment(current_alignment, 4);
    }

    inline size_t calculate_serialized_size(
            const uint32_t&,
            size_t current_alignment = 0) const
    {
        return 4 + alignment(current_alignment, 4);
    }

    inline size_t calculate_serialized_size(
            const int64_t&,
            size_t current_alignment = 0) const
    {
        return 8 + alignment(current_alignment, CdrVersion::XCDRv2 == cdr_version_ ? 4 : 8);
    }

    inline size_t calculate_serialized_size(
            const uint64_t&,
            size_t current_alignment = 0) const
    {
        return 8 + alignment(current_alignment, CdrVersion::XCDRv2 == cdr_version_ ? 4 : 8);
    }

    inline size_t calculate_serialized_size(
            const float&,
            size_t current_alignment = 0) const
    {
        return 4 + alignment(current_alignment, 4);
    }

    inline size_t calculate_serialized_size(
            const double&,
            size_t current_alignment = 0) const
    {
        return 8 + alignment(current_alignment, CdrVersion::XCDRv2 == cdr_version_ ? 4 : 8);
    }

    inline size_t calculate_serialized_size(
            const long double&,
            size_t current_alignment = 0) const
    {
        return 16 + alignment(current_alignment, CdrVersion::XCDRv2 == cdr_version_ ? 4 : 8);
    }

    template<class _T>
    inline size_t calculate_serialized_size(
            const MemberId& id,
            const optional<_T>& data,
            size_t current_alignment = 0) const
    {
        size_t initial_alignment = current_alignment;

        if (CdrVersion::XCDRv2 != cdr_version_ ||
                (EncodingAlgorithmFlag::PLAIN_CDR2 != current_encoding_ &&
                EncodingAlgorithmFlag::DELIMIT_CDR2 != current_encoding_))
        {
            if (data.has_value() || EncodingAlgorithmFlag::PLAIN_CDR == current_encoding_)
            {
                // Align to 4 because the XCDR header before calculate the data serialized size.
                current_alignment += alignment(current_alignment, 4);
            }
        }
        else
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

        if (CdrVersion::XCDRv2 == cdr_version_ && EncodingAlgorithmFlag::PL_CDR2 == current_encoding_ &&
                0 < calculated_size)
        {
            calculated_size += 4; // EMHEADER;

            if (4 < calculated_size)
            {
                calculated_size += 4; // Long header.
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
