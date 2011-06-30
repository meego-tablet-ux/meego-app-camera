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
    property bool rotationCounterClockwise: false
    property int rotationAnimationDuration: 0
    property string capturedPhotoPath
    property string capturedVideoPath : "/home/meego/Videos/sintel_trailer-480p.ogv"

    function calculatePixmapRotation(_orientation){

        var angle;
        switch (_orientation) {
            case 0:
                angle = 270; break;
            case 1:
                angle = 0; break;
            case 2:
                angle = 90; break;
            case 3:
                angle = 180; break;
        }
        return angle
    }

    onCapturedPhotoPathChanged: {
        reviewBin.imageRotation = calculatePixmapRotation(camera.lastPhotoOrientation);
    }

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
            rotationCounterClockwise: container.rotationCounterClockwise
            rotationAnimationDuration: container.rotationAnimationDuration
        }

        PhotoReviewBin {
            id: reviewBin
            x: parent.width - width
            anchors.verticalCenter: parent.verticalCenter
            source: capturedPhotoPath
            type: camera.state

            transformOrigin: Item.Center
            rotation: rotationAngle
            Behavior on rotation { RotationAnimation { duration: rotationAnimationDuration; direction: rotationCounterClockwise ? RotationAnimation.Counterclockwise : RotationAnimation.Clockwise}}

            onClicked: {
                if (reviewBin.type == "photo") {
                    qApp.launchDesktopByName("/usr/share/applications/meego-app-photos.desktop", "showPhoto", capturedPhotoPath);
                } else {
                    qApp.launchDesktopByName("/usr/share/applications/meego-app-video.desktop", "playVideo", capturedVideoPath);
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
