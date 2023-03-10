/*
 * Copyright (c) 2021, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2023, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/HTML/Scripting/Environments.h>
#include <LibWeb/HTML/Scripting/Script.h>

namespace Web::HTML {

Script::Script(AK::URL base_url, DeprecatedString filename, EnvironmentSettingsObject& environment_settings_object, ScriptFetchOptions fetch_options)
    : m_base_url(move(base_url))
    , m_filename(move(filename))
    , m_settings_object(environment_settings_object)
    , m_fetch_options(move(fetch_options))
{
}

Script::~Script() = default;

void Script::visit_host_defined_self(JS::Cell::Visitor& visitor)
{
    visitor.visit(this);
}

// https://html.spec.whatwg.org/multipage/webappapis.html#active-script
JS::GCPtr<Script> Script::active_script(JS::VM& vm)
{
    // 1. Let record be GetActiveScriptOrModule().
    auto record = vm.get_active_script_or_module();

    // 2. If record is null, return null.
    if (record.has<Empty>())
        return nullptr;

    // 3. Return record.[[HostDefined]].
    return verify_cast<Script>(record.has<JS::NonnullGCPtr<JS::Script>>() ? record.get<JS::NonnullGCPtr<JS::Script>>()->host_defined() : record.get<JS::NonnullGCPtr<JS::Module>>()->host_defined());
}

}
