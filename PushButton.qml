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

    width: backgroundImage.width
    height: backgroundImage.height

    property string iconSource: ""
    property string activeIconSource: ""
    property string backgroundSource: ""
    property string activeBackgroundSource: ""


    signal clicked (/*variant mouse*/)
    signal pressed (/*variant mouse*/)

    state: "up"

    property int rotationAngle: 0
    property bool rotationCounterClockwise: false
    property int  rotationAnimationDuration: 0
    property alias iconScale: iconImage.scale
    Image {
        id: backgroundImage
    }

    Image {
        id: iconImage

        anchors.centerIn: parent

        smooth: true
        transformOrigin: Item.Center
        rotation: rotationAngle
        Behavior on rotation { RotationAnimation { duration: rotationAnimationDuration; direction: rotationCounterClockwise ? RotationAnimation.Counterclockwise : RotationAnimation.Clockwise}}
    }

    Timer {
        id: animationTimer
        interval: 200
        repeat: false
        onTriggered:  button.state = "up"
    }

    GestureArea {
        anchors.fill: parent
        Tap {
            onStarted: {
                button.pressed()
                button.clicked()
                button.state = "down"
                animationTimer.start();
            }
        }
    }


    states: [
        State {
            name: "up"
            PropertyChanges {
                target: backgroundImage
                source: backgroundSource
            }
            PropertyChanges {
                target: iconImage
                source: iconSource
            }
        },
        State {
            name: "down"
            PropertyChanges {
                target: backgroundImage
                source: activeBackgroundSource
            }
            PropertyChanges {
                target: iconImage
                source: activeIconSource
            }
        }
    ]
}
