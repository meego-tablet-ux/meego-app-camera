/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.0
import MeeGo.Ux.Components.Common 0.1

Image {
    id: bottomBarBackground

    property int rotationAngle: 0
    property bool rotationCounterClockwise: false
    property int rotationAnimationDuration: 0

    signal switchCamera();
    property bool cameraSwitchBtnEnabled: true

    width: parent.width
    source: {
        if (orientation == 1 || orientation == 3) {
            return "image://themedimage/images/camera/camera_bottombar_l";
        } else {
            return "image://themedimage/images/camera/camera_bottombar_p";
        }
    }

    Item {
        id: internal
        states: [
            State {
                name: "recording"
                when: camera.recording
                PropertyChanges {
                    target: changeCameras
                    visible: false
                }
                PropertyChanges {
                    target: flashButton
                    visible: false
                }
                PropertyChanges {
                    target: duration
                    visible: true
                }
            },
            State {
                name: "stopped"
                when: !camera.recording
                PropertyChanges {
                    target: changeCameras
                    visible: true
                }
                PropertyChanges {
                    target: flashButton
                    visible: true
                }
                PropertyChanges {
                    target: duration
                    visible: false
                }
            }
        ]
    }

    // Switch cameras
    PushButton {
        id: changeCameras
        anchors.verticalCenter: parent.verticalCenter

        iconSource: "image://themedimage/icons/actionbar/view-sync"
        activeIconSource: "image://themedimage/icons/actionbar/view-sync-active"
        backgroundSource: "image://themedimage/images/camera/camera_bottombar_up"
        activeBackgroundSource: "image://themedimage/images/camera/camera_bottombar_dn"

        rotationAngle: bottomBarBackground.rotationAngle
        rotationCounterClockwise: bottomBarBackground.rotationCounterClockwise
        rotationAnimationDuration: bottomBarBackground.rotationAnimationDuration

        enabled: cameraSwitchBtnEnabled

        visible: camera.cameraCount > 1
        onClicked: {
//            camera.changeCamera ();
            switchCamera();
        }
    }


    FlashButton {
        id: flashButton
        x: parent.width - width
        anchors.verticalCenter: parent.verticalCenter
        value: camera.flashMode
        visible: camera.cameraHasFlash

        activeBackgroundSource: "image://themedimage/images/camera/camera_bottombar_dn"

        flashMenuRotationAngle: bottomBarBackground.rotationAngle
        flashMenuRotationCounterClockwise: bottomBarBackground.rotationCounterClockwise
        flashMenuRotationAnimationDuration: bottomBarBackground.rotationAnimationDuration
    }



    Text {
        id: duration
        text: camera.durationString

        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
//        font.family: theme.fontFamily
//        font.pixelSize: 28 //theme.fontPixelSizeLarge

//        color: theme.fontColorHighlight
        color: "white"
        font { pixelSize: 28; weight: Font.Bold }
    }
 }
