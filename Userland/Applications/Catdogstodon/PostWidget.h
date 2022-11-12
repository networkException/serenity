/*
 * Copyright (c) 2022, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Post.h"
#include <LibGUI/Frame.h>

namespace Catdogstodon {

class PostWidget final : public GUI::Frame {
    C_OBJECT(PostWidget);

public:
    virtual ~PostWidget() override = default;

    void set_display_name(String const&);
    void set_account_name(String const&);
    void set_profile_picture(Gfx::Bitmap const&);

    void set_content(String const&);
    void set_metadata(Core::DateTime, Post::Visibility);

private:
    PostWidget();

    RefPtr<GUI::Label> m_display_name;
    RefPtr<GUI::Label> m_account_name;
    RefPtr<GUI::ImageWidget> m_profile_picture;

    RefPtr<GUI::Label> m_content;
    RefPtr<GUI::Label> m_metadata;
};

}
