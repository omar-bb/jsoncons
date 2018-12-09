// Copyright 2017 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_UBJSON_UBJSON_READER_HPP
#define JSONCONS_UBJSON_UBJSON_READER_HPP

#include <string>
#include <sstream>
#include <vector>
#include <istream>
#include <cstdlib>
#include <memory>
#include <limits>
#include <cassert>
#include <iterator>
#include <jsoncons/json.hpp>
#include <jsoncons/detail/source.hpp>
#include <jsoncons/json_content_handler.hpp>
#include <jsoncons/config/binary_detail.hpp>
#include <jsoncons_ext/ubjson/ubjson_detail.hpp>
#include <jsoncons_ext/ubjson/ubjson_error.hpp>

namespace jsoncons { namespace ubjson {

template <class Source>
class basic_ubjson_reader : public serializing_context
{
    Source source_;
    json_content_handler& handler_;
    size_t column_;
    size_t nesting_depth_;
    std::string buffer_;
public:
    basic_ubjson_reader(Source&& source, json_content_handler& handler)
       : source_(std::move(source)),
         handler_(handler), 
         column_(1),
         nesting_depth_(0)
    {
    }

    void read(std::error_code& ec)
    {
        //const uint8_t* pos = input_ptr_++;

        uint8_t type;
        if (source_.get(type) == 0)
        {
            ec = ubjson_errc::unexpected_eof;
            return;
        }
        read_value(type, ec);
    }

    void read_value(uint8_t type, std::error_code& ec)
    {
        switch (type)
        {
            case ubjson_format::null_type: 
            {
                handler_.null_value(semantic_tag_type::none, *this);
                break;
            }
            case ubjson_format::no_op_type: 
            {
                break;
            }
            case ubjson_format::true_type:
            {
                handler_.bool_value(true, semantic_tag_type::none, *this);
                break;
            }
            case ubjson_format::false_type:
            {
                handler_.bool_value(false, semantic_tag_type::none, *this);
                break;
            }
            case ubjson_format::int8_type: 
            {
                uint8_t buf[sizeof(int8_t)];
                source_.read(sizeof(int8_t), buf);
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return;
                }
                const uint8_t* endp;
                int8_t val = jsoncons::detail::from_big_endian<int8_t>(buf,buf+sizeof(buf),&endp);
                handler_.int64_value(val, semantic_tag_type::none, *this);
                break;
            }
            case ubjson_format::uint8_type: 
            {
                uint8_t val;
                if (source_.get(val) == 0)
                {
                    ec = ubjson_errc::unexpected_eof;
                    return;
                }
                handler_.uint64_value(val, semantic_tag_type::none, *this);
                break;
            }
            case ubjson_format::int16_type: 
            {
                uint8_t buf[sizeof(int16_t)];
                source_.read(sizeof(int16_t), buf);
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return;
                }
                const uint8_t* endp;
                int16_t val = jsoncons::detail::from_big_endian<int16_t>(buf,buf+sizeof(buf),&endp);
                handler_.int64_value(val, semantic_tag_type::none, *this);
                break;
            }
            case ubjson_format::int32_type: 
            {
                uint8_t buf[sizeof(int32_t)];
                source_.read(sizeof(int32_t), buf);
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return;
                }
                const uint8_t* endp;
                int32_t val = jsoncons::detail::from_big_endian<int32_t>(buf,buf+sizeof(buf),&endp);
                handler_.int64_value(val, semantic_tag_type::none, *this);
                break;
            }
            case ubjson_format::int64_type: 
            {
                uint8_t buf[sizeof(int64_t)];
                source_.read(sizeof(int64_t), buf);
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return;
                }
                const uint8_t* endp;
                int64_t val = jsoncons::detail::from_big_endian<int64_t>(buf,buf+sizeof(buf),&endp);
                handler_.int64_value(val, semantic_tag_type::none, *this);
                break;
            }
            case ubjson_format::float32_type: 
            {
                uint8_t buf[sizeof(float)];
                source_.read(sizeof(float), buf);
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return;
                }
                const uint8_t* endp;
                float val = jsoncons::detail::from_big_endian<float>(buf,buf+sizeof(buf),&endp);
                handler_.double_value(val, semantic_tag_type::none, *this);
                break;
            }
            case ubjson_format::float64_type: 
            {
                uint8_t buf[sizeof(double)];
                source_.read(sizeof(double), buf);
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return;
                }
                const uint8_t* endp;
                double val = jsoncons::detail::from_big_endian<double>(buf,buf+sizeof(buf),&endp);
                handler_.double_value(val, semantic_tag_type::none, *this);
                break;
            }
            case ubjson_format::char_type: 
            {
                uint8_t buf[sizeof(char)];
                source_.read(sizeof(char), buf);
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return;
                }
                const uint8_t* endp;
                char c = jsoncons::detail::from_big_endian<char>(buf,buf+sizeof(buf),&endp);
                handler_.string_value(basic_string_view<char>(&c,1), semantic_tag_type::none, *this);
                break;
            }
            case ubjson_format::string_type: 
            {
                size_t length = get_length(ec);
                if (ec)
                {
                    return;
                }
                std::string s;
                source_.read(length,std::back_inserter(s));
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return;
                }
                handler_.string_value(basic_string_view<char>(s.data(),s.length()), semantic_tag_type::none, *this);
                break;
            }
            case ubjson_format::high_precision_number_type: 
            {
                size_t length = get_length(ec);
                if (ec)
                {
                    return;
                }
                std::string s;
                source_.read(length,std::back_inserter(s));
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return;
                }
                if (jsoncons::detail::is_integer(s.data(),s.length()))
                {
                    handler_.string_value(basic_string_view<char>(s.data(),s.length()), semantic_tag_type::bignum, *this);
                }
                else
                {
                    handler_.string_value(basic_string_view<char>(s.data(),s.length()), semantic_tag_type::decimal_fraction, *this);
                }
                break;
            }
            case ubjson_format::start_array_marker: 
            {
                if (source_.peek() == ubjson_format::type_marker)
                {
                    source_.ignore(1);
                    uint8_t type;
                    if (source_.get(type) == 0)
                    {
                        ec = ubjson_errc::unexpected_eof;
                        return;
                    }
                    if (source_.peek() == ubjson_format::count_marker)
                    {
                        source_.ignore(1);
                        size_t length = get_length(ec);
                        handler_.begin_array(length, semantic_tag_type::none, *this);
                        for (size_t i = 0; i < length; ++i)
                        {
                            read_value(type, ec);
                            if (ec)
                            {
                                return;
                            }
                        }
                        handler_.end_array(*this);
                    }
                    else
                    {
                        ec = ubjson_errc::count_required_after_type;
                        return;
                    }
                }
                else if (source_.peek() == ubjson_format::count_marker)
                {
                    source_.ignore(1);
                    size_t length = get_length(ec);
                    handler_.begin_array(length, semantic_tag_type::none, *this);
                    for (size_t i = 0; i < length; ++i)
                    {
                        read(ec);
                        if (ec)
                        {
                            return;
                        }
                    }
                    handler_.end_array(*this);
                }
                else
                {
                    handler_.begin_array(semantic_tag_type::none, *this);
                    while (source_.peek() != ubjson_format::end_array_marker)
                    {
                        read(ec);
                        if (ec)
                        {
                            return;
                        }
                    }
                    handler_.end_array(*this);
                    source_.ignore(1);
                }
                break;
            }
            case ubjson_format::start_object_marker: 
            {
                if (source_.peek() == ubjson_format::type_marker)
                {
                    source_.ignore(1);
                    uint8_t type;
                    if (source_.get(type) == 0)
                    {
                        ec = ubjson_errc::unexpected_eof;
                        return;
                    }
                    if (source_.peek() == ubjson_format::count_marker)
                    {
                        source_.ignore(1);
                        size_t length = get_length(ec);
                        handler_.begin_array(length, semantic_tag_type::none, *this);
                        for (size_t i = 0; i < length; ++i)
                        {
                            read_name(ec);
                            if (ec)
                            {
                                return;
                            }
                            read_value(type, ec);
                            if (ec)
                            {
                                return;
                            }
                        }
                        handler_.end_array(*this);
                    }
                    else
                    {
                        ec = ubjson_errc::count_required_after_type;
                        return;
                    }
                }
                else
                {
                    if (source_.peek() == ubjson_format::count_marker)
                    {
                        size_t length = get_length(ec);
                        handler_.begin_object(length, semantic_tag_type::none, *this);
                        for (size_t i = 0; i < length; ++i)
                        {
                            read_name(ec);
                            if (ec)
                            {
                                return;
                            }
                            read(ec);
                            if (ec)
                            {
                                return;
                            }
                        }
                        handler_.end_object(*this);
                    }
                    else
                    {
                        handler_.begin_object(semantic_tag_type::none, *this);
                        while (source_.peek() != ubjson_format::end_array_marker)
                        {
                            read_name(ec);
                            if (ec)
                            {
                                return;
                            }
                            read(ec);
                            if (ec)
                            {
                                return;
                            }
                        }
                        handler_.end_object(*this);
                        source_.ignore(1);
                    }
                }
                break;
            }
            default:
            {
                ec = ubjson_errc::unknown_type;
                return;
            }
        }
    }

    size_t line_number() const override
    {
        return 1;
    }

    size_t column_number() const override
    {
        return column_;
    }
private:

    size_t get_length(std::error_code& ec)
    {
        size_t length = 0;
        if (JSONCONS_UNLIKELY(source_.eof()))
        {
            ec = ubjson_errc::unexpected_eof;
            return length;
        }
        uint8_t type;
        if (source_.get(type) == 0)
        {
            ec = ubjson_errc::unexpected_eof;
            return length;
        }
        switch (type)
        {
            case ubjson_format::int8_type: 
            {
                uint8_t buf[sizeof(int8_t)];
                source_.read(sizeof(int8_t), buf);
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return length;
                }
                const uint8_t* endp;
                int8_t val = jsoncons::detail::from_big_endian<int8_t>(buf,buf+sizeof(buf),&endp);
                if (val >= 0)
                {
                    length = val;
                }
                else
                {
                    ec = ubjson_errc::length_cannot_be_negative;
                    return length;
                }
                break;
            }
            case ubjson_format::uint8_type: 
            {
                uint8_t val;
                if (source_.get(val) == 0)
                {
                    ec = ubjson_errc::unexpected_eof;
                    return length;
                }
                length = val;
                break;
            }
            case ubjson_format::int16_type: 
            {
                uint8_t buf[sizeof(int16_t)];
                source_.read(sizeof(int16_t), buf);
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return length;
                }
                const uint8_t* endp;
                int16_t val = jsoncons::detail::from_big_endian<int16_t>(buf,buf+sizeof(buf),&endp);
                if (val >= 0)
                {
                    length = val;
                }
                else
                {
                    ec = ubjson_errc::length_cannot_be_negative;
                    return length;
                }
                break;
            }
            case ubjson_format::int32_type: 
            {
                uint8_t buf[sizeof(int32_t)];
                source_.read(sizeof(int32_t), buf);
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return length;
                }
                const uint8_t* endp;
                int32_t val = jsoncons::detail::from_big_endian<int32_t>(buf,buf+sizeof(buf),&endp);
                if (val >= 0)
                {
                    length = val;
                }
                else
                {
                    ec = ubjson_errc::length_cannot_be_negative;
                    return length;
                }
                break;
            }
            case ubjson_format::int64_type: 
            {
                uint8_t buf[sizeof(int64_t)];
                source_.read(sizeof(int64_t), buf);
                if (source_.eof())
                {
                    ec = ubjson_errc::unexpected_eof;
                    return length;
                }
                const uint8_t* endp;
                int64_t val = jsoncons::detail::from_big_endian<int64_t>(buf,buf+sizeof(buf),&endp);
                if (val >= 0)
                {
                    length = val;
                }
                else
                {
                    ec = ubjson_errc::length_cannot_be_negative;
                    return length;
                }
                break;
            }
            default:
            {
                ec = ubjson_errc::length_must_be_integer;
                return length;
            }
        }
        return length;
    }

    void read_name(std::error_code& ec)
    {
        size_t length = get_length(ec);
        if (ec)
        {
            return;
        }
        std::string s;
        source_.read(length,std::back_inserter(s));
        if (source_.eof())
        {
            ec = ubjson_errc::unexpected_eof;
            return;
        }
        handler_.name(basic_string_view<char>(s.data(),s.length()), *this);
    }
};

typedef basic_ubjson_reader<jsoncons::detail::buffer_source> ubjson_reader;

}}

#endif