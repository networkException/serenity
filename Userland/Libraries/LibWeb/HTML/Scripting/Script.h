/*
 * Copyright (c) 2021, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2023, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/URL.h>
#include <LibJS/Heap/Cell.h>
#include <LibJS/Script.h>
#include <LibWeb/Forward.h>
#include <LibWeb/HTML/Scripting/FetchOptions.h>

namespace Web::HTML {

// https://html.spec.whatwg.org/multipage/webappapis.html#concept-script
class Script
    : public JS::Cell
    , public JS::Script::HostDefined {
    JS_CELL(Script, JS::Cell);

public:
    virtual ~Script() override;

    ScriptFetchOptions const& fetch_options() { return m_fetch_options; }
    void set_fetch_options(ScriptFetchOptions fetch_options) { m_fetch_options = move(fetch_options); }

    AK::URL const& base_url() const { return m_base_url; }
    DeprecatedString const& filename() const { return m_filename; }

    EnvironmentSettingsObject& settings_object() { return m_settings_object; }

    // https://html.spec.whatwg.org/multipage/webappapis.html#active-script
    static JS::GCPtr<Script> active_script(JS::VM&);

protected:
    Script(AK::URL base_url, DeprecatedString filename, EnvironmentSettingsObject& environment_settings_object, ScriptFetchOptions);

private:
    virtual void visit_host_defined_self(JS::Cell::Visitor&) override;

    AK::URL m_base_url;
    DeprecatedString m_filename;
    EnvironmentSettingsObject& m_settings_object;
    ScriptFetchOptions m_fetch_options;
};

}
