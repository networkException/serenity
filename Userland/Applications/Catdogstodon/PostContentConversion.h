/*
 * Copyright (c) 2022, kleines Filmröllchen <filmroellchen@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

// Tries to create Serenity GUI text object(s) from the given HTML coming from a post content.
#include <AK/Forward.h>
#include <LibGUI/Widget.h>

ErrorOr<Vector<NonnullRefPtr<GUI::Widget>>> try_dehtmlify_post_content(StringView html);
