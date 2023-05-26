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
//

#include <cstdint>

namespace eprosima {
namespace fastcdr {

class Cdr;

class MemberId
{
public:

    MemberId() = default;

    MemberId(
            uint32_t id_value)
        : id(id_value)
    {
    }

    bool operator ==(
            uint32_t id_value)
    {
        return id == id_value;
    }

    bool operator ==(
            const MemberId member_id)
    {
        return id == member_id.id;
    }

    uint32_t id = member_id_invalid_value_;

private:

    static constexpr uint32_t member_id_invalid_value_ = 0xFFFFFFFF;

};

static constexpr MemberId MEMBER_ID_INVALID {};

} // namespace fastcdr
} // namespace eprosima
