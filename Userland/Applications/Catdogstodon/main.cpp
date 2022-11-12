/*
 * Copyright (c) 2022, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "MainWidget.h"
#include <LibCore/System.h>
#include <LibGUI/Application.h>
#include <LibGUI/Window.h>
#include <LibMain/Main.h>
#include <LibProtocol/Request.h>
#include <LibProtocol/RequestClient.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    TRY(Core::System::pledge("accept stdio recvfd sendfd rpath unix"));
    auto app = TRY(GUI::Application::try_create(arguments, Core::EventLoop::MakeInspectable::Yes));

    TRY(Core::System::unveil("/res", "r"));
    TRY(Core::System::unveil("/sys/kernel/processes", "r"));
    TRY(Core::System::unveil("/tmp/session/%sid/portal/request", "rw"));
    TRY(Core::System::unveil(nullptr, nullptr));

    auto window = TRY(GUI::Window::try_create());
    // window->set_icon(app_icon.bitmap_for_size(16));
    window->set_title("Catdogstodon");
    window->resize(570, 500);

    auto main_widget = TRY(window->try_set_main_widget<Catdogstodon::MainWidget>());
    auto request_client = TRY(Protocol::RequestClient::try_create());

    auto request = request_client->start_request("GET"sv, URL("https://chaos.social/api/v1/timelines/tag/serenityos"));

    request->on_buffered_request_finish = [&main_widget](auto, auto, auto, auto, StringView payload) {
        auto json = MUST(JsonValue::from_string(payload));

        json.as_array().for_each([&main_widget](JsonValue const& post) {
            auto post_widget = Catdogstodon::PostWidget::construct();

            auto const& post_object = post.as_object();
            auto const& account_object = post_object.get("account"sv).as_object();

            post_widget->set_display_name(account_object.get("display_name"sv).as_string());
            post_widget->set_account_name(account_object.get("acct"sv).as_string());

            post_widget->set_content(post_object.get("content"sv).as_string());
            // post_widget->set_metadata(Core::DateTime::parse("%Y-%M-%DT%h:%m:%s.000Z"sv, post_object.get("created_at"sv).as_string()));

            main_widget->add_post_widget(post_widget);
        });
    };

    request->set_should_buffer_all_input(true);

    window->show();

    return app->exec();
}
