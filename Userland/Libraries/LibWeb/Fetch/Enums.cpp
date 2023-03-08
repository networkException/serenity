/*
 * Copyright (c) 2022, Linus Groh <linusg@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Bindings/RequestPrototype.h>
#include <LibWeb/Bindings/ResponsePrototype.h>
#include <LibWeb/Fetch/Enums.h>
#include <LibWeb/Fetch/Infrastructure/HTTP/Requests.h>
#include <LibWeb/ReferrerPolicy/ReferrerPolicy.h>

namespace Web::Fetch {

// We have a handful of enums that have both a generated and a handwritten version, and need to
// convert between some of them. This has three reasons:
// - Some enums have more internal values in the spec than what is exposed to JS. An example of
//   this is Request::Destination's ServiceWorker member and Request::Mode's WebSocket member,
//   both of which are not present in the IDL-defined enums.
// - The generated enums are not perfect, e.g. "no-cors" becomes NoCors, not NoCORS. This is fine
//   for the generated constructor/prototype code, but not great for the remaining handwritten
//   code.
// - Fetch has use-cases beyond its JS interface, so having to refer to the 'Bindings' namespace
//   constantly is irritating.

Optional<ReferrerPolicy::ReferrerPolicy> from_bindings_enum(Bindings::ReferrerPolicy referrer_policy)
{
    switch (referrer_policy) {
    case Bindings::ReferrerPolicy::Empty:
        return {};
    case Bindings::ReferrerPolicy::NoReferrer:
        return ReferrerPolicy::ReferrerPolicy::NoReferrer;
    case Bindings::ReferrerPolicy::NoReferrerWhenDowngrade:
        return ReferrerPolicy::ReferrerPolicy::NoReferrerWhenDowngrade;
    case Bindings::ReferrerPolicy::SameOrigin:
        return ReferrerPolicy::ReferrerPolicy::SameOrigin;
    case Bindings::ReferrerPolicy::Origin:
        return ReferrerPolicy::ReferrerPolicy::Origin;
    case Bindings::ReferrerPolicy::StrictOrigin:
        return ReferrerPolicy::ReferrerPolicy::StrictOrigin;
    case Bindings::ReferrerPolicy::OriginWhenCrossOrigin:
        return ReferrerPolicy::ReferrerPolicy::OriginWhenCrossOrigin;
    case Bindings::ReferrerPolicy::StrictOriginWhenCrossOrigin:
        return ReferrerPolicy::ReferrerPolicy::StrictOriginWhenCrossOrigin;
    case Bindings::ReferrerPolicy::UnsafeUrl:
        return ReferrerPolicy::ReferrerPolicy::UnsafeURL;
    default:
        VERIFY_NOT_REACHED();
    }
}

Infrastructure::Requesting::Mode from_bindings_enum(Bindings::RequestMode mode)
{
    switch (mode) {
    case Bindings::RequestMode::SameOrigin:
        return Infrastructure::Requesting::Mode::SameOrigin;
    case Bindings::RequestMode::Cors:
        return Infrastructure::Requesting::Mode::CORS;
    case Bindings::RequestMode::NoCors:
        return Infrastructure::Requesting::Mode::NoCORS;
    case Bindings::RequestMode::Navigate:
        return Infrastructure::Requesting::Mode::Navigate;
    default:
        VERIFY_NOT_REACHED();
    }
}

Infrastructure::Requesting::CredentialsMode from_bindings_enum(Bindings::RequestCredentials request_credentials)
{
    switch (request_credentials) {
    case Bindings::RequestCredentials::Omit:
        return Infrastructure::Requesting::CredentialsMode::Omit;
    case Bindings::RequestCredentials::SameOrigin:
        return Infrastructure::Requesting::CredentialsMode::SameOrigin;
    case Bindings::RequestCredentials::Include:
        return Infrastructure::Requesting::CredentialsMode::Include;
    default:
        VERIFY_NOT_REACHED();
    }
}

Infrastructure::Requesting::CacheMode from_bindings_enum(Bindings::RequestCache request_cache)
{
    switch (request_cache) {
    case Bindings::RequestCache::Default:
        return Infrastructure::Requesting::CacheMode::Default;
    case Bindings::RequestCache::NoStore:
        return Infrastructure::Requesting::CacheMode::NoStore;
    case Bindings::RequestCache::Reload:
        return Infrastructure::Requesting::CacheMode::Reload;
    case Bindings::RequestCache::NoCache:
        return Infrastructure::Requesting::CacheMode::NoCache;
    case Bindings::RequestCache::ForceCache:
        return Infrastructure::Requesting::CacheMode::ForceCache;
    case Bindings::RequestCache::OnlyIfCached:
        return Infrastructure::Requesting::CacheMode::OnlyIfCached;
    default:
        VERIFY_NOT_REACHED();
    }
}

Infrastructure::Requesting::RedirectMode from_bindings_enum(Bindings::RequestRedirect request_redirect)
{
    switch (request_redirect) {
    case Bindings::RequestRedirect::Follow:
        return Infrastructure::Requesting::RedirectMode::Follow;
    case Bindings::RequestRedirect::Error:
        return Infrastructure::Requesting::RedirectMode::Error;
    case Bindings::RequestRedirect::Manual:
        return Infrastructure::Requesting::RedirectMode::Manual;
    default:
        VERIFY_NOT_REACHED();
    }
}

Bindings::ReferrerPolicy to_bindings_enum(Optional<ReferrerPolicy::ReferrerPolicy> const& referrer_policy)
{
    if (!referrer_policy.has_value())
        return Bindings::ReferrerPolicy::Empty;
    switch (*referrer_policy) {
    case ReferrerPolicy::ReferrerPolicy::NoReferrer:
        return Bindings::ReferrerPolicy::NoReferrer;
    case ReferrerPolicy::ReferrerPolicy::NoReferrerWhenDowngrade:
        return Bindings::ReferrerPolicy::NoReferrerWhenDowngrade;
    case ReferrerPolicy::ReferrerPolicy::SameOrigin:
        return Bindings::ReferrerPolicy::SameOrigin;
    case ReferrerPolicy::ReferrerPolicy::Origin:
        return Bindings::ReferrerPolicy::Origin;
    case ReferrerPolicy::ReferrerPolicy::StrictOrigin:
        return Bindings::ReferrerPolicy::StrictOrigin;
    case ReferrerPolicy::ReferrerPolicy::OriginWhenCrossOrigin:
        return Bindings::ReferrerPolicy::OriginWhenCrossOrigin;
    case ReferrerPolicy::ReferrerPolicy::StrictOriginWhenCrossOrigin:
        return Bindings::ReferrerPolicy::StrictOriginWhenCrossOrigin;
    case ReferrerPolicy::ReferrerPolicy::UnsafeURL:
        return Bindings::ReferrerPolicy::UnsafeUrl;
    default:
        VERIFY_NOT_REACHED();
    }
}

Bindings::RequestDestination to_bindings_enum(Optional<Infrastructure::Requesting::Destination> const& destination)
{
    if (!destination.has_value())
        return Bindings::RequestDestination::Empty;
    switch (*destination) {
    case Infrastructure::Requesting::Destination::Audio:
        return Bindings::RequestDestination::Audio;
    case Infrastructure::Requesting::Destination::AudioWorklet:
        return Bindings::RequestDestination::Audioworklet;
    case Infrastructure::Requesting::Destination::Document:
        return Bindings::RequestDestination::Document;
    case Infrastructure::Requesting::Destination::Embed:
        return Bindings::RequestDestination::Embed;
    case Infrastructure::Requesting::Destination::Font:
        return Bindings::RequestDestination::Font;
    case Infrastructure::Requesting::Destination::Frame:
        return Bindings::RequestDestination::Frame;
    case Infrastructure::Requesting::Destination::IFrame:
        return Bindings::RequestDestination::Iframe;
    case Infrastructure::Requesting::Destination::Image:
        return Bindings::RequestDestination::Image;
    case Infrastructure::Requesting::Destination::Manifest:
        return Bindings::RequestDestination::Manifest;
    case Infrastructure::Requesting::Destination::Object:
        return Bindings::RequestDestination::Object;
    case Infrastructure::Requesting::Destination::PaintWorklet:
        return Bindings::RequestDestination::Paintworklet;
    case Infrastructure::Requesting::Destination::Report:
        return Bindings::RequestDestination::Report;
    case Infrastructure::Requesting::Destination::Script:
        return Bindings::RequestDestination::Script;
    case Infrastructure::Requesting::Destination::ServiceWorker:
        // NOTE: "serviceworker" is omitted from RequestDestination as it cannot be observed from JavaScript.
        //       Implementations will still need to support it as a destination.
        VERIFY_NOT_REACHED();
    case Infrastructure::Requesting::Destination::SharedWorker:
        return Bindings::RequestDestination::Sharedworker;
    case Infrastructure::Requesting::Destination::Style:
        return Bindings::RequestDestination::Style;
    case Infrastructure::Requesting::Destination::Track:
        return Bindings::RequestDestination::Track;
    case Infrastructure::Requesting::Destination::Video:
        return Bindings::RequestDestination::Video;
    case Infrastructure::Requesting::Destination::Worker:
        return Bindings::RequestDestination::Worker;
    case Infrastructure::Requesting::Destination::XSLT:
        return Bindings::RequestDestination::Xslt;
    default:
        VERIFY_NOT_REACHED();
    }
}

Bindings::RequestMode to_bindings_enum(Infrastructure::Requesting::Mode mode)
{
    switch (mode) {
    case Infrastructure::Requesting::Mode::SameOrigin:
        return Bindings::RequestMode::SameOrigin;
    case Infrastructure::Requesting::Mode::CORS:
        return Bindings::RequestMode::Cors;
    case Infrastructure::Requesting::Mode::NoCORS:
        return Bindings::RequestMode::NoCors;
    case Infrastructure::Requesting::Mode::Navigate:
        return Bindings::RequestMode::Navigate;
    case Infrastructure::Requesting::Mode::WebSocket:
        // NOTE: "websocket" is omitted from RequestMode as it cannot be used nor observed from JavaScript.
        VERIFY_NOT_REACHED();
    default:
        VERIFY_NOT_REACHED();
    }
}

Bindings::RequestCredentials to_bindings_enum(Infrastructure::Requesting::CredentialsMode credentials_mode)
{
    switch (credentials_mode) {
    case Infrastructure::Requesting::CredentialsMode::Omit:
        return Bindings::RequestCredentials::Omit;
    case Infrastructure::Requesting::CredentialsMode::SameOrigin:
        return Bindings::RequestCredentials::SameOrigin;
    case Infrastructure::Requesting::CredentialsMode::Include:
        return Bindings::RequestCredentials::Include;
    default:
        VERIFY_NOT_REACHED();
    }
}

Bindings::RequestCache to_bindings_enum(Infrastructure::Requesting::CacheMode cache_mode)
{
    switch (cache_mode) {
    case Infrastructure::Requesting::CacheMode::Default:
        return Bindings::RequestCache::Default;
    case Infrastructure::Requesting::CacheMode::NoStore:
        return Bindings::RequestCache::NoStore;
    case Infrastructure::Requesting::CacheMode::Reload:
        return Bindings::RequestCache::Reload;
    case Infrastructure::Requesting::CacheMode::NoCache:
        return Bindings::RequestCache::NoCache;
    case Infrastructure::Requesting::CacheMode::ForceCache:
        return Bindings::RequestCache::ForceCache;
    case Infrastructure::Requesting::CacheMode::OnlyIfCached:
        return Bindings::RequestCache::OnlyIfCached;
    default:
        VERIFY_NOT_REACHED();
    }
}

Bindings::RequestRedirect to_bindings_enum(Infrastructure::Requesting::RedirectMode redirect_mode)
{
    switch (redirect_mode) {
    case Infrastructure::Requesting::RedirectMode::Follow:
        return Bindings::RequestRedirect::Follow;
    case Infrastructure::Requesting::RedirectMode::Error:
        return Bindings::RequestRedirect::Error;
    case Infrastructure::Requesting::RedirectMode::Manual:
        return Bindings::RequestRedirect::Manual;
    default:
        VERIFY_NOT_REACHED();
    }
}

Bindings::ResponseType to_bindings_enum(Infrastructure::Response::Type type)
{
    switch (type) {
    case Infrastructure::Response::Type::Basic:
        return Bindings::ResponseType::Basic;
    case Infrastructure::Response::Type::CORS:
        return Bindings::ResponseType::Cors;
    case Infrastructure::Response::Type::Default:
        return Bindings::ResponseType::Default;
    case Infrastructure::Response::Type::Error:
        return Bindings::ResponseType::Error;
    case Infrastructure::Response::Type::Opaque:
        return Bindings::ResponseType::Opaque;
    case Infrastructure::Response::Type::OpaqueRedirect:
        return Bindings::ResponseType::Opaqueredirect;
    default:
        VERIFY_NOT_REACHED();
    }
}

}
