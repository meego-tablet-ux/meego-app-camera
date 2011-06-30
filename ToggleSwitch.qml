/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.0

Item {
    id: toggle
    width: 170
    height: 55

    signal toggled

    property int rotationAngle: 0
    property bool rotationCounterClockwise: false
    property int rotationAnimationDuration: 0

    state: "photo"

    Row {
        //anchors.leftMargin: 9
        //anchors.topMargin: 4
        anchors.fill: parent

        CameraToggleButton {
            id: photo
            width: 85
            height: 55
            source: "image://themedimage/images/camera/camera_video_sm_up"
            activeSource: "image://themedimage/images/camera/camera_video_sm_dn"
            onClicked: {
                toggle.state = "photo";
                toggle.toggled ();
            }

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                transformOrigin: Item.Center
                rotation: rotationAngle
                Behavior on rotation { RotationAnimation { duration: rotationAnimationDuration; direction: rotationCounterClockwise ? RotationAnimation.Counterclockwise : RotationAnimation.Clockwise}}

                source: if (photo.state == "unchecked") {
                    return "image://themedimage/icons/toolbar/camera-photo";
                } else {
                    return "image://themedimage/icons/toolbar/camera-photo-active";
                }
            }
        }

        CameraToggleButton {
            id: video
            width: 85
            height: 55
            source: "image://themedimage/images/camera/camera_video_sm_up"
            activeSource: "image://themedimage/images/camera/camera_video_sm_dn"
            onClicked: {
                toggle.state = "video";
                toggle.toggled ();
            }

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                transformOrigin: Item.Center
                rotation: rotationAngle
                Behavior on rotation { RotationAnimation { duration: rotationAnimationDuration; direction: rotationCounterClockwise ? RotationAnimation.Counterclockwise : RotationAnimation.Clockwise}}

                source: if (video.state == "unchecked") {
                    return "image://themedimage/icons/toolbar/camera-video";
                } else {
                    return "image://themedimage/icons/toolbar/camera-video-active";
                }
            }
        }
    }

    states: [
        State {
            name: "photo"
            PropertyChanges {
                target: photo
                state: "checked"
            }
            PropertyChanges {
                target: video
                state: "unchecked"
            }
        },
        State {
            name: "video"
            PropertyChanges {
                target: photo
                state: "unchecked"
            }
            PropertyChanges {
                target: video
                state: "checked"
            }
        }
    ]
}
