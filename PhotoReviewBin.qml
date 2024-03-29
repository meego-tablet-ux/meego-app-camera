/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.0
import MeeGo.App.Camera 0.1
import MeeGo.Ux.Gestures 0.1

Item {
    id: bin

    width: parent.height
    height: parent.height
    state: "up"

    property alias source: img.source
    property alias imageRotation: img.rotation
    property string type: "photo"

    signal clicked

    RoundedImage {
        id: img
        x: 1
        y: 1
        //Keep it square
        width: 55
        height: 55
    }

    Image {
        id: frame
        anchors.fill: parent
    }

    Timer {
        id: animationTimer
        interval: 200
        repeat: false
        onTriggered:  bin.state = "up"
    }

    GestureArea {
        anchors.fill: parent
        Tap {
            onStarted: {
                bin.clicked()
                bin.state = "down"
                animationTimer.start();
            }
        }
    }

    states: [
        State {
            name: "up"
            PropertyChanges {
                target: frame
                source: "image://themedimage/images/camera/camera_thumb_up"
            }
        },
        State {
            name: "down"
            PropertyChanges {
                target: frame
                source: "image://themedimage/images/camera/camera_thumb_dn"
            }
        }
    ]
}
