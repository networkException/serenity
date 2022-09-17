/*
 * Copyright (c) 2022, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/HTML/Scripting/ModuleMap.h>

namespace Web::HTML {

static StringView type_to_string(ModuleMap::EntryType type)
{
    switch (type) {
    case ModuleMap::EntryType::ModuleScript:
        return "ModuleScript"sv;
    case ModuleMap::EntryType::Fetching:
        return "Fetching"sv;
    case ModuleMap::EntryType::Failed:
        return "Failed"sv;
    default:
        VERIFY_NOT_REACHED();
    }
}

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
    dbgln("ModuleMap::is(url: {}, type: {}, entry_type: {})", url, type, type_to_string(entry_type));

    print_debug();

    auto value = m_values.get({ url, type });
    if (!value.has_value()) {
        dbgln("ModuleMap::is not in m_values, returning false");

        return false;
    }

    dbgln("ModuleMap::is found value of type {}, returning {}", type_to_string(entry_type), value->type == entry_type);

    return value->type == entry_type;
}

Optional<ModuleMap::Entry> ModuleMap::get(AK::URL const& url, AK::String const& type) const
{
    return m_values.get({ url, type });
}

AK::HashSetResult ModuleMap::set(AK::URL const& url, AK::String const& type, Entry entry)
{
    auto callbacks = m_callbacks.get({ url, type });
    if (callbacks.has_value())
        for (auto const& callback : *callbacks)
            callback(entry);

    return m_values.set({ url, type }, entry);
}

void ModuleMap::wait_for_change(AK::URL const& url, AK::String const& type, Function<void(Entry)> callback)
{
    auto callbacks = m_callbacks.get({ url, type });
    if (callbacks.has_value()) {
        callbacks.value().append(move(callback));
        return;
    }

    Vector<Function<void(Entry)>> new_callbacks;
    new_callbacks.append(move(callback));

    m_callbacks.set({ url, type }, move(new_callbacks));
}

void ModuleMap::print_debug() const
{
    dbgln("<ModuleMap::print_debug {} --------------->", (void*) this);

    for (auto const& entry : m_values) {
        dbgln("{} {}: {}", entry.key.url(), entry.key.type(), type_to_string(entry.value.type));
    }

    dbgln("</ModuleMap::print_debug {} --------------->", (void*) this);
}

}
