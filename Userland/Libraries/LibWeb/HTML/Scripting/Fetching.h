/*
 * Copyright (c) 2022, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/HTML/Scripting/ModuleScript.h>

namespace Web::HTML {

using ModuleCallback = Function<void(JavaScriptModuleScript const*)>;

void fetch_internal_module_script_graph(JS::ModuleRequest const& module_request, EnvironmentSettingsObject& fetch_client_settings_object, StringView destination, EnvironmentSettingsObject& module_script_settings_object, HashTable<ModuleLocationTuple> const& visited_set, AK::URL const& referrer, ModuleCallback callback);
void fetch_external_module_script_graph(AK::URL const&, EnvironmentSettingsObject& settings_object, ModuleCallback callback);
void fetch_inline_module_script_graph(String const& filename, String const& source_text, AK::URL const& base_url, EnvironmentSettingsObject& settings_object, ModuleCallback callback);

void fetch_descendants_of_a_module_script(JavaScriptModuleScript const& module_script, EnvironmentSettingsObject& fetch_client_settings_object, StringView destination, HashTable<ModuleLocationTuple> visited_set, ModuleCallback callback);
void fetch_descendants_of_and_link_a_module_script(JavaScriptModuleScript const& module_script, EnvironmentSettingsObject& fetch_client_settings_object, StringView destination, HashTable<ModuleLocationTuple> const& visited_set, ModuleCallback callback);

void fetch_single_module_script(AK::URL const&, EnvironmentSettingsObject& fetch_client_settings_object, StringView destination, EnvironmentSettingsObject& module_map_settings_object, AK::URL const& referrer, Optional<JS::ModuleRequest> const&, ModuleCallback callback);

}
