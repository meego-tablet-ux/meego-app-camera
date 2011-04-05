/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7

Image {
    id: topBarBackground

    width: parent.width

    source: {
        if (orientation == 1 || orientation == 3) {
            return "image://theme/titlebar_l";
        } else {
            return "image://theme/titleBar_p";
        }
    }

    ToggleSwitch {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        state: camera.state
        onToggled: {
            camera.state = state;
        }
    }

    PhotoReviewBin {
        id: reviewBin
        x: parent.width - width
        anchors.verticalCenter: parent.verticalCenter
        source: camera.imageLocation

        onClicked: {
            if (reviewBin.type == "photo") {
                processLauncher.launch ("meego-qml-launcher --opengl --fullscreen --skip-app-switcher --app meego-app-photos");
            } else {
                processLauncher.launch ("meego-qml-launcher --opengl --fullscreen --skip-app-switcher --app meego-app-video");
            }
        }
    }
}
