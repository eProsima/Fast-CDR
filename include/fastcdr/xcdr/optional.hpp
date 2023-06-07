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
#ifndef _FASTCDR_XCDR_OPTIONAL_HPP_
#define _FASTCDR_XCDR_OPTIONAL_HPP_

#include <new>
#include <type_traits>

#include "../exceptions/BadParamException.h"

namespace eprosima {
namespace fastcdr {

namespace detail {
template<class T, class E = void>
struct optional_storage
{
    union
    {
        char dummy_; T val_;
    };

    bool engaged_ { false };

    optional_storage()
    {
    }

    ~optional_storage()
    {
        if (engaged_)
        {
            val_.~T();
        }
    }

};

/* *INDENT-OFF* */
template<class T>
struct optional_storage<T, typename std::enable_if<std::is_trivially_destructible<T>{}>::type>
{
    union
    {
        char dummy_; T val_;
    };

    bool engaged_ { false };

    optional_storage() = default;

    ~optional_storage() = default;
};
/* *INDENT-ON* */
} // namespace detail

struct nullopt_t
{
    constexpr explicit nullopt_t(
            int)
    {
    }

};

template<class T>
class optional
{
public:

    optional() = default;

    optional(
            const T& val) noexcept
    {
        storage_.val_ = val;
        storage_.engaged_ = true;
    }

    optional(
            T&& val) noexcept
    {
        storage_.val_ = std::move(val);
        storage_.engaged_ = true;
    }

    optional(
            const optional<T>& val) noexcept
    {
        storage_.val_ = val.storage_.val;
        storage_.engaged_ = val.storage_.engaged_;
    }

    optional(
            optional<T>&& val) noexcept
    {
        storage_.val_ = std::move(val.storage_.val);
        storage_.engaged_ = val.storage_.engaged_;
    }

    ~optional() = default;

    template<class ... Args> void emplace(
            Args&&... _args)
    {
        reset();
        storage_.val_.T(std::forward<Args>(_args)...);
        storage_.engaged_ = true;
    }

    void reset(
            bool initial_engaged = false)
    {
        if ( storage_.engaged_)
        {
            storage_.val_.~T();
        }
        storage_.engaged_ = initial_engaged;
    }

    T& value()&
    {
        return storage_.engaged_ ? storage_.val_ : throw exception::BadParamException("value not set");
    }

    const T& value() const&
    {
        return storage_.engaged_ ? storage_.val_ : throw exception::BadParamException("value not set");
    }

    T&& value() &&
    {
        return storage_.engaged_ ? std::move(storage_.val_) : throw exception::BadParamException("value not set");
    }

    const T&& value() const&&
    {
        return storage_.engaged_ ? std::move(storage_.val_) : throw exception::BadParamException("value not set");
    }

    bool has_value() const
    {
        return storage_.engaged_;
    }

    optional& operator =(
            const optional& opt)
    {
        storage_ = opt.storage_;
    }

    optional& operator =(
            optional&& opt)
    {
        storage_ = std::move(opt.storage_);
    }

    optional& operator =(
            const T& val)
    {
        reset();
        storage_.val_ = val;
        storage_.engaged_ = true;
        return *this;
    }

    optional& operator =(
            T&& val)
    {
        reset();
        storage_.val_ = std::move(val);
        storage_.engaged_ = true;
        return *this;
    }

    optional& operator = (
            nullopt_t) noexcept
    {
        reset();
        return *this;
    }

    optional& operator ==(
            const optional& opt_val) const
    {
        return opt_val.storage_.engaged_ == storage_.engaged_ &&
               (storage_.engaged_ ? opt_val.storage_.val_ == storage_.val_ : true);
    }

    T& operator *() & noexcept
    {
        return storage_.val_;
    }

    const T& operator *() const& noexcept
    {
        return storage_.val_;
    }

    T&& operator *() && noexcept
    {
        return std::move(storage_.val_);
    }

    const T&& operator *() const&& noexcept
    {
        return std::move(storage_.val_);
    }

    T* operator ->() noexcept
    {
        return std::addressof(storage_.val_);
    }

    const T* operator ->() const noexcept
    {
        return std::addressof(storage_.val_);
    }

    explicit operator bool() const noexcept
    {
        return storage_.engaged_;
    }

private:

    detail::optional_storage<T> storage_;
};// namespace fastcdr

} // namespace fastcdr
} // namespace eprosima

#endif //_FASTCDR_XCDR_OPTIONAL_HPP_
