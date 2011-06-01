/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.0

Item {
    id: button

    width: img.width
    height: img.height

    property string source
    property string activeSource

    signal clicked (variant mouse)
    signal pressed (variant mouse)

    state: "up"

    property int rotationAngle: 0
    Image {
        id: img
        source: button.source
        transformOrigin: Item.Center
        rotation: rotationAngle
        Behavior on rotation { RotationAnimation { duration: 200; direction: RotationAnimation.Counterclockwise}}

    }

    MouseArea {
        id: mouse
        anchors.fill: parent

        onClicked: parent.clicked (mouse)
        onPressed: {
            parent.state = "down"
            parent.pressed(mouse)
        }
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
