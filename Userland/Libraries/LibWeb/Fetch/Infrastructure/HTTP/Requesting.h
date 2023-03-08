/*
 * Copyright (c) 2022-2023, Linus Groh <linusg@serenityos.org>
 * Copyright (c) 2023, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

namespace Web::Fetch::Infrastructure::Requesting {

enum class CacheMode {
    Default,
    NoStore,
    Reload,
    NoCache,
    ForceCache,
    OnlyIfCached,
};

enum class CredentialsMode {
    Omit,
    SameOrigin,
    Include,
};

enum class Destination {
    Audio,
    AudioWorklet,
    Document,
    Embed,
    Font,
    Frame,
    IFrame,
    Image,
    Manifest,
    Object,
    PaintWorklet,
    Report,
    Script,
    ServiceWorker,
    SharedWorker,
    Style,
    Track,
    Video,
    WebIdentity,
    Worker,
    XSLT,
};

enum class Initiator {
    Download,
    ImageSet,
    Manifest,
    Prefetch,
    Prerender,
    XSLT,
};

enum class InitiatorType {
    Audio,
    Beacon,
    Body,
    CSS,
    EarlyHint,
    Embed,
    Fetch,
    Font,
    Frame,
    IFrame,
    Image,
    IMG,
    Input,
    Link,
    Object,
    Ping,
    Script,
    Track,
    Video,
    XMLHttpRequest,
    Other,
};

enum class Mode {
    SameOrigin,
    CORS,
    NoCORS,
    Navigate,
    WebSocket,
};

enum class Origin {
    Client,
};

enum class ParserMetadata {
    ParserInserted,
    NotParserInserted,
};

enum class PolicyContainer {
    Client,
};

enum class Priority {
    High,
    Low,
    Auto
};

enum class RedirectMode {
    Follow,
    Error,
    Manual,
};

enum class Referrer {
    NoReferrer,
    Client,
};

enum class ResponseTainting {
    Basic,
    CORS,
    Opaque,
};

enum class ServiceWorkersMode {
    All,
    None,
};

enum class Window {
    NoWindow,
    Client,
};

}
