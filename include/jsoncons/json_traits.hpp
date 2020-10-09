// Copyright 2020 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_JSON_TRAITS_HPP
#define JSONCONS_JSON_TRAITS_HPP

#include <jsoncons/json_type_traits.hpp>

namespace jsoncons {

    template <class T, class Enable=void>
    struct json_traits
    {
        template <class Json>
        static constexpr bool is_compatible()
        {
            return !jsoncons::detail::legacy_is_json_type_traits_unspecialized<Json,T>::value;
        }

        template <class Json>
        static constexpr bool is(const Json& j) noexcept
        {
            return json_type_traits<Json,T>::is(j);
        }

        template <class Json>
        static T as(const Json& j)
        {
            return json_type_traits<Json,T>::as(j);
        }

        template <class Json>
        static Json to_json(const T& val)
        {
            return json_type_traits<Json,T>::to_json(val);
        }

        template <class Json, class Allocator=typename Json::allocator_type>
        static Json to_json(const T& val, const Allocator& alloc)
        {
            return json_type_traits<Json,T>::to_json(val, alloc);
        }
    };
}

#endif


