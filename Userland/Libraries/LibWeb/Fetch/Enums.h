/*
 * Copyright (c) 2022, Linus Groh <linusg@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Forward.h>
#include <LibWeb/Fetch/Infrastructure/HTTP/Requests.h>
#include <LibWeb/Fetch/Infrastructure/HTTP/Responses.h>
#include <LibWeb/Forward.h>

namespace Web::Fetch {

[[nodiscard]] Optional<ReferrerPolicy::ReferrerPolicy> from_bindings_enum(Bindings::ReferrerPolicy);
[[nodiscard]] Infrastructure::Requesting::Mode from_bindings_enum(Bindings::RequestMode);
[[nodiscard]] Infrastructure::Requesting::CredentialsMode from_bindings_enum(Bindings::RequestCredentials);
[[nodiscard]] Infrastructure::Requesting::CacheMode from_bindings_enum(Bindings::RequestCache);
[[nodiscard]] Infrastructure::Requesting::RedirectMode from_bindings_enum(Bindings::RequestRedirect);

[[nodiscard]] Bindings::ReferrerPolicy to_bindings_enum(Optional<ReferrerPolicy::ReferrerPolicy> const&);
[[nodiscard]] Bindings::RequestDestination to_bindings_enum(Optional<Infrastructure::Requesting::Destination> const&);
[[nodiscard]] Bindings::RequestMode to_bindings_enum(Infrastructure::Requesting::Mode);
[[nodiscard]] Bindings::RequestCredentials to_bindings_enum(Infrastructure::Requesting::CredentialsMode);
[[nodiscard]] Bindings::RequestCache to_bindings_enum(Infrastructure::Requesting::CacheMode);
[[nodiscard]] Bindings::RequestRedirect to_bindings_enum(Infrastructure::Requesting::RedirectMode);
[[nodiscard]] Bindings::ResponseType to_bindings_enum(Infrastructure::Response::Type);

}
