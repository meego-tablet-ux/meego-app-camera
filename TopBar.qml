/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.App.Camera 0.1


Column {
    id: container
    width: parent.width
    height: topBarBackground.height + topBarShadow.height
    //visible: !camera.recording
    anchors.left: parent.left
    anchors.right: parent.right

    property int rotationAngle: 0

    Image {
        id: topBarBackground

        width: parent.width

        source: {
            if (orientation == 1 || orientation == 3) {
                return "image://themedimage/images/titlebar_l";
            } else {
                return "image://themedimage/images/titleBar_p";
            }
        }

        ToggleSwitch {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            state: camera.state
            onToggled: {
                camera.state = state;
            }

            rotationAngle: container.rotationAngle


        }

        PhotoReviewBin {
            id: reviewBin
            x: parent.width - width
            anchors.verticalCenter: parent.verticalCenter
            source: camera.imageLocation
            type: camera.state

            transformOrigin: Item.Center
            rotation: rotationAngle
            Behavior on rotation { RotationAnimation { duration: 200}}

            Launcher {
                id: processLauncher;
            }

            onClicked: {
                if (reviewBin.type == "photo") {
                    processLauncher.launch ("meego-qml-launcher --opengl --fullscreen --skip-app-switcher --app meego-app-photos");
                } else {
                    processLauncher.launch ("meego-qml-launcher --opengl --fullscreen --skip-app-switcher --app meego-app-video");
                }
            }
        }
    }

    Image {
        id: topBarShadow
        width: parent.width
        source: "image://themedimage/images/toolbar-shadow"
    }

    Item {
        id: internal
        states: [
            State {
                name: "topbarvisible"
                when: !camera.recording
                AnchorChanges {
                    target: container
                    anchors.top: container.parent.top
                    anchors.bottom: undefined
                }
            },
            State {
                name: "bottombarhidden"
                when: camera.recording
                AnchorChanges {
                    target: container
                    anchors.top: undefined
                    anchors.bottom: container.parent.top
                }
            }
        ]

        transitions: [
            Transition {
                from: "topbarvisible"
                to: "bottombarhidden"
                AnchorAnimation { duration: 300}
            },
            Transition {
                from: "bottombarhidden"
                to: "topbarvisible"
                AnchorAnimation { duration: 300}
            }
        ]
    }
}
