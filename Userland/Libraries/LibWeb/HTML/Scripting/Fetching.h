/*
 * Copyright (c) 2022-2023, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Fetch/Infrastructure/HTTP/Requests.h>
#include <LibWeb/HTML/Scripting/ImportMap.h>
#include <LibWeb/HTML/Scripting/ModuleMap.h>
#include <LibWeb/HTML/Scripting/ModuleScript.h>

namespace Web::HTML {

struct FetchContext : public JS::GraphLoadingState::HostDefined {
    FetchContext(Fetch::Infrastructure::Requesting::Destination destination, JS::Promise* perform_fetch)
        : destination(destination)
        , perform_fetch(perform_fetch)
    {
    }

    virtual ~FetchContext() override = default;
    virtual void visit_edges(JS::Cell::Visitor&) override { }

    // FIXME:                                                   // [[ParseError]]
    Fetch::Infrastructure::Requesting::Destination destination; // [[Destination]]
    JS::Promise* perform_fetch;                                 // [[PerformFetch]]
};

using ModuleCallback = Function<void(JavaScriptModuleScript*)>;

DeprecatedString module_type_from_module_request(JS::ModuleRequest const&);
WebIDL::ExceptionOr<AK::URL> resolve_module_specifier(Optional<Script&> referring_script, DeprecatedString const& specifier);
WebIDL::ExceptionOr<Optional<AK::URL>> resolve_imports_match(DeprecatedString const& normalized_specifier, Optional<AK::URL> as_url, ModuleSpecifierMap const&);
Optional<AK::URL> resolve_url_like_module_specifier(DeprecatedString const& specifier, AK::URL const& base_url);

void fetch_external_module_script_graph(AK::URL const&, EnvironmentSettingsObject& settings_object, ScriptFetchOptions const&, ModuleCallback on_complete);
void fetch_inline_module_script_graph(DeprecatedString const& filename, DeprecatedString const& source_text, AK::URL const& base_url, EnvironmentSettingsObject& settings_object, ScriptFetchOptions const&, ModuleCallback on_complete);

void fetch_descendants_of_and_link_a_module_script(JavaScriptModuleScript& module_script, EnvironmentSettingsObject& fetch_client_settings_object, Fetch::Infrastructure::Requesting::Destination, ModuleCallback on_complete);

enum class TopLevelModule {
    Yes,
    No
};

void fetch_single_module_script(AK::URL const&, EnvironmentSettingsObject& fetch_client_settings_object, Fetch::Infrastructure::Requesting::Destination, ScriptFetchOptions const&, EnvironmentSettingsObject& module_map_settings_object, Fetch::Infrastructure::Request::ReferrerType const&, Optional<JS::ModuleRequest> const&, TopLevelModule, ModuleCallback callback);
void fetch_single_imported_module_script(AK::URL const&, EnvironmentSettingsObject& settings_object, Fetch::Infrastructure::Requesting::Destination, ScriptFetchOptions const&, Fetch::Infrastructure::Request::ReferrerType const&, JS::ModuleRequest, ModuleCallback on_complete);

}
