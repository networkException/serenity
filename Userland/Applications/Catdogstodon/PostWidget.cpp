/*
 * Copyright (c) 2022, networkException <networkexception@serentiyos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "PostWidget.h"
#include <Applications/Catdogstodon/PostGML.h>
#include <LibCore/DateTime.h>
#include <LibGUI/ImageWidget.h>
#include <LibGUI/Label.h>

namespace Catdogstodon {

PostWidget::PostWidget()
{
    load_from_gml(post_gml);
    set_frame_shadow(Gfx::FrameShadow::Raised);

    m_display_name = find_descendant_of_type_named<GUI::Label>("display_name");
    m_account_name = find_descendant_of_type_named<GUI::Label>("account_name");
    m_profile_picture = find_descendant_of_type_named<GUI::ImageWidget>("profile_picture");

    m_content = find_descendant_of_type_named<GUI::Label>("content");
    m_metadata = find_descendant_of_type_named<GUI::Label>("metadata");
}

void PostWidget::set_display_name(String const& display_name)
{
    m_display_name->set_text(display_name);
}

void PostWidget::set_account_name(String const& account_name)
{
    m_account_name->set_text(account_name);
}

void PostWidget::set_profile_picture(Gfx::Bitmap const& profile_picture)
{
    m_profile_picture->set_bitmap(&profile_picture);
}

void PostWidget::set_content(String const& content)
{
    m_content->set_text(content);
}

void PostWidget::set_metadata(Core::DateTime time, Post::Visibility visibility)
{
    m_content->set_text(String::formatted("{} ({})", time.to_string(), visibility == Post::Visibility::Public ? "Public" : "Private"));
}

}
