/*
 * Copyright (c) 2023, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/URL.h>
#include <LibCore/ArgsParser.h>
#include <LibCore/File.h>
#include <LibMain/Main.h>

static int parse_url(StringView input) {
    auto url = AK::URL(input);
    if (!url.is_valid())
        return 1;

    dbgln("URL: {}", url);

    if (url.is_valid()) {
        dbgln("includes_credentials: {}", url.includes_credentials());

        if (url.includes_credentials()) {
            dbgln("username: {}", url.username());
            dbgln("password: {}", url.password());
        }

        dbgln("is_special: {}", url.is_special());

        dbgln("scheme: {}", url.scheme());
        dbgln("host: {}", url.host());
        dbgln("paths: {}", url.paths());
        dbgln("query: {}", url.query());
        dbgln("fragment: {}", url.fragment());
        dbgln("port: {}", url.port_or_default());
    }

    return 0;
}

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    StringView url_string;
    bool allow_invalid = false;

    Core::ArgsParser args_parser;
    args_parser.add_positional_argument(url_string, "URL string to be parsed", "url");
    args_parser.add_option(allow_invalid, "Allow invalid URLs", "allow-invalid", 'i');
    args_parser.parse(arguments);

    if (url_string == "-"sv) {
        auto standard_input = TRY(Core::File::standard_input());
        auto buffered_standard_input = TRY(Core::BufferedFile::create(move(standard_input)));

        Array<u8, 64 * 1024> input_buffer {};
        while (!buffered_standard_input->is_eof()) {
            auto line = TRY(buffered_standard_input->read_line(input_buffer));
            auto result = parse_url(line);

            if (allow_invalid && !result)
                continue;
            if (!result)
                return 1;
        }
    }

    auto result = parse_url(url_string);
    if (allow_invalid)
        return 0;
    return result;
}
