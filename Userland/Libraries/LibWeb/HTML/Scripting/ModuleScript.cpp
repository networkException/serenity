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

    auto& realm = settings_object.realm();

    // 2. Let script be a new module script that this algorithm will subsequently initialize.
    auto* script = realm.heap().allocate<JavaScriptModuleScript>(realm, move(base_url), filename, settings_object);

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
        dbgln("JavaScriptModuleScript: Failed to parse: {}", parse_error.to_string());

        // FIXME: 1. Set script's parse error to result[0].

        // 2. Return script.
        return script;
    }

    dbgln("did parse {}", filename);

    // 10. For each ModuleRequest record requested of result.[[RequestedModules]]:
    for (auto const& requested : result.value()->requested_modules()) {
        dbgln("got requested module: {}", requested.module_specifier);
        // 9. Assert: requested.[[Assertions]] does not contain any Record entry such that entry.[[Key]] is not "type",
        //            because we only asked for "type" assertions in HostGetSupportedImportAssertions.
        for (auto const& assertion : requested.assertions) {
            dbgln("A");
            VERIFY(assertion.key == "type"sv);
        }

        // 1. Let url be the result of resolving a module specifier given script's base URL and requested.[[Specifier]].
        auto url = requested.resolve_specifier(script->base_url());

        // 2. Let moduleType be the result of running the module type from module request steps given requested.
        auto module_type = requested.module_type();

        // 3. If url is failure, or if the result of running the module type allowed steps given moduleType and settings is false, then:
        if (!url.is_valid() || !settings_object.module_type_allowed(module_type)) {
            // FIXME: 1. Let error be a new TypeError exception.

            // FIXME: 2. Set script's parse error to error.

            dbgln("oops url not valid or module type not allowed! {} [{}], {}", url, url.is_valid(), module_type);
            // 3. Return script.
            return script;
        }
    }

    // 11. Set script's record to result.
    script->m_record = result.value();

    dbgln("{} should be fine!", filename);
    // 12. Return script.
    return script;
}

// https://html.spec.whatwg.org/multipage/webappapis.html#run-a-module-script
JS::Promise* JavaScriptModuleScript::run(PreventErrorReporting)
{
    // 1. Let settings be the settings object of script.
    auto& settings = settings_object();

    // 2. Check if we can run script with settings. If this returns "do not run", then return a promise resolved with undefined.
    if (settings.can_run_script() == RunScriptDecision::DoNotRun) {
        return JS::Promise::create_fulfilled(settings.realm(), JS::js_undefined());
    }

    // 3. Prepare to run script given settings.
    settings.prepare_to_run_script();

    // 4. Let evaluationPromise be null.
    JS::Promise* evaluation_promise = nullptr;

    // FIXME: 5. If script's error to rethrow is not null, then set evaluationPromise to a promise rejected with script's error to rethrow.

    // 6. Otherwise:
    if (m_record) {
        // 1. Let record be script's record.
        auto record = m_record;

        // 2. Set evaluationPromise to record.Evaluate().
        auto elevation_promise_or_error = record->evaluate(vm());

        dbgln("evaluated module! {}", elevation_promise_or_error.is_error());
        if (elevation_promise_or_error.is_error()) {
            dbgln("error: {}", elevation_promise_or_error.release_error().value().value());
        }

        // NOTE: This step will recursively evaluate all of the module's dependencies.
        // If Evaluate fails to complete as a result of the user agent aborting the running script,
        // then set evaluationPromise to a promise rejected with a new "QuotaExceededError" DOMException.
        if (elevation_promise_or_error.is_error()) {
            auto* promise = JS::Promise::create(settings_object().realm());
            promise->reject(DOM::QuotaExceededError::create(current_global_object(), "Failed to evaluate module script").ptr());

            evaluation_promise = promise;
        } else {
            evaluation_promise = elevation_promise_or_error.value();
        }
    } else {
        dbgln("parsing failed!???");
    }

    // FIXME: 7. If preventErrorReporting is false, then upon rejection of evaluationPromise with reason, report the exception given by reason for script.

    // 8. Clean up after running script with settings.
    settings.clean_up_after_running_script();

    // 9. Return evaluationPromise.
    return evaluation_promise;
}

void JavaScriptModuleScript::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_record);
}

}
