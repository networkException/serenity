/*
 * Copyright (c) 2023, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Fetch/Request.h>
#include <LibWeb/HTML/Scripting/FetchOptions.h>
#include <LibWeb/ReferrerPolicy/ReferrerPolicy.h>

namespace Web::HTML {

// https://html.spec.whatwg.org/multipage/webappapis.html#default-classic-script-fetch-options
ScriptFetchOptions ScriptFetchOptions::default_classic_script()
{
    // The default classic script fetch options are a script fetch options whose
    return {
        // cryptographic nonce is the empty string,
        .cryptographic_nonce = String(),
        // integrity metadata is the empty string,
        .integrity_metadata = String(),
        // parser metadata is "not-parser-inserted",
        .parser_metadata = Fetch::Infrastructure::Requesting::ParserMetadata::NotParserInserted,
        // credentials mode is "same-origin",
        .credentials_mode = Fetch::Infrastructure::Requesting::CredentialsMode::SameOrigin,
        // referrer policy is the empty string,
        .referrer_policy = ReferrerPolicy::ReferrerPolicy::EmptyString,
        // and fetch priority is "auto".
        .fetch_priority = Fetch::Infrastructure::Requesting::Priority::Auto
    };
}

// https://html.spec.whatwg.org/multipage/webappapis.html#descendant-script-fetch-options
ScriptFetchOptions ScriptFetchOptions::descendant_script(ScriptFetchOptions options)
{
    // For any given script fetch options options, the descendant script fetch options are
    // a new script fetch options whose items all have the same values,

    // except for the integrity metadata, which is instead the empty string,
    options.integrity_metadata = String();
    // and the fetch priority, which is instead "auto".
    options.fetch_priority = Fetch::Infrastructure::Requesting::Priority::Auto;

    return options;
}

// https://html.spec.whatwg.org/multipage/webappapis.html#set-up-the-classic-script-request
void ScriptFetchOptions::setup_classic_script_request(Fetch::Infrastructure::Request& request, ScriptFetchOptions const& options)
{
    // Set request's cryptographic nonce metadata to options's cryptographic nonce,
    request.set_cryptographic_nonce_metadata(options.cryptographic_nonce);
    // its integrity metadata to options's integrity metadata,
    request.set_integrity_metadata(options.integrity_metadata);
    // its parser metadata to options's parser metadata,
    request.set_parser_metadata(options.parser_metadata);
    // its referrer policy to options's referrer policy,
    request.set_referrer_policy(options.referrer_policy);
    // its render-blocking to options's render-blocking,
    request.set_render_blocking(options.render_blocking);
    // and its priority to options's fetch priority.
    request.set_priority(options.fetch_priority);
}

// https://html.spec.whatwg.org/multipage/webappapis.html#set-up-the-module-script-request
void ScriptFetchOptions::setup_module_script_request(Fetch::Infrastructure::Request& request, ScriptFetchOptions const& options)
{
    // Set request's cryptographic nonce metadata to options's cryptographic nonce,
    request.set_cryptographic_nonce_metadata(options.cryptographic_nonce);
    // its integrity metadata to options's integrity metadata,
    request.set_integrity_metadata(options.integrity_metadata);
    // its parser metadata to options's parser metadata,
    request.set_parser_metadata(options.parser_metadata);
    // its credentials mode to options's credentials mode,
    request.set_credentials_mode(options.credentials_mode);
    // its referrer policy to options's referrer policy,
    request.set_referrer_policy(options.referrer_policy);
    // its render-blocking to options's render-blocking,
    request.set_render_blocking(options.render_blocking);
    // and its priority to options's fetch priority.
    request.set_priority(options.fetch_priority);
}

}
