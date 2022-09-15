/*
 * Copyright (c) 2022, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Tuple.h>
#include <LibWeb/HTML/Scripting/ModuleScript.h>

namespace Web::HTML {

class ModuleLocationTuple {
public:
    ModuleLocationTuple(AK::URL url, String type)
        : m_url(move(url))
        , m_type(move(type))
    {
    }

    AK::URL const& url() const { return m_url; };
    String const& type() const { return m_type; }

    bool operator==(ModuleLocationTuple const& other) const
    {
        return other.url() == m_url && other.type() == m_type;
    };

private:
    AK::URL m_url;
    String m_type;
};

// https://html.spec.whatwg.org/multipage/webappapis.html#module-map
class ModuleMap {
public:
    ModuleMap() = default;
    ~ModuleMap() = default;

    enum class EntryType {
        Fetching,
        Failed,
        Module
    };

    struct Entry {
        EntryType type;
        ModuleScript* module_script;
    };

    bool is_fetching(AK::URL const& url, String const& type) const;
    bool is_failed(AK::URL const& url, String const& type) const;

    bool is(AK::URL const& url, String const& type, EntryType) const;

private:
    HashMap<ModuleLocationTuple, Entry> m_values;
};

}

namespace AK {

template<>
struct Traits<Web::HTML::ModuleLocationTuple> : public GenericTraits<Web::HTML::ModuleLocationTuple> {
    static unsigned hash(Web::HTML::ModuleLocationTuple const& tuple)
    {
        return pair_int_hash(tuple.url().to_string().hash(), tuple.type().hash());
    }
};

}
