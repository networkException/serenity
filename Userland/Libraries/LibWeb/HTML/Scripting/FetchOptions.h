/*
 * Copyright (c) 2023, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <LibWeb/Fetch/Infrastructure/HTTP/Requesting.h>

namespace Web::HTML {

// https://html.spec.whatwg.org/multipage/webappapis.html#script-fetch-options
struct ScriptFetchOptions {
    // https://html.spec.whatwg.org/multipage/webappapis.html#concept-script-fetch-options-nonce
    // The cryptographic nonce metadata used for the initial fetch and for fetching any imported modules
    String cryptographic_nonce;

    // https://html.spec.whatwg.org/multipage/webappapis.html#concept-script-fetch-options-integrity
    // The integrity metadata used for the initial fetch
    String integrity_metadata;

    // https://html.spec.whatwg.org/multipage/webappapis.html#concept-script-fetch-options-parser
    // The parser metadata used for the initial fetch and for fetching any imported modules
    Fetch::Infrastructure::Requesting::ParserMetadata parser_metadata;

    // https://html.spec.whatwg.org/multipage/webappapis.html#concept-script-fetch-options-credentials
    // The credentials mode used for the initial fetch (for module scripts) and
    // for fetching any imported modules (for both module scripts and classic scripts)
    Fetch::Infrastructure::Requesting::CredentialsMode credentials_mode;

    // https://html.spec.whatwg.org/multipage/webappapis.html#concept-script-fetch-options-referrer-policy
    // The referrer policy used for the initial fetch and for fetching any imported modules
    ReferrerPolicy::ReferrerPolicy referrer_policy;

    // https://html.spec.whatwg.org/multipage/webappapis.html#concept-script-fetch-options-render-blocking
    // The boolean value of render-blocking used for the initial fetch and for fetching any imported modules.
    // Unless otherwise stated, its value is false.
    bool render_blocking { false };

    // https://html.spec.whatwg.org/multipage/webappapis.html#concept-script-fetch-options-fetch-priority
    // The priority used for the initial fetch
    Fetch::Infrastructure::Requesting::Priority fetch_priority;

    static ScriptFetchOptions default_classic_script();
    static ScriptFetchOptions descendant_script(ScriptFetchOptions options);

    static void setup_classic_script_request(Fetch::Infrastructure::Request& request, ScriptFetchOptions const& options);
    static void setup_module_script_request(Fetch::Infrastructure::Request& request, ScriptFetchOptions const& options);
};

}
