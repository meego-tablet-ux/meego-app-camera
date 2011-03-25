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

    property string source
    property string activeSource

    width: img.width
    height: img.height

    signal clicked

    Timer {
        id: repeatTimer
        interval: 150
        triggeredOnStart: true
        repeat: true
        onTriggered: {
            button.clicked ();
        }
    }

    Image {
        id: img

        source: button.source

        visible: true
    }

    Image {
        id: overlay

        visible: false
        source: button.activeSource

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        onPressed: {
            parent.state = "down"
            repeatTimer.start ()
        }
        onReleased: {
            parent.state = ""
            repeatTimer.stop ()
        }
    }

    states: [
        State {
            name: "down"
            PropertyChanges {
                target: overlay
                visible: true
            }
            PropertyChanges {
                target: img
                visible: false
            }
        }
    ]
}