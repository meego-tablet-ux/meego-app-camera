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
            console.debug ("landscape...");
            return "image://theme/titlebar_l";
        } else {
            console.debug ("portrait...");
            return "image://theme/titleBar_p";
        }
    }

    // We can't use anchors for layout because anchors don't follow
    // transformations like rotation.
    // http://bugreports.qt.nokia.com/browse/QTBUG-10644
/*
    PushButton {
        anchors.verticalCenter: parent.verticalCenter
        x: 40
        width: 60
        height: 60
        id: quit
        source: "image://theme/icn_home_up"
        activeSource: "image://theme/icn_home_dn"

        onClicked: Qt.quit ();
    }
*/
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
