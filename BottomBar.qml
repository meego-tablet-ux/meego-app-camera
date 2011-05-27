/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Components 0.1

Image {
    id: bottomBarBackground

    property int rotationAngle: 0

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
                    enabled: false
                    //this should be an icon specific for disabled button
                    source: "image://themedimage/images/camera/camera_rotate_up"
                }
                PropertyChanges {
                    target: flashButton
                    enabled: false
                    //this should be an icon specific for disabled button
                    source: "image://themedimage/images/camera/camera_bottombar_up"
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
                    enabled: true
                    source: "image://themedimage/images/camera/camera_rotate_up"
                }
                PropertyChanges {
                    target: flashButton
                    enabled: true
                    source: "image://themedimage/images/camera/camera_bottombar_up"
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
        //source: "image://themedimage/images/camera/camera_rotate_up"
        activeSource: "image://themedimage/images/camera/camera_rotate_dn"

        transformOrigin: Item.Center
        rotation: rotationAngle
        Behavior on rotation { RotationAnimation { duration: 200}}

        visible: camera.cameraCount > 1
        onClicked: {
            camera.changeCamera ();
        }



    }

    FlashButton {
        id: flashButton
        x: parent.width - width
        //y: (parent.height - height) / 2
        anchors.verticalCenter: parent.verticalCenter
        value: camera.flashMode
        visible: camera.cameraHasFlash

        //source: "image://themedimage/images/camera/camera_bottombar_up"
        activeSource: "image://themedimage/images/camera/camera_bottombar_dn"

        onFlashMode: {
            camera.flashMode = flashValue;
        }
    }


    Theme {
        id: theme
    }

    Text {
        id: duration
        text: camera.durationString

        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: theme.fontFamily
        font.pixelSize: theme.fontPixelSizeLarge
        color: theme.fontColorNormal
    }
 }
