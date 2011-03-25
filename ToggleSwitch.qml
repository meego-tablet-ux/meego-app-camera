/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7

Item {
    id: toggle
    width: 170
    height: 55

    signal toggled

    state: "photo"

    Row {
        //anchors.leftMargin: 9
        //anchors.topMargin: 4
        anchors.fill: parent

        CameraToggleButton {
            id: photo
            width: 85
            height: 55
//            activeSource: "image://theme/camera/camera_topbar_dn"
            onClicked: {
                toggle.state = "photo";
                toggle.toggled ();
            }

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                source: if (photo.state == "unchecked") {
                    return "image://theme/camera/camera_lens_sm_up";
                } else {
                    return "image://theme/camera/camera_lens_sm_dn";
                }
            }
        }

        CameraToggleButton {
            id: video
            width: 85
            height: 55
//            activeSource: "image://theme/camera/camera_topbar_dn"
            onClicked: {
                toggle.state = "video";
                toggle.toggled ();
            }

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                source: if (video.state == "unchecked") {
                    return "image://theme/camera/camera_video_sm_up";
                } else {
                    return "image://theme/camera/camera_video_sm_dn";
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