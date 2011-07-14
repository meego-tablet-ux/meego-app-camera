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

    property string source
    property string activeSource

    property int rotationAngle: 0
    property bool rotationCounterClockwise: false
    property int rotationAnimationDuration: 0

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

        transformOrigin: Item.Center
        rotation: rotationAngle
        Behavior on rotation { RotationAnimation { duration: rotationAnimationDuration; direction: rotationCounterClockwise ? RotationAnimation.Counterclockwise : RotationAnimation.Clockwise}}
    }

    Image {
        id: overlay

        visible: false
        source: button.activeSource

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        transformOrigin: Item.Center
        rotation: rotationAngle
        Behavior on rotation { RotationAnimation { duration: rotationAnimationDuration; direction: rotationCounterClockwise ? RotationAnimation.Counterclockwise : RotationAnimation.Clockwise}}
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

//    RepeatButton {
//        id: up

//        anchors.horizontalCenter: parent.horizontalCenter
//        y: (trough.y / 2) - (height / 2)

//        source: "image://themedimage/images/camera/camera_icn_add_up"
//        activeSource: "image://themedimage/images/camera/camera_icn_add_dn"

//        onClicked: {
//            moveScrub (scrub.y - stepSize);
//        }

//        rotationAngle: slider.rotationAngle
//        rotationCounterClockwise: slider.rotationCounterClockwise
//        rotationAnimationDuration: slider.rotationAnimationDuration
//    }

