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

#ifndef _FASTCDR_CDRCONTEXT_HPP_
#define _FASTCDR_CDRCONTEXT_HPP_

#include "fastcdr_dll.h"

namespace eprosima {
namespace fastcdr {

/**
 * @brief This structure represents the context of a CDR serialization.
 *
 * Can be used by external (de)serialization functions to customize the serialization of a type.
 * The Cdr object can be instantiated with a shared pointer to a Context object, and a getter for the context is
 * provided in the Cdr class so it can be accessed from the external (de)serialization functions.
 */
struct Cdr_DllAPI CdrContext
{
    // Default virtual destructor to allow proper cleanup of derived classes, and to make sure the class
    // is polymorphic so it can be used with dynamic_pointer_cast.
    virtual ~CdrContext() = default;
};

}  // namespace fastcdr
}  // namespace eprosima

#endif // _FASTCDR_CDRCONTEXT_HPP_
