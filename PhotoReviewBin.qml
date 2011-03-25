/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.App.Camera 0.1

Item {
    id: bin

    width: parent.height
    height: parent.height
    state: "up"

    property alias source: img.source
    property string type: "photo"

    signal clicked

    RoundedImage {
        id: img
        x: 1
        y: 1
        // Keep it square
        width: parent.height
        height: parent.height
    }

    Image {
        id: frame
        anchors.fill: parent
    }

    MouseArea {
        anchors.fill: parent

        onPressed: { bin.state = "down"; }
        onReleased: { bin.state = "up"; }
        onClicked: {
            bin.clicked ();
        }
    }

    states: [
        State {
            name: "up"
            PropertyChanges {
                target: frame
                source: "image://theme/camera/camera_thumb_up"
            }
        },
        State {
            name: "down"
            PropertyChanges {
                target: frame
                source: "image://theme/camera/camera_thumb_dn"
            }
        }
    ]
}
