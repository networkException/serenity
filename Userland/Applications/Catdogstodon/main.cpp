/*
 * Copyright (c) 2022, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "MainWidget.h"
#include <LibCore/System.h>
#include <LibCore/Timer.h>
#include <LibGUI/Application.h>
#include <LibGUI/Window.h>
// Need to include this before RequestClientEndpoint.h as that one includes LibIPC/(De En)coder.h, which would bomb if included before this.
#include <LibCore/Proxy.h>
#include <LibImageDecoderClient/Client.h>
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
    TRY(Core::System::unveil("/tmp/session/%sid/portal/image", "rw"));
    TRY(Core::System::unveil(nullptr, nullptr));

    auto window = TRY(GUI::Window::try_create());
    // window->set_icon(app_icon.bitmap_for_size(16));
    window->set_title("Catdogstodon");
    window->resize(570, 500);

    auto main_widget = TRY(window->try_set_main_widget<Catdogstodon::MainWidget>());

    auto request_client = TRY(Protocol::RequestClient::try_create());
    auto image_decoder_client = TRY(ImageDecoderClient::Client::try_create());

    auto request = request_client->start_request("GET"sv, URL("https://chaos.social/api/v1/timelines/tag/serenityos"sv));

    request->on_buffered_request_finish = [&](auto, auto, auto, auto, StringView payload) {
        auto json = MUST(JsonValue::from_string(payload));

        // json.as_array().for_each([&](JsonValue const& post) {
            auto post = json.as_array().at(0);

            auto post_widget = Catdogstodon::PostWidget::construct();

            auto const& post_object = post.as_object();
            auto const& account_object = post_object.get("account"sv).as_object();

            post_widget->set_display_name(account_object.get("display_name"sv).as_string());
            post_widget->set_account_name(account_object.get("acct"sv).as_string());

            post_widget->set_content(post_object.get("content"sv).as_string());
            // post_widget->set_metadata(Core::DateTime::parse("%Y-%M-%DT%h:%m:%s.000Z"sv, post_object.get("created_at"sv).as_string()));

            main_widget->add_post_widget(post_widget);

            auto avatar_url = account_object.get("avatar"sv).as_string();

            Core::deferred_invoke([avatar_url, &request_client, &image_decoder_client, &post_widget]() {
            // auto timer = Core::Timer::create_single_shot(3, [avatar_url, &request_client, &image_decoder_client, &post_widget]() {
                auto profile_picture_request = request_client->start_request("GET"sv, URL(avatar_url));

                dbgln("Loading {}", avatar_url);

                profile_picture_request->on_buffered_request_finish = [&](auto, auto, auto, auto, ReadonlyBytes payload) {
                    dbgln("Finished loading {}", avatar_url);

                    post_widget->set_profile_picture(*image_decoder_client->decode_image(payload).release_value().frames.first().bitmap);
                };

                profile_picture_request->set_should_buffer_all_input(true);
            });
        // });
    };

    request->set_should_buffer_all_input(true);

    window->show();

    return app->exec();
}
