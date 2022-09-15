/*
 * Copyright (c) 2022, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/HTML/Scripting/Environments.h>
#include <LibWeb/HTML/Scripting/ModuleScript.h>

namespace Web::HTML {

ModuleScript::~ModuleScript() = default;

ModuleScript::ModuleScript(AK::URL base_url, String filename, EnvironmentSettingsObject& environment_settings_object)
    : Script(move(base_url), move(filename), environment_settings_object)
{
}

JavaScriptModuleScript::~JavaScriptModuleScript() = default;

JavaScriptModuleScript::JavaScriptModuleScript(AK::URL base_url, String filename, EnvironmentSettingsObject& environment_settings_object)
    : ModuleScript(move(base_url), move(filename), environment_settings_object)
{
}

// https://html.spec.whatwg.org/multipage/webappapis.html#creating-a-javascript-module-script
JS::GCPtr<JavaScriptModuleScript> JavaScriptModuleScript::create(String const& filename, StringView source, EnvironmentSettingsObject& settings_object, AK::URL base_url)
{
    // 1. If scripting is disabled for settings, then set source to the empty string.
    if (settings_object.is_scripting_disabled())
        source = ""sv;

    // 2. Let script be a new module script that this algorithm will subsequently initialize.
    auto script = JavaScriptModuleScript(move(base_url), filename, settings_object);

    // 3. Set script's settings object to settings. (NOTE: This was already done when constructing.)

    // 4. Set script's base URL to baseURL. (NOTE: This was already done when constructing.)

    // FIXME: 5. Set script's fetch options to options.

    // 6. Set script's parse error and error to rethrow to null.
    // NOTE: Parse error and error to rethrow were set to null in the construction of Script.

    // 7. Let result be ParseModule(source, settings's Realm, script).
    // FIXME: Pass script instead of filename
    auto result = JS::SourceTextModule::parse(source, settings_object.realm(), filename.view());

    // 8. If result is a list of errors, then:
    if (result.is_error()) {
        auto& parse_error = result.error().first();
        dbgln_if(HTML_SCRIPT_DEBUG, "JavaScriptModuleScript: Failed to parse: {}", parse_error.to_string());

        // FIXME: 1. Set script's parse error to result[0].

        // 2. Return script.
        return script;
    }

    // 10. For each ModuleRequest record requested of result.[[RequestedModules]]:
    for (auto const& requested : result.value()->requested_modules()) {
        // 9. Assert: requested.[[Assertions]] does not contain any Record entry such that entry.[[Key]] is not "type",
        //            because we only asked for "type" assertions in HostGetSupportedImportAssertions.
        for (auto const& assertion : requested.assertions)
            VERIFY(assertion.key == "type"sv);

        // 1. Let url be the result of resolving a module specifier given script's base URL and requested.[[Specifier]].
        auto url = requested.resolve_specifier(script.base_url());

        // 2. Let moduleType be the result of running the module type from module request steps given requested.
        auto module_type = requested.module_type();

        // 3. If url is failure, or if the result of running the module type allowed steps given moduleType and settings is false, then:
        if (!url.is_valid() || !settings_object.module_type_allowed(module_type)) {
            // FIXME: 1. Let error be a new TypeError exception.

            // FIXME: 2. Set script's parse error to error.

            // 3. Return script.
            return script;
        }
    }

    // 11. Set script's record to result.
    script.m_record = result.value();

    // 12. Return script.
    return script;
}

void JavaScriptModuleScript::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(*m_record);
}

}
