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

#include <fastcdr/CdrSizeCalculator.hpp>

namespace eprosima {
namespace fastcdr {

CdrSizeCalculator::CdrSizeCalculator(
        CdrVersion cdr_version)
    : cdr_version_(cdr_version)
{
    switch (cdr_version_)
    {
        case CdrVersion::XCDRv2:
            break;
        default:
            current_encoding_ = EncodingAlgorithmFlag::PLAIN_CDR;
            align64_ = 8;
            break;
    }
}

CdrVersion CdrSizeCalculator::get_cdr_version() const
{
    return cdr_version_;
}

EncodingAlgorithmFlag CdrSizeCalculator::get_encoding() const
{
    return current_encoding_;
}

size_t CdrSizeCalculator::begin_calculate_type_serialized_size(
        EncodingAlgorithmFlag new_encoding,
        size_t current_alignment)
{
    size_t initial_alignment = current_alignment;
    current_encoding_ = new_encoding;

    if (CdrVersion::XCDRv2 == cdr_version_ && EncodingAlgorithmFlag::PLAIN_CDR2 != current_encoding_)
    {
        current_alignment += 4 + alignment(current_alignment, 4);     // DHEADER
    }

    serialized_member_size_ = NO_SERIALIZED_MEMBER_SIZE;     // Avoid error when serializen arrays, sequences, etc..

    return current_alignment - initial_alignment;
}

size_t CdrSizeCalculator::end_calculate_type_serialized_size(
        EncodingAlgorithmFlag new_encoding,
        size_t current_alignment)
{
    size_t initial_alignment = current_alignment;

    if (CdrVersion::XCDRv1 == cdr_version_ && EncodingAlgorithmFlag::PL_CDR == current_encoding_)
    {
        current_alignment += 4 + alignment(current_alignment, 4);     // Sentinel
    }
    else if (CdrVersion::XCDRv2 == cdr_version_ && EncodingAlgorithmFlag::PLAIN_CDR2 != current_encoding_)
    {
        serialized_member_size_ = SERIALIZED_MEMBER_SIZE;
    }

    current_encoding_ = new_encoding;
    return current_alignment - initial_alignment;
}

} // namespace fastcdr
} // namespace eprosima
