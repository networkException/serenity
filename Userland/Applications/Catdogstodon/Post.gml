@GUI::Frame {
    layout: @GUI::VerticalBoxLayout {
        margins: [3]
    }
    max_height: "shrink"

    // Post bodys
    @GUI::Widget {
        layout: @GUI::VerticalBoxLayout {
            margins: [0, 5]
        }

        // Profile
        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {
                spacing: 10
            }

            @GUI::ImageWidget {
                name: "profile_picture"
                bitmap: "/res/icons/32x32/app-catdog.png"
                // bitmap: "/res/icons/32x32/app-gameoflife.png"
                should_stretch: true
                auto_resize: false
                fixed_width: 32
                fixed_height: 32
            }

            @GUI::Widget {
                layout: @GUI::VerticalBoxLayout {
                    spacing: 5
                }

                @GUI::Label {
                    name: "display_name"
                    text: "CatDog"
                    text_alignment: "BottomLeft"
                }

                @GUI::LinkLabel {
                    name: "account_name"
                    text: "@catdog@serenityos.org"
                    text_alignment: "TopLeft"
                }
            }
        }

        // Content
        @GUI::Widget {
            layout: @GUI::VerticalBoxLayout {}

            @GUI::Widget {
                name: "content"
                layout: @GUI::VerticalBoxLayout {}
                // FIXME: Change this to a "resize vertically if necessary" computed height once that is supported
                min_height: 60
            }

            @GUI::Label {
                name: "metadata"
                text: "2022-11-12, 13:42"
                text_alignment: "CenterLeft"
                foreground_color: "gray"
            }
        }
    }

    @GUI::HorizontalSeparator {}

    // Controls
    @GUI::Widget {
        layout: @GUI::HorizontalBoxLayout {
            margins: [2]
        }

        @GUI::Button {
            text: "Reply"
        }

        @GUI::Button {
            text: "Boost"
        }

        @GUI::Button {
            // icon: "/res/icons/16x16/continue.png"
            text: "Favorite"
        }

        @GUI::Button {
            // icon: "/res/icons/16x16/bookmark-contour.png"
            text: "Bookmark"
        }

        @GUI::Button {
            text: "Share"
        }
    }
}
