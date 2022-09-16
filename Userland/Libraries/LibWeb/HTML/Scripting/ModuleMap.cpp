/*
 * Copyright (c) 2022, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/HTML/Scripting/ModuleMap.h>

namespace Web::HTML {

bool ModuleMap::is_fetching(const AK::URL& url, const AK::String& type) const
{
    return is(url, type, EntryType::Fetching);
}

bool ModuleMap::is_failed(const AK::URL& url, const AK::String& type) const
{
    return is(url, type, EntryType::Failed);
}

bool ModuleMap::is(AK::URL const& url, AK::String const& type, EntryType entry_type) const
{
    auto value = m_values.get({ url, type });
    if (!value.has_value())
        return false;

    return value->type == entry_type;
}

}
