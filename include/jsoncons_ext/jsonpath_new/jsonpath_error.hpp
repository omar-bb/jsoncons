/// Copyright 2020 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_JSONPATH_JSONPATH_ERROR_HPP
#define JSONCONS_JSONPATH_JSONPATH_ERROR_HPP

#include <jsoncons/json_exception.hpp>
#include <system_error>

namespace jsoncons { namespace jsonpath {

class jsonpath_error : public std::system_error
{
    std::string buffer_;
    std::size_t line_number_;
    std::size_t column_number_;
public: 
    jsonpath_error(std::error_code ec)
        : std::system_error(ec), line_number_(0), column_number_(0)
    {
    }
    jsonpath_error(std::error_code ec, std::size_t position)
        : std::system_error(ec), line_number_(0), column_number_(position)
    {
    }
    jsonpath_error(std::error_code ec, std::size_t line, std::size_t column)
        : std::system_error(ec), line_number_(line), column_number_(column)
    {
    }
    jsonpath_error(const jsonpath_error& other) = default;

    jsonpath_error(jsonpath_error&& other) = default;

    const char* what() const noexcept override
    {
        JSONCONS_TRY
        {
            std::ostringstream os;
            os << std::system_error::what();
            if (line_number_ != 0 && column_number_ != 0)
            {
                os << " at line " << line_number_ << " and column " << column_number_;
            }
            else if (column_number_ != 0)
            {
                os << " at position " << column_number_;
            }
            const_cast<std::string&>(buffer_) = os.str();
            return buffer_.c_str();
        }
        JSONCONS_CATCH(...)
        {
            return std::system_error::what();
        }
    }

    std::size_t line() const noexcept
    {
        return line_number_;
    }

    std::size_t column() const noexcept
    {
        return column_number_;
    }
#if !defined(JSONCONS_NO_DEPRECATED)
    JSONCONS_DEPRECATED_MSG("Instead, use line()")
    std::size_t line_number() const noexcept
    {
        return line();
    }

    JSONCONS_DEPRECATED_MSG("Instead, use column()")
    std::size_t column_number() const noexcept
    {
        return column();
    }
#endif
};

enum class jsonpath_errc 
{
    success = 0,
    expected_identifier,
    expected_index,
    expected_A_Za_Z_,
    expected_right_bracket,
    expected_right_brace,
    expected_colon,
    expected_dot,
    expected_or,
    expected_and,
    expected_multi_select_list,
    invalid_number,
    invalid_literal,
    expected_comparator,
    expected_key,
    invalid_argument,
    unknown_function,
    invalid_type,
    unexpected_end_of_input,
    step_cannot_be_zero,
    syntax_error,
    invalid_codepoint,
    illegal_escaped_character,
    unbalanced_parentheses,
    invalid_arity,
    identifier_not_found,
    expected_index_expression,
    expected_pipe,
    unknown_error 
};

class jsonpath_error_category_impl
   : public std::error_category
{
public:
    const char* name() const noexcept override
    {
        return "jsoncons/jsonpath";
    }
    std::string message(int ev) const override
    {
        switch (static_cast<jsonpath_errc>(ev))
        {
            case jsonpath_errc::expected_identifier:
                return "Expected identifier";
            case jsonpath_errc::expected_index:
                return "Expected index";
            case jsonpath_errc::expected_A_Za_Z_:
                return "Expected A-Z, a-z, or _";
            case jsonpath_errc::expected_right_bracket:
                return "Expected ]";
            case jsonpath_errc::expected_right_brace:
                return "Expected }";
            case jsonpath_errc::expected_colon:
                return "Expected :";
            case jsonpath_errc::expected_dot:
                return "Expected \".\"";
            case jsonpath_errc::expected_or:
                return "Expected \"||\"";
            case jsonpath_errc::expected_and:
                return "Expected \"&&\"";
            case jsonpath_errc::expected_multi_select_list:
                return "Expected multi-select-list";
            case jsonpath_errc::invalid_number:
                return "Invalid number";
            case jsonpath_errc::invalid_literal:
                return "Invalid literal";
            case jsonpath_errc::expected_comparator:
                return "Expected <, <=, ==, >=, > or !=";
            case jsonpath_errc::expected_key:
                return "Expected key";
            case jsonpath_errc::invalid_argument:
                return "Invalid argument type";
            case jsonpath_errc::unknown_function:
                return "Unknown function";
            case jsonpath_errc::invalid_type:
                return "Invalid type";
            case jsonpath_errc::unexpected_end_of_input:
                return "Unexpected end of jsonpath input";
            case jsonpath_errc::step_cannot_be_zero:
                return "Slice step cannot be zero";
            case jsonpath_errc::syntax_error:
                return "Syntax error";
            case jsonpath_errc::invalid_codepoint:
                return "Invalid codepoint";
            case jsonpath_errc::illegal_escaped_character:
                return "Illegal escaped character";
            case jsonpath_errc::unbalanced_parentheses:
                return "Unbalanced parentheses";
            case jsonpath_errc::invalid_arity:
                return "Function called with wrong number of arguments";
            case jsonpath_errc::identifier_not_found:
                return "Identifier not found";
            case jsonpath_errc::expected_index_expression:
                return "Expected index expression";
            case jsonpath_errc::expected_pipe:
                return "Expected '|'";
            case jsonpath_errc::unknown_error:
            default:
                return "Unknown jsonpath parser error";
        }
    }
};

inline
const std::error_category& jsonpath_error_category()
{
  static jsonpath_error_category_impl instance;
  return instance;
}

inline 
std::error_code make_error_code(jsonpath_errc result)
{
    return std::error_code(static_cast<int>(result),jsonpath_error_category());
}

}}

namespace std {
    template<>
    struct is_error_code_enum<jsoncons::jsonpath::jsonpath_errc> : public true_type
    {
    };
}

#endif
