/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7

Item {
    id: button

    width: img.width
    height: img.height

    property string source
    property string activeSource

    signal clicked (variant mouse)

    state: "up"

    Image {
        id: img
        source: button.source
    }

    MouseArea {
        id: mouse
        anchors.fill: parent

        onClicked: parent.clicked (mouse)
        onPressed: parent.state = "down"
        onReleased: parent.state = "up"
    }

    states: [
        State {
            name: "up"
            PropertyChanges {
                target: img
                source: button.source
            }
        },
        State {
            name: "down"
            PropertyChanges {
                target: img
                source: button.activeSource
            }
        }
    ]
}