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

    // auto request = request_client->start_request("GET"sv, URL("https://chaos.social/api/v1/timelines/tag/serenityos"sv));

    // request->on_buffered_request_finish = [&](auto, auto, auto, auto, StringView payload) {

    auto payload = "[{\"id\":\"109335814947753476\",\"created_at\":\"2022-11-13T09:35:37.270Z\",\"in_reply_to_id\":null,\"in_reply_to_account_id\":null,\"sensitive\":false,\"spoiler_text\":\"\",\"visibility\":\"public\",\"language\":\"de\",\"uri\":\"https://chaos.social/users/filmroellchen/statuses/109335814947753476\",\"url\":\"https://chaos.social/@filmroellchen/109335814947753476\",\"replies_count\":1,\"reblogs_count\":0,\"favourites_count\":2,\"edited_at\":null,\"content\":\"<p><span class=\\\"h-card\\\"><a href=\\\"https://chaos.social/@leah\\\" class=\\\"u-url mention\\\">@<span>leah</span></a></span> <span class=\\\"h-card\\\"><a href=\\\"https://chaos.social/@rixx\\\" class=\\\"u-url mention\\\">@<span>rixx</span></a></span> Im Namen aller <a href=\\\"https://chaos.social/tags/SerenityOS\\\" class=\\\"mention hashtag\\\" rel=\\\"tag\\\">#<span>SerenityOS</span></a>-Entwickler entschuldige ich mich jetzt schon für den fragwürdigen TCP/HTTP-Müll, der in nächster Zeit Richtung chaos.social APIs fliegt. Wir schreiben einen Mastodon-Desktopclient (bzw. wurde <span class=\\\"h-card\\\"><a href=\\\"https://chaos.social/@networkexception\\\" class=\\\"u-url mention\\\">@<span>networkexception</span></a></span> dazu von mir aus Versehen überredet), der auf unserer eigenen HTTP-Bibliothek basiert, die wiederum mithilfe unseres eigenen Kernel-Networkstacks und unserer eigenen TLS-Kryptographie kommunizieren muss. </p>\",\"reblog\":null,\"application\":{\"name\":\"Web\",\"website\":null},\"account\":{\"id\":\"108409340361353773\",\"username\":\"filmroellchen\",\"acct\":\"filmroellchen\",\"display_name\":\"kleines Filmröllchen\",\"locked\":false,\"bot\":false,\"discoverable\":true,\"group\":false,\"created_at\":\"2022-06-02T00:00:00.000Z\",\"note\":\"<p>he/him :demiboy_flag: demiboy :nonbinary_flag: non-binary :blob_rainbowheart: , Computer Science Student, Programmer (operating systems, programming languages, signal processing), Weeb, Nerd, YouTuber, <a href=\\\"https://chaos.social/tags/AleaAquarius\\\" class=\\\"mention hashtag\\\" rel=\\\"tag\\\">#<span>AleaAquarius</span></a> Fandom Admin, <a href=\\\"https://chaos.social/tags/SerenityOS\\\" class=\\\"mention hashtag\\\" rel=\\\"tag\\\">#<span>SerenityOS</span></a> contributor, Wannabe-Author/Worldbuilder, Musician</p><p>Your non-generic local genderqueer tech nerd.</p><p>Feel free to follow/boost/hate/respond with cat pictures. Especially the latter.</p>\",\"url\":\"https://chaos.social/@filmroellchen\",\"avatar\":\"https://assets.chaos.social/accounts/avatars/108/409/340/361/353/773/original/c84caca6c6b74b43.png\",\"avatar_static\":\"https://assets.chaos.social/accounts/avatars/108/409/340/361/353/773/original/c84caca6c6b74b43.png\",\"header\":\"https://assets.chaos.social/accounts/headers/108/409/340/361/353/773/original/09a4c073d34e2a5a.png\",\"header_static\":\"https://assets.chaos.social/accounts/headers/108/409/340/361/353/773/original/09a4c073d34e2a5a.png\",\"followers_count\":63,\"following_count\":53,\"statuses_count\":457,\"last_status_at\":\"2022-11-13\",\"emojis\":[{\"shortcode\":\"demiboy_flag\",\"url\":\"https://assets.chaos.social/custom_emojis/images/000/014/063/original/11b9f12f65778013.png\",\"static_url\":\"https://assets.chaos.social/custom_emojis/images/000/014/063/static/11b9f12f65778013.png\",\"visible_in_picker\":true},{\"shortcode\":\"nonbinary_flag\",\"url\":\"https://assets.chaos.social/custom_emojis/images/000/014/059/original/9d5f4db6c85f5b1d.png\",\"static_url\":\"https://assets.chaos.social/custom_emojis/images/000/014/059/static/9d5f4db6c85f5b1d.png\",\"visible_in_picker\":true},{\"shortcode\":\"blob_rainbowheart\",\"url\":\"https://assets.chaos.social/custom_emojis/images/000/224/933/original/b77c378c9f6f1361.png\",\"static_url\":\"https://assets.chaos.social/custom_emojis/images/000/224/933/static/b77c378c9f6f1361.png\",\"visible_in_picker\":true}],\"fields\":[{\"name\":\"YouTube\",\"value\":\"<a href=\\\"https://www.youtube.com/c/kleinesfilmroellchen\\\" target=\\\"_blank\\\" rel=\\\"nofollow noopener noreferrer me\\\"><span class=\\\"invisible\\\">https://www.</span><span class=\\\"ellipsis\\\">youtube.com/c/kleinesfilmroell</span><span class=\\\"invisible\\\">chen</span></a>\",\"verified_at\":null},{\"name\":\"Website\",\"value\":\"<a href=\\\"https://klfr.spdns.de\\\" target=\\\"_blank\\\" rel=\\\"nofollow noopener noreferrer me\\\"><span class=\\\"invisible\\\">https://</span><span class=\\\"\\\">klfr.spdns.de</span><span class=\\\"invisible\\\"></span></a>\",\"verified_at\":null},{\"name\":\"Twitter\",\"value\":\"<a href=\\\"https://twitter.com/filmroellchen\\\" target=\\\"_blank\\\" rel=\\\"nofollow noopener noreferrer me\\\"><span class=\\\"invisible\\\">https://</span><span class=\\\"\\\">twitter.com/filmroellchen</span><span class=\\\"invisible\\\"></span></a>\",\"verified_at\":null},{\"name\":\"Pronouns\",\"value\":\"<a href=\\\"https://en.pronouns.page/@Filmroellchen\\\" target=\\\"_blank\\\" rel=\\\"nofollow noopener noreferrer me\\\"><span class=\\\"invisible\\\">https://</span><span class=\\\"ellipsis\\\">en.pronouns.page/@Filmroellche</span><span class=\\\"invisible\\\">n</span></a>\",\"verified_at\":null}]},\"media_attachments\":[],\"mentions\":[{\"id\":\"1\",\"username\":\"leah\",\"url\":\"https://chaos.social/@leah\",\"acct\":\"leah\"},{\"id\":\"2\",\"username\":\"rixx\",\"url\":\"https://chaos.social/@rixx\",\"acct\":\"rixx\"},{\"id\":\"243427\",\"username\":\"networkexception\",\"url\":\"https://chaos.social/@networkexception\",\"acct\":\"networkexception\"}],\"tags\":[{\"name\":\"serenityos\",\"url\":\"https://chaos.social/tags/serenityos\"}],\"emojis\":[],\"card\":null,\"poll\":null}]"sv;

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

            auto profile_picture_request = request_client->start_request("GET"sv, URL(avatar_url));

            dbgln("Loading {}", avatar_url);

            profile_picture_request->on_buffered_request_finish = [&](auto, auto, auto, auto, ReadonlyBytes payload) {
                dbgln("Finished loading {}", avatar_url);

                post_widget->set_profile_picture(*image_decoder_client->decode_image(payload).release_value().frames.first().bitmap);
            };

            profile_picture_request->set_should_buffer_all_input(true);
        // });
    //};

    //request->set_should_buffer_all_input(true);

    window->show();

    return app->exec();
}
