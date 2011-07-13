/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.0
import MeeGo.Ux.Gestures 0.1

Item {
    id: button

    property string source
    property string activeSource

    property bool pressed: false

    signal clicked

    state: "unchecked"

    Image {
        id: img
        anchors.fill: parent
    }

    Timer {
        id: animationTimer
        interval: 200
        repeat: false
        onTriggered:  button.pressed = false
    }

    GestureArea {
        anchors.fill: parent
        Tap {
            onStarted: {
                button.clicked()
                button.pressed = true
                animationTimer.start();
            }
        }
    }



//    MouseArea {
//        id: mouse
//        anchors.fill: parent
//        onClicked: parent.clicked ()
//        onPressed: parent.pressed = true;
//        onReleased: parent.pressed = false;
//    }

    states: [
        State {
            name: "unchecked"
            PropertyChanges {
                target: img
                source: button.source
            }
        },
        State {
            name: "checked"
            PropertyChanges {
                target: img
                source: button.activeSource
            }
        }
    ]
}
