/*
 * Copyright (c) 2022, kleines Filmröllchen <filmroellchen@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "PostContentConversion.h"
#include <LibGUI/Label.h>
#include <LibXML/Parser/Parser.h>

static void print(XML::Node const& node)
{
    node.content.visit(
        [&](XML::Node::Comment const& comment) {
            dbgln("comment: {}", comment.text);
        },
        [&](XML::Node::Text const& text) {
            dbgln("text: {}", text.builder.to_string());
        },
        [&](XML::Node::Element const& element) {
            dbgln("start element: {}", element.name);
            for (auto const& attribute : element.attributes) {
                dbgln("    > {} = {}", attribute.key, attribute.value);
            }
            for (auto const& child : element.children)
                print(child);
            dbgln("end element: {}", element.name);
        });
}

static void extract_text_internal(XML::Node::Element const& node, StringBuilder& builder)
{
    for (auto const& child : node.children) {
        child.content.visit(
            [&](XML::Node::Comment const&) {
                // ignore
            },
            [&](XML::Node::Text const& text) {
                builder.append(text.builder.to_string());
            },
            [&](XML::Node::Element const& element) {
                extract_text_internal(element, builder);
            });
    }
}

static String extract_text(XML::Node::Element const& node)
{
    StringBuilder builder;
    extract_text_internal(node, builder);
    return builder.to_string();
}

ErrorOr<Vector<NonnullRefPtr<GUI::Widget>>> try_dehtmlify_post_content(StringView html)
{
    // We intentionally don't use the LibWeb HTML parser, as that does way too many things for our simple use case.
    // Just hope that the server sends us XML-valid HTML.
    XML::Parser parser { html };
    auto result = parser.parse();
    if (result.is_error()) {
        dbgln("Post HTML malformed: {} (offset {})", result.error().error, result.error().offset);
        return Error::from_string_view("Post HTML malformed"sv);
    }

    Vector<NonnullRefPtr<GUI::Widget>> widgets;

    auto const document = result.release_value();
    print(document.root());
    // FIXME: bad assumption?
    auto const& containing_paragraph = document.root().content.get<XML::Node::Element>();
    // FIXME: Do something better than just extracting the text.
    widgets.append(GUI::Label::construct(extract_text(containing_paragraph)));

    return widgets;
}
