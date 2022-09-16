/*
 * Copyright (c) 2022, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Fetch/Infrastructure/HTTP/Requests.h>
#include <LibWeb/HTML/Scripting/Environments.h>
#include <LibWeb/HTML/Scripting/Fetching.h>
#include <LibWeb/HTML/Scripting/ModuleScript.h>
#include <LibWeb/Loader/LoadRequest.h>
#include <LibWeb/Loader/ResourceLoader.h>
#include <LibWeb/MimeSniff/MimeType.h>

namespace Web::HTML {

// https://html.spec.whatwg.org/multipage/webappapis.html#internal-module-script-graph-fetching-procedure
void fetch_internal_module_script_graph(JS::ModuleRequest const& module_request, EnvironmentSettingsObject& fetch_client_settings_object, StringView destination, EnvironmentSettingsObject& module_script_settings_object, HashTable<ModuleLocationTuple> const& visited_set, AK::URL const& referrer, ModuleCallback callback)
{
    // 1. Let url be the result of resolving a module specifier given referrer and moduleRequest.[[Specifier]].
    auto url = module_request.resolve_specifier(referrer);

    // 2. Assert: url is never failure, because resolving a module specifier must have been previously successful with these same two arguments.
    VERIFY(url.is_valid());
    dbgln("fetch_internal_module_script_graph {}", url);

    // 3. Let moduleType be the result of running the module type from module request steps given moduleRequest.
    auto module_type = module_request.module_type();

    // 4. Assert: visited set contains (url, moduleType).
    VERIFY(visited_set.contains({ url, module_type }));

    // 5. Fetch a single module script given url, fetch client settings object, destination, options, module map settings object,
    //    referrer, moduleRequest, and with the top-level module fetch flag unset.
    //    If the caller of this algorithm specified custom perform the fetch steps, pass those along while fetching a single module script.
    // FIXME: Pass options.
    fetch_single_module_script(url, fetch_client_settings_object, destination, module_script_settings_object, referrer, module_request, [callback = move(callback), &fetch_client_settings_object, &destination, &visited_set](auto const* result) {
        // 6. Return from this algorithm, and run the following steps when fetching a single module script asynchronously completes with result:

        // 7. If result is null, asynchronously complete this algorithm with null, and return.
        if (!result) {
            callback(nullptr);
            return;
        }

        // 8. Fetch the descendants of result given fetch client settings object, destination, and visited set.
        fetch_descendants_of_a_module_script(*result, fetch_client_settings_object, destination, visited_set, [&callback](auto const* result) {
            // 9. When the appropriate algorithm asynchronously completes with final result, asynchronously complete this algorithm with final result.
            callback(result);
        });
    });
}

// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-the-descendants-of-a-module-script
void fetch_descendants_of_a_module_script(JavaScriptModuleScript const& module_script, EnvironmentSettingsObject& fetch_client_settings_object, StringView destination, HashTable<ModuleLocationTuple> visited_set, ModuleCallback callback)
{
    // 1. If module script's record is null, then asynchronously complete this algorithm with module script and return.
    if (!module_script.record()) {
        dbgln("no requested modules!");
        callback(&module_script);
        return;
    }

    // 2. Let record be module script's record.
    auto const& record = module_script.record();

    // 3. If record is not a Cyclic Module Record, or if record.[[RequestedModules]] is empty, asynchronously complete this algorithm with module script.
    // FIXME: Currently record is always a cyclic module.
    if (record->requested_modules().is_empty()) {
        callback(&module_script);
        return;
    }

    // 4. Let moduleRequests be a new empty list.
    Vector<JS::ModuleRequest> module_requests;

    // 5. For each ModuleRequest Record requested of record.[[RequestedModules]],
    for (auto const& requested : record->requested_modules()) {
        // 1. Let url be the result of resolving a module specifier given module script's base URL and requested.[[Specifier]].
        auto url = requested.resolve_specifier(module_script.base_url());

        // 2. Assert: url is never failure, because resolving a module specifier must have been previously successful with these same two arguments.
        dbgln("4");
        VERIFY(url.is_valid());

        // 3. Let moduleType be the result of running the module type from module request steps given requested.
        auto module_type = requested.module_type();

        // 2. If visited set does not contain (url, moduleType), then:
        if (!visited_set.contains({ url, module_type })) {
            // 1. Append requested to moduleRequests.
            module_requests.append(requested);

            // 2. Append (url, moduleType) to visited set.
            visited_set.set({ url, module_type });
        }
    }

    dbgln("got {} modules to request from {}", module_requests.size(), record->requested_modules().size());

    // FIXME: 6. Let options be the descendant script fetch options for module script's fetch options.

    // FIXME: 7. Assert: options is not null, as module script is a JavaScript module script.

    // 8. For each moduleRequest in moduleRequests, perform the internal module script graph fetching procedure given moduleRequest,
    //    fetch client settings object, destination, options, module script's settings object, visited set,
    //    and module script's base URL. If the caller of this algorithm specified custom perform the fetch steps,
    //    pass those along while performing the internal module script graph fetching procedure.
    for (auto const& request : module_requests) {
        dbgln("recursing module request: {}", request.module_specifier);
        // FIXME: These invocations of the internal module script graph fetching procedure should be performed in parallel to each other.

        // FIXME: If any of the invocations of the internal module script graph fetching procedure asynchronously complete with null,
        //        asynchronously complete this algorithm with null, and return.

        // FIXME: Otherwise, wait until all of the internal module script graph fetching procedure invocations have asynchronously completed.
        //        Asynchronously complete this algorithm with module script.
        // FIXME: Pass options.
        fetch_internal_module_script_graph(request, fetch_client_settings_object, destination, module_script.settings_object(), visited_set, module_script.base_url(), [](auto const*) {
            dbgln("about to hit this TODO!");
            TODO();
        });
    }
}

// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-a-single-module-script
void fetch_single_module_script(AK::URL const& url, EnvironmentSettingsObject&, StringView, EnvironmentSettingsObject& module_map_settings_object, AK::URL const&, Optional<JS::ModuleRequest> const& module_request, ModuleCallback callback)
{
    // 1. Let moduleType be "javascript".
    String module_type = "javascript"sv;
    dbgln("fetching inline script! {}", url);

    // 2. If moduleRequest was given, then set moduleType to the result of running the module type from module request steps given moduleRequest.
    if (module_request.has_value())
        module_type = module_request->module_type();

    // 3. Assert: the result of running the module type allowed steps given moduleType and module map settings object is true.
    //    Otherwise we would not have reached this point because a failure would have been raised when inspecting moduleRequest.[[Assertions]]
    //    in create a JavaScript module script or fetch an import() module script graph.
    VERIFY(module_map_settings_object.module_type_allowed(module_type));

    // 4. Let moduleMap be module map settings object's module map.
    auto module_map = module_map_settings_object.module_map();

    // 5. If moduleMap[(url, moduleType)] is "fetching", wait in parallel until that entry's value changes,
    //    then queue a task on the networking task source to proceed with running the following steps.
    if (module_map.is_fetching(url, module_type)) {
        TODO();
        // FIXME: Implement queuing.
    }

    // 6. If moduleMap[(url, moduleType)] exists, asynchronously complete this algorithm with moduleMap[url / moduleType], and return.
    // FIXME: Check if exists is the same as "has ModuleScript".
    auto entry = module_map.get(url, module_type);
    if (entry.has_value() && entry->type == ModuleMap::EntryType::ModuleScript) {
        callback(entry->module_script);
        return;
    }

    // 7. Set moduleMap[(url, moduleType)] to "fetching".
    module_map.set(url, module_type, { ModuleMap::EntryType::Fetching, nullptr });

    // FIXME: Implement non ad-hoc version of steps 8 to 11.

    auto request = LoadRequest::create_for_url_on_page(url, nullptr);

    ResourceLoader::the().load(
        request,
        [url, module_type, &module_map_settings_object, callback = move(callback)](StringView data, auto& response_headers, auto) {
            dbgln("got inline module result! {}", url);
            if (data.is_null()) {
                dbgln("HTMLScriptElement: Failed to load {}", url);
                return;
            }

            auto content_type_header = response_headers.get("Content-Type");
            if (!content_type_header.has_value()) {
                dbgln("no content type! {}", url);
                return;
            }

            if (MimeSniff::is_javascript_mime_type(*content_type_header) && module_type == "javascript"sv) {
                dbgln("callback module of", url);
                callback(JavaScriptModuleScript::create(url.basename(), data, module_map_settings_object, url));
                return;
            }

            dbgln("no JS content type! {} of type {}, with content {}", url, module_type, *content_type_header);
        },
        [](auto&, auto) {
            dbgln("HONK! Failed to load script, but ready nonetheless.");
        });
}

// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-a-module-script-tree
void fetch_external_module_script_graph(AK::URL const& url, EnvironmentSettingsObject& settings_object, ModuleCallback callback)
{
    // 1. Fetch a single module script given url, settings object, "script", options, settings object, "client",
    //    and with the top-level module fetch flag set. If the caller of this algorithm specified custom perform the fetch steps,
    //    pass those along as well. Wait until the algorithm asynchronously completes with result.
    fetch_single_module_script(url, settings_object, "script"sv, settings_object, "client"sv, {}, [&settings_object, &callback, &url](auto* result) {
        dbgln("here1");

        // 2. If result is null, asynchronously complete this algorithm with null, and return.
        if (!result) {
            callback(nullptr);
            return;
        }

        // 3. Let visited set be « (url, "javascript") ».
        HashTable<ModuleLocationTuple> visited_set;
        visited_set.set({ url, "javascript"sv });

        // 4. Fetch the descendants of and link result given settings object, "script", and visited set.
        //    When this asynchronously completes with final result, asynchronously complete this algorithm with final result.
        fetch_descendants_of_and_link_a_module_script(*result, settings_object, "script"sv, visited_set, move(callback));
    });
}

// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-an-inline-module-script-graph
void fetch_inline_module_script_graph(String const& filename, String const& source_text, AK::URL const& base_url, EnvironmentSettingsObject& settings_object, ModuleCallback callback)
{
    dbgln("fetch inline script graph {}", filename);
    // 1. Let script be the result of creating a JavaScript module script using source text, settings object, base URL, and options.
    auto script = JavaScriptModuleScript::create(filename, source_text.view(), settings_object, base_url);

    // 2. If script is null, asynchronously complete this algorithm with null, and return.
    if (!script) {
        callback(nullptr);
        return;
    }

    // 3. Let visited set be an empty set.
    HashTable<ModuleLocationTuple> visited_set;

    // 4. Fetch the descendants of and link script, given settings object, the destination "script",
    //    and visited set. When this asynchronously completes with final result, asynchronously complete this algorithm with final result.
    fetch_descendants_of_and_link_a_module_script(*script, settings_object, "script"sv, visited_set, move(callback));
}

// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-the-descendants-of-and-link-a-module-script
void fetch_descendants_of_and_link_a_module_script(JavaScriptModuleScript const& module_script, EnvironmentSettingsObject& fetch_client_settings_object, StringView destination, HashTable<ModuleLocationTuple> const& visited_set, ModuleCallback callback)
{
    dbgln("fetch descendants!");
    // 1. Fetch the descendants of module script, given fetch client settings object, destination, and visited set.
    fetch_descendants_of_a_module_script(module_script, fetch_client_settings_object, destination, visited_set, [&callback](JavaScriptModuleScript const* result) {
        // 2. Return from this algorithm, and run the following steps when fetching the descendants of a module script asynchronously completes with result.

        // 3. If result is null, then asynchronously complete this algorithm with result.
        if (!result) {
            dbgln("got null result to get descendants from");
            callback(nullptr);
            return;
        }

        // FIXME: 4. Let parse error be the result of finding the first parse error given result.

        // 5. If parse error is null, then:
        if (result->record() != nullptr) {
            dbgln("did parse so linking!");
            // 1. Let record be result's record.
            auto const& record = *result->record();

            // 2. Perform record.Link().
            auto linking_result = const_cast<JS::SourceTextModule&>(record).link(result->vm());

            // NOTE: This step will recursively call Link on all of the module's unlinked dependencies.
            //       If this throws an exception, set result's error to rethrow to that exception.
            if (linking_result.is_error()) {
                // FIXME: How do we pass the error? We can only pass a JavaScriptModuleScript pointer....
                callback(nullptr);
            }
        } else {
            dbgln("got null record to get descendants");
            // FIXME: 6. Otherwise, set result's error to rethrow to parse error.
        }

        // 7. Asynchronously complete this algorithm with result.
        callback(result);
    });
}

}
