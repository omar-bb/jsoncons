// Copyright 2019
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_CDDL_CDDL_VALIDATOR_HPP
#define JSONCONS_CDDL_CDDL_VALIDATOR_HPP

#include <jsoncons/json.hpp>
#include <jsoncons/json_visitor.hpp>
#include <jsoncons/staj_cursor.hpp>
#include <jsoncons_ext/cddl/cddl_error.hpp>
#include <jsoncons_ext/cddl/acceptor.hpp>
#include <jsoncons_ext/cddl/cddl_specification.hpp>
#include <memory>

namespace jsoncons { namespace cddl {

    class cddl_validator : public json_visitor
    {
    public:
        typedef std::string string_type;
    private:
        cddl_specification spec_;
        std::vector<acceptor> acceptor_stack_;

        string_type key_;
    public:
        cddl_validator(cddl_specification&& spec)
            : spec_(std::move(spec))
        {
            acceptor_stack_.emplace_back(spec.root());
        }
        cddl_validator(const cddl_validator&) = delete;
        cddl_validator(cddl_validator&&) = default;
        cddl_validator& operator=(const cddl_validator&) = delete;
        cddl_validator& operator=(cddl_validator&&) = default;
    private:
        void visit_flush() override
        {
        }

        bool visit_begin_object(semantic_tag, const ser_context&, std::error_code&) override
        {
            return true;
        }

        bool visit_end_object(const ser_context&, std::error_code&) override
        {
            return true;
        }

        bool visit_begin_array(semantic_tag tag, const ser_context&, std::error_code&) override
        {
            std::cout << "visit_begin_array\n";
            if (!acceptor_stack_.back().accept_event(key_, staj_event(staj_event_type::begin_array,tag)))
            {
                throw std::runtime_error("Not an array");
            }
            key_.clear();
            return true;
        }

        bool visit_end_array(const ser_context&, std::error_code&) override
        {
            return true;
        }

        bool visit_key(const string_view_type&, const ser_context&, std::error_code&) override
        {
            return true;
        }

        bool visit_null(semantic_tag, const ser_context&, std::error_code&) override
        {
            return true;
        }

        bool visit_string(const string_view_type&, semantic_tag, const ser_context&, std::error_code&) override
        {
            //Acceptor next = acceptor_stack_.back().createChildAcceptor(key_, staj_event(s,tag));
            return true;
        }

        bool visit_byte_string(const byte_string_view&,
                                  semantic_tag, 
                                  const ser_context&, std::error_code&) override
        {
            return true;
        }

        bool visit_int64(int64_t, 
                            semantic_tag, 
                            const ser_context&, std::error_code&) override
        {
            return true;
        }

        bool visit_uint64(uint64_t, 
                             semantic_tag, 
                             const ser_context&, std::error_code&) override
        {
            return true;
        }

        bool visit_double(double, 
                             semantic_tag,
                             const ser_context&, std::error_code&) override
        {
            return true;
        }

        bool visit_bool(bool, semantic_tag, const ser_context&, std::error_code&) override
        {
            return true;
        }
    };

} // namespace cddl
} // namespace jsoncons

#endif
