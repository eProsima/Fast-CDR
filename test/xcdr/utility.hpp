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

#ifndef _TEST_XCDR_UTILITY_HPP_
#define _TEST_XCDR_UTILITY_HPP_

#include <fastcdr/Cdr.h>

static eprosima::fastcdr::CdrVersion get_version_from_algorithm(
        eprosima::fastcdr::EncodingAlgorithmFlag ef)
{
    eprosima::fastcdr::CdrVersion cdr_version { eprosima::fastcdr::CdrVersion::XCDRv2 };

    switch (ef)
    {
        case eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR:
        case eprosima::fastcdr::EncodingAlgorithmFlag::PL_CDR:
            cdr_version = eprosima::fastcdr::CdrVersion::XCDRv1;
            break;
        default:
            break;
    }

    return cdr_version;
}

#endif // _TEST_XCDR_UTILITY_HPP_
