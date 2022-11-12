/*
 * Copyright (c) 2022, networkException <networkexception@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "PostWidget.h"
#include <LibGUI/Widget.h>

namespace Catdogstodon {

class MainWidget final : public GUI::Widget {
    C_OBJECT(MainWidget);

public:
    virtual ~MainWidget() override = default;

    void add_post_widget(NonnullRefPtr<PostWidget>);

private:
    MainWidget();

    RefPtr<GUI::Widget> m_timeline;
};

}
