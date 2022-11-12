/*
 * Copyright (c) 2022, networkException <networkexception@serentiyos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "MainWidget.h"
#include "PostWidget.h"
#include <LibGUI/BoxLayout.h>
#include <LibGUI/ScrollableContainerWidget.h>

namespace Catdogstodon {

MainWidget::MainWidget()
{
    set_fill_with_background_color(true);

    set_layout<GUI::VerticalBoxLayout>();

    auto& timeline_container = add<GUI::ScrollableContainerWidget>();
    timeline_container.set_should_hide_unnecessary_scrollbars(true);

    m_timeline = Widget::construct();

    m_timeline->set_layout<GUI::VerticalBoxLayout>();
    m_timeline->layout()->set_margins(3);

    timeline_container.set_widget(m_timeline);
}

void MainWidget::add_post_widget(NonnullRefPtr<PostWidget> post_widget)
{
    m_timeline->add_child(*post_widget);
}

}
