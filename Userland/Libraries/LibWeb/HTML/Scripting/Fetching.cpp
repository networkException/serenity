/*
 * Copyright (c) 2022-2023, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/String.h>
#include <AK/URLParser.h>
#include <LibJS/Runtime/ModuleRequest.h>
#include <LibWeb/Fetch/Fetching/Fetching.h>
#include <LibWeb/Fetch/Infrastructure/FetchAlgorithms.h>
#include <LibWeb/Fetch/Infrastructure/HTTP/Requests.h>
#include <LibWeb/Fetch/Infrastructure/HTTP/Responses.h>
#include <LibWeb/Fetch/Infrastructure/HTTP/Statuses.h>
#include <LibWeb/HTML/Scripting/Environments.h>
#include <LibWeb/HTML/Scripting/Fetching.h>
#include <LibWeb/HTML/Scripting/ModuleScript.h>
#include <LibWeb/HTML/Window.h>
#include <LibWeb/Infra/Strings.h>
#include <LibWeb/Loader/LoadRequest.h>
#include <LibWeb/Loader/ResourceLoader.h>
#include <LibWeb/MimeSniff/MimeType.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::HTML {

// https://html.spec.whatwg.org/multipage/webappapis.html#module-type-from-module-request
DeprecatedString module_type_from_module_request(JS::ModuleRequest const& module_request)
{
    // 1. Let moduleType be "javascript".
    DeprecatedString module_type = "javascript"sv;

    // 2. If moduleRequest.[[Assertions]] has a Record entry such that entry.[[Key]] is "type", then:
    for (auto const& entry : module_request.assertions) {
        if (entry.key != "type"sv)
            continue;

        // 1. If entry.[[Value]] is "javascript", then set moduleType to null.
        if (entry.value == "javascript"sv)
            module_type = nullptr;
        // 2. Otherwise, set moduleType to entry.[[Value]].
        else
            module_type = entry.value;
    }

    // 3. Return moduleType.
    return module_type;
}

// https://html.spec.whatwg.org/multipage/webappapis.html#resolve-a-module-specifier
WebIDL::ExceptionOr<AK::URL> resolve_module_specifier(Optional<Script&> referring_script, DeprecatedString const& specifier)
{
    // 1. Let settingsObject and baseURL be null.
    Optional<EnvironmentSettingsObject&> settings_object;
    Optional<AK::URL const&> base_url;

    // 2. If referringScript is not null, then:
    if (referring_script.has_value()) {
        // 1. Set settingsObject to referringScript's settings object.
        settings_object = referring_script->settings_object();

        // 2. Set baseURL to referringScript's base URL.
        base_url = referring_script->base_url();
    }
    // 3. Otherwise:
    else {
        // 1. Assert: there is a current settings object.
        // NOTE: This is handled by the current_settings_object() accessor.

        // 2. Set settingsObject to the current settings object.
        settings_object = current_settings_object();

        // 3. Set baseURL to settingsObject's API base URL.
        base_url = settings_object->api_base_url();
    }

    // 4. Let importMap be an empty import map.
    ImportMap import_map;

    // 5. If settingsObject's global object implements Window, then set importMap to settingsObject's global object's import map.
    if (is<Window>(settings_object->global_object()))
        import_map = verify_cast<Window>(settings_object->global_object()).import_map();

    // 6. Let baseURLString be baseURL, serialized.
    auto base_url_string = base_url->serialize();

    // 7. Let asURL be the result of resolving a URL-like module specifier given specifier and baseURL.
    auto as_url = resolve_url_like_module_specifier(specifier, *base_url);

    // 8. Let normalizedSpecifier be the serialization of asURL, if asURL is non-null; otherwise, specifier.
    auto normalized_specifier = as_url.has_value() ? as_url->serialize() : specifier;

    // 9. For each scopePrefix → scopeImports of importMap's scopes:
    for (auto const& entry : import_map.scopes()) {
        // FIXME: Clarify if the serialization steps need to be run here. The steps below assume
        //        scopePrefix to be a string.
        auto const& scope_prefix = entry.key.serialize();
        auto const& scope_imports = entry.value;

        // 1. If scopePrefix is baseURLString, or if scopePrefix ends with U+002F (/) and scopePrefix is a code unit prefix of baseURLString, then:
        if (scope_prefix == base_url_string || (scope_prefix.ends_with("/"sv) && Infra::is_code_unit_prefix(scope_prefix, base_url_string))) {
            // 1. Let scopeImportsMatch be the result of resolving an imports match given normalizedSpecifier, asURL, and scopeImports.
            auto scope_imports_match = TRY(resolve_imports_match(normalized_specifier, as_url, scope_imports));

            // 2. If scopeImportsMatch is not null, then return scopeImportsMatch.
            if (scope_imports_match.has_value())
                return scope_imports_match.release_value();
        }
    }

    // 10. Let topLevelImportsMatch be the result of resolving an imports match given normalizedSpecifier, asURL, and importMap's imports.
    auto top_level_imports_match = TRY(resolve_imports_match(normalized_specifier, as_url, import_map.imports()));

    // 11. If topLevelImportsMatch is not null, then return topLevelImportsMatch.
    if (top_level_imports_match.has_value())
        return top_level_imports_match.release_value();

    // 12. If asURL is not null, then return asURL.
    if (as_url.has_value())
        return as_url.release_value();

    // 13. Throw a TypeError indicating that specifier was a bare specifier, but was not remapped to anything by importMap.
    return WebIDL::SimpleException { WebIDL::SimpleExceptionType::TypeError, String::formatted("Failed to resolve non relative module specifier '{}' from an import map.", specifier).release_value_but_fixme_should_propagate_errors() };
}

// https://html.spec.whatwg.org/multipage/webappapis.html#resolving-an-imports-match
WebIDL::ExceptionOr<Optional<AK::URL>> resolve_imports_match(DeprecatedString const& normalized_specifier, Optional<AK::URL> as_url, ModuleSpecifierMap const& specifier_map)
{
    // 1. For each specifierKey → resolutionResult of specifierMap:
    for (auto const& [specifier_key, resolution_result] : specifier_map) {
        // 1. If specifierKey is normalizedSpecifier, then:
        if (specifier_key == normalized_specifier) {
            // 1. If resolutionResult is null, then throw a TypeError indicating that resolution of specifierKey was blocked by a null entry.
            if (!resolution_result.has_value())
                return WebIDL::SimpleException { WebIDL::SimpleExceptionType::TypeError, String::formatted("Import resolution of '{}' was blocked by a null entry.", specifier_key).release_value_but_fixme_should_propagate_errors() };

            // 2. Assert: resolutionResult is a URL.
            VERIFY(resolution_result->is_valid());

            // 3. Return resolutionResult.
            return resolution_result;
        }

        // 2. If all of the following are true:
        if (
            // - specifierKey ends with U+002F (/);
            specifier_key.ends_with("/"sv) &&
            // - specifierKey is a code unit prefix of normalizedSpecifier; and
            Infra::is_code_unit_prefix(specifier_key, normalized_specifier) &&
            // - either asURL is null, or asURL is special,
            (!as_url.has_value() || as_url->is_special())
            // then:
        ) {
            // 1. If resolutionResult is null, then throw a TypeError indicating that the resolution of specifierKey was blocked by a null entry.
            if (!resolution_result.has_value())
                return WebIDL::SimpleException { WebIDL::SimpleExceptionType::TypeError, String::formatted("Import resolution of '{}' was blocked by a null entry.", specifier_key).release_value_but_fixme_should_propagate_errors() };

            // 2. Assert: resolutionResult is a URL.
            VERIFY(resolution_result->is_valid());

            // 3. Let afterPrefix be the portion of normalizedSpecifier after the initial specifierKey prefix.
            // FIXME: Clarify if this is meant by the portion after the initial specifierKey prefix.
            auto after_prefix = normalized_specifier.substring(specifier_key.length());

            // 4. Assert: resolutionResult, serialized, ends with U+002F (/), as enforced during parsing.
            VERIFY(resolution_result->serialize().ends_with("/"sv));

            // 5. Let url be the result of URL parsing afterPrefix with resolutionResult.
            auto url = URLParser::parse(after_prefix, &*resolution_result);

            // 6. If url is failure, then throw a TypeError indicating that resolution of normalizedSpecifier was blocked since the afterPrefix portion
            //    could not be URL-parsed relative to the resolutionResult mapped to by the specifierKey prefix.
            if (!url.is_valid())
                return WebIDL::SimpleException { WebIDL::SimpleExceptionType::TypeError, String::formatted("Could not resolve '{}' as the after prefix portion could not be URL-parsed.", normalized_specifier).release_value_but_fixme_should_propagate_errors() };

            // 7. Assert: url is a URL.
            VERIFY(url.is_valid());

            // 8. If the serialization of resolutionResult is not a code unit prefix of the serialization of url, then throw a TypeError indicating
            //    that the resolution of normalizedSpecifier was blocked due to it backtracking above its prefix specifierKey.
            if (!Infra::is_code_unit_prefix(resolution_result->serialize(), url.serialize()))
                return WebIDL::SimpleException { WebIDL::SimpleExceptionType::TypeError, String::formatted("Could not resolve '{}' as it backtracks above its prefix specifierKey.", normalized_specifier).release_value_but_fixme_should_propagate_errors() };

            // 9. Return url.
            return url;
        }
    }

    // 2. Return null.
    return Optional<AK::URL> {};
}

// https://html.spec.whatwg.org/multipage/webappapis.html#resolving-a-url-like-module-specifier
Optional<AK::URL> resolve_url_like_module_specifier(DeprecatedString const& specifier, AK::URL const& base_url)
{
    // 1. If specifier starts with "/", "./", or "../", then:
    if (specifier.starts_with("/"sv) || specifier.starts_with("./"sv) || specifier.starts_with("../"sv)) {
        // 1. Let url be the result of URL parsing specifier with baseURL.
        auto url = URLParser::parse(specifier, &base_url);

        // 2. If url is failure, then return null.
        if (!url.is_valid())
            return {};

        // 3. Return url.
        return url;
    }

    // 2. Let url be the result of URL parsing specifier (with no base URL).
    auto url = URLParser::parse(specifier);

    // 3. If url is failure, then return null.
    if (!url.is_valid())
        return {};

    // 4. Return url.
    return url;
}

// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-a-single-module-script
void fetch_single_module_script(AK::URL const& url, EnvironmentSettingsObject& fetch_client_settings_object, Fetch::Infrastructure::Requesting::Destination destination, ScriptFetchOptions const& options, EnvironmentSettingsObject& module_map_settings_object, Fetch::Infrastructure::Request::ReferrerType const& referrer, Optional<JS::ModuleRequest> const& module_request, TopLevelModule top_level_module, ModuleCallback on_complete)
{
    // 1. Let moduleType be "javascript".
    DeprecatedString module_type = "javascript"sv;

    // 2. If moduleRequest was given, then set moduleType to the result of running the module type from module request steps given moduleRequest.
    if (module_request.has_value())
        module_type = module_type_from_module_request(*module_request);

    // 3. Assert: the result of running the module type allowed steps given moduleType and module map settings object is true.
    //    Otherwise we would not have reached this point because a failure would have been raised when inspecting moduleRequest.[[Assertions]]
    //    in create a JavaScript module script or fetch an import() module script graph.
    VERIFY(module_map_settings_object.module_type_allowed(module_type));

    // 4. Let moduleMap be module map settings object's module map.
    auto& module_map = module_map_settings_object.module_map();

    // 5. If moduleMap[(url, moduleType)] is "fetching", wait in parallel until that entry's value changes,
    //    then queue a task on the networking task source to proceed with running the following steps.
    if (module_map.is_fetching(url, module_type)) {
        module_map.wait_for_change(url, module_type, [on_complete = move(on_complete)](auto entry) {
            // FIXME: This should queue a task.

            // FIXME: This should run other steps, for now we just assume the script loaded.
            VERIFY(entry.type == ModuleMap::EntryType::ModuleScript);

            on_complete(entry.module_script);
        });

        return;
    }

    // 6. If moduleMap[(url, moduleType)] exists, run onComplete given moduleMap[(url, moduleType)], and return.
    auto entry = module_map.get(url, module_type);
    if (entry.has_value() && entry->type == ModuleMap::EntryType::ModuleScript) {
        on_complete(entry->module_script);
        return;
    }

    // 7. Set moduleMap[(url, moduleType)] to "fetching".
    module_map.set(url, module_type, { ModuleMap::EntryType::Fetching, nullptr });

    // 8. Let request be a new request whose URL is url, destination is destination,
    //    mode is "cors", referrer is referrer, and client is fetch client settings object.
    auto request = Fetch::Infrastructure::Request::create(module_map_settings_object.vm());
    request->set_url(url);
    request->set_destination(destination);
    request->set_mode(Fetch::Infrastructure::Requesting::Mode::CORS);
    request->set_referrer(referrer);
    request->set_client(&fetch_client_settings_object);

    // 9. If destination is "worker", "sharedworker", or "serviceworker",
    //    and the top-level module fetch flag is set, then set request's mode to "same-origin".
    if (
        (destination == Fetch::Infrastructure::Requesting::Destination::Worker || destination == Fetch::Infrastructure::Requesting::Destination::ServiceWorker)
        && top_level_module == TopLevelModule::Yes) {
        request->set_mode(Fetch::Infrastructure::Requesting::Mode::SameOrigin);
    }

    // 10. Set request's initiator type to script".
    request->set_initiator_type(Fetch::Infrastructure::Requesting::InitiatorType::Script);

    // 11. Set up the module script request given request and options.
    ScriptFetchOptions::setup_module_script_request(request, options);

    // FIXME: 12. If performFetch was given, run performFetch with request, isTopLevel, and with processResponseConsumeBody as defined below.

    auto process_response_consume_body = [&module_map, &url, &module_type, &module_map_settings_object, &options, on_complete = move(on_complete)](JS::NonnullGCPtr<Fetch::Infrastructure::Response> response, Variant<Empty, Fetch::Infrastructure::FetchAlgorithms::ConsumeBodyFailureTag, ByteBuffer> bodyBytes) {
        // 1. If either of the following conditions are met:
        if (
            // bodyBytes is null or failure; or
            (bodyBytes.has<Empty>() || bodyBytes.has<Fetch::Infrastructure::FetchAlgorithms::ConsumeBodyFailureTag>()) ||
            // response's status is not an ok status,
            !Fetch::Infrastructure::is_ok_status(response->status())) {
            // then set moduleMap[(url, moduleType)] to null, run onComplete given null, and abort these steps.
            // FIXME: Clarify if the Failed type is meant by null.
            module_map.set(url, module_type, { ModuleMap::EntryType::Failed, nullptr });
            on_complete(nullptr);
            return;
        }

        // 2. Let source text be the result of UTF-8 decoding bodyBytes.
        auto source_text = MUST(String::from_utf8(bodyBytes.get<ByteBuffer>()));

        // 3. Let MIME type be the result of extracting a MIME type from response's header list.
        auto mime_type = response->header_list()->extract_mime_type().release_value_but_fixme_should_propagate_errors();

        // 4. Let module script be null.
        JS::GCPtr<JavaScriptModuleScript> module_script;

        // 5. If MIME type is a JavaScript MIME type and moduleType is "javascript",
        //    then set module script to the result of creating a JavaScript module script given source text, module map settings object, response's URL, and options.
        if (mime_type.has_value() && mime_type->is_javascript() && module_type == "javascript"sv)
            module_script = JavaScriptModuleScript::create(url.basename(), source_text.to_deprecated_string(), module_map_settings_object, *response->url(), options).release_value_but_fixme_should_propagate_errors();

        // 6. If the MIME type essence of MIME type is "text/css" and moduleType is "css",
        //    then set module script to the result of creating a CSS module script given source text and module map settings object.
        if (mime_type.has_value() && mime_type->essence() == "text/css"sv && module_type == "css"sv) {
            // FIXME: Implement CSS module script handling.
            dbgln("Handling of css module scripts not implemented for script fetching");
            on_complete(nullptr);
            return;
        }

        // 7. If MIME type essence is a JSON MIME type and moduleType is "json",
        //    then set module script to the result of creating a JSON module script given source text and module map settings object.
        if (mime_type.has_value() && mime_type->is_json() && module_type == "json"sv) {
            // FIXME: Implement JSON module script handling.
            dbgln("Handling of json module scripts not implemented for script fetching");
            on_complete(nullptr);
            return;
        }

        // 8. Set moduleMap[(url, moduleType)] to module script, and run onComplete given module script.
        module_map.set(url, module_type, { ModuleMap::EntryType::ModuleScript, module_script });
        on_complete(module_script);
    };

    // Otherwise, fetch request with processResponseConsumeBody set to processResponseConsumeBody as defined below.
    // FIXME: Handle exception
    (void)Fetch::Fetching::fetch(
        fetch_client_settings_object.realm(),
        request,
        Fetch::Infrastructure::FetchAlgorithms::create(fetch_client_settings_object.realm().vm(), {
                                                                                                      .process_request_body_chunk_length = {},
                                                                                                      .process_request_end_of_body = {},
                                                                                                      .process_early_hints_response = {},
                                                                                                      .process_response = {},
                                                                                                      .process_response_end_of_body = {},
                                                                                                      .process_response_consume_body = move(process_response_consume_body),
                                                                                                  }));
}

// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-a-single-imported-module-script
void fetch_single_imported_module_script(AK::URL const& url, EnvironmentSettingsObject& settings_object, Fetch::Infrastructure::Requesting::Destination destination, ScriptFetchOptions const& options, Fetch::Infrastructure::Request::ReferrerType const& referrer, JS::ModuleRequest module_request, ModuleCallback on_complete)
{
    // 1. Assert: moduleRequest.[[Assertions]] does not contain any Record entry such that entry.[[Key]] is not "type",
    //    because we only asked for "type" assertions in HostGetSupportedImportAssertions.
    VERIFY(all_of(module_request.assertions, [](auto const& assertion) { return assertion.key == "type"sv; }));

    // 2. Let moduleType be the result of running the module type from module request steps given moduleRequest.
    auto module_type = module_type_from_module_request(module_request);

    // 3. If the result of running the module type allowed steps given moduleType and settings object is false, then run onComplete given null, and return.
    if (!settings_object.module_type_allowed(module_type)) {
        on_complete(nullptr);
        return;
    }

    // 4. Fetch a single module script given url, settings object, destination, options, settings object, referrer, moduleRequest,
    //    false, and onComplete. If performFetch was given, pass it along as well.
    // FIXME: Pass performFetch if given.
    fetch_single_module_script(url, settings_object, destination, options, settings_object, referrer, module_request, TopLevelModule::No, move(on_complete));
}

// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-a-module-script-tree
void fetch_external_module_script_graph(AK::URL const& url, EnvironmentSettingsObject& settings_object, ScriptFetchOptions const& options, ModuleCallback on_complete)
{
    // 1. Disallow further import maps given settings object.
    settings_object.disallow_further_import_maps();

    // 2. Fetch a single module script given url, settings object, "script", options, settings object, "client", true, and with the following steps given result:
    fetch_single_module_script(url, settings_object, Fetch::Infrastructure::Requesting::Destination::Script, options, settings_object, Fetch::Infrastructure::Requesting::Referrer::Client, {}, TopLevelModule::Yes, [&settings_object, on_complete = move(on_complete), url](auto* result) mutable {
        // 1. If result is null, run onComplete given null, and abort these steps.
        if (!result) {
            on_complete(nullptr);
            return;
        }

        // 2. Fetch the descendants of and link result given settings object, "script", and onComplete.
        fetch_descendants_of_and_link_a_module_script(*result, settings_object, Fetch::Infrastructure::Requesting::Destination::Script, move(on_complete));
    });
}

// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-an-inline-module-script-graph
void fetch_inline_module_script_graph(DeprecatedString const& filename, DeprecatedString const& source_text, AK::URL const& base_url, EnvironmentSettingsObject& settings_object, ScriptFetchOptions const& options, ModuleCallback on_complete)
{
    // 1. Disallow further import maps given settings object.
    settings_object.disallow_further_import_maps();

    // 2. Let script be the result of creating a JavaScript module script using source text, settings object, base URL, and options.
    auto script = JavaScriptModuleScript::create(filename, source_text.view(), settings_object, base_url, options).release_value_but_fixme_should_propagate_errors();

    // 3. If script is null, run onComplete given null, and return.
    if (!script) {
        on_complete(nullptr);
        return;
    }

    // 4. Fetch the descendants of and link script, given settings object, "script", and onComplete.
    fetch_descendants_of_and_link_a_module_script(*script, settings_object, Fetch::Infrastructure::Requesting::Destination::Script, move(on_complete));
}

// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-the-descendants-of-and-link-a-module-script
void fetch_descendants_of_and_link_a_module_script(JavaScriptModuleScript& module_script, EnvironmentSettingsObject& fetch_client_settings_object, Fetch::Infrastructure::Requesting::Destination destination, ModuleCallback on_complete)
{
    // 1. Let record be module script's record.
    auto* record = module_script.record();

    // 2. If record is null, then:
    if (!record) {
        // FIXME: 1. Set module script's error to rethrow to module script's parse error.

        // 2. Run onComplete given module script.
        on_complete(&module_script);

        // 3. Return.
        return;
    }

    // 3. Let state be Record { [[ParseError]]: null, [[Destination]]: destination, [[PerformFetch]]: null }.
    auto state = FetchContext { destination, nullptr };

    // FIXME: 4. If performFetch was given, set state.[[PerformFetch]] to performFetch.

    // FIXME: Our implementation requires a current_realm() when calling this function, as we allocated
    //        a native function in the realm's heap() in new_promise_capability, NativeFunction::create
    fetch_client_settings_object.realm().vm().push_execution_context(fetch_client_settings_object.realm_execution_context());

    // 5. Let loadingPromise be record.LoadRequestedModules(state).
    auto const& loading_promise = record->load_requested_modules(fetch_client_settings_object.realm(), state);

    // 6. Upon fulfillment of loadingPromise, run the following steps:
    WebIDL::upon_fulfillment(loading_promise, [&record, &fetch_client_settings_object, &module_script, on_complete = move(on_complete)](auto) {
        // 1. Perform record.Link().
        auto linking_result = record->link(fetch_client_settings_object.realm().vm());

        // FIXME: If this throws an exception, set result's error to rethrow to that exception.
        if (linking_result.is_error()) {
            dbgln("Linking module script record caused an exception");
            return JS::js_undefined();
        }

        // 2. Run onComplete given module script.
        on_complete(&module_script);

        // FIXME: See fixme about current_realm() above. The call to WebIDL::upon_fulfillment ends up allocating
        //        a native function as well.
        fetch_client_settings_object.realm().vm().pop_execution_context();

        return JS::js_undefined();
    });

    // 7. Upon rejection of loadingPromise, run the following steps:
    WebIDL::upon_rejection(loading_promise, [state, &fetch_client_settings_object, on_complete = move(on_complete)](auto) {
        // FIXME: 1. If state.[[ParseError]] is not null, set module script's error to rethrow to state.[[ParseError]] and run onComplete given module script.

        // 2. Otherwise, run onComplete given null.
        on_complete(nullptr);

        // FIXME: See fixme about current_realm() above. The call to WebIDL::upon_rejection ends up allocating
        //        a native function as well.
        fetch_client_settings_object.realm().vm().pop_execution_context();

        return JS::js_undefined();
    });
}

}
