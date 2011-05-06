/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7

import QtMultimediaKit 1.1
import MeeGo.Labs.Components 0.1
import MeeGo.App.Camera 0.1

Window {
    id: scene

    showtoolbar: false
    statusBar.visible: false

    fullscreen: true
    applicationPage: cameraContent

    Component {
        id: cameraContent

        ApplicationPage {
            id: cameraPage

            fullContent: true

            Connections {
                target: scene
                onForegroundChanged: {
                    if (scene.foreground)
                    {
                        standbyCover.visible = false;
                        camera.leaveStandbyMode ();
                    }
                    else
                    {
                        standbyCover.visible = true;
                        camera.enterStandbyMode ();
                    }
                }
            }

            Rectangle {
                id: background
                width: container.width
                height: container.height

                color: "black"
            }

            ViewFinder {
                id: camera

                width: container.width
                height: container.height

                rotateAngle: {
                    switch (orientation) {
                    case 0:
                        return 90;
                    case 1:
                        return 0;
                    case 2:
                        return 270;
                    case 3:
                        return 180;
                    }
                }
                zoom: zoomer.zoomLevel

                state: (camera.captureMode == 0? "photo" : "video");

                states: [
                    State {
                        name: "photo"
                        PropertyChanges {
                            target: camera
                            captureMode: 0
                        }
                    },
                    State {
                        name: "video"
                        PropertyChanges {
                            target: camera
                            captureMode: 1
                        }
                    }
                ]

                onImageCaptured: { console.log ("Photo taken"); }
                onNoSpaceOnDevice: {
                    // Stop the recording
                    if (camera.recording) {
                        camera.endRecording ();
                    }

                    showModalDialog (noSpaceDialogComponent);
                }

                onCameraChanged: {
                    zoomer.resetZoom ();
                }
            }

            Rectangle {
                id: standbyCover

                width: container.width
                height: container.height + statusBar.height

                color: "black"
                visible: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        standbyCover.visible = false;
                        camera.leaveStandbyMode ();
                    }
                }
            }

            TopBar {
                id: topBar
                x: 0
                y: 0
            }

            ZoomSlider {
                id: zoomer
                x: 12
                y: (container.height - height) / 2
                opacity: camera.canFocus ? 1.0: 0.0

                transitions: Transition {
                    NumberAnimation {
                        properties: opacity
                        duration: 100
                    }
                }

                state: (orientation == 0 || orientation == 2) ? "portrait" : "landscape"
            }

            BottomBar {
                id: bottomBar
                x: 0

                y: container.height - height
            }

            PushButton {
                id: photoButton
                x: container.width - width
                anchors.verticalCenter: parent.verticalCenter

                visible: camera.state == "photo"

                source: "image://theme/camera/camera_takephoto_up"
                activeSource: "image://theme/camera/camera_takephoto_dn"

                onClicked: {
                    camera.takePhoto ();
                }
            }

            PushButton {
                id: recordButton
                x: container.width - width
                anchors.verticalCenter: parent.verticalCenter

                visible: camera.state == "video"

                source: "image://theme/camera/camera_record"
                activeSource: source

                onClicked: {
                    if (camera.recording) {
                        camera.endRecording ();
                        source = "image://theme/camera/camera_record"
                    } else {
                        camera.startRecording ();
                        source = "image://theme/camera/camera_record_dn"
                    }
                }
            }
        }
    }

    Loader {
        id: dialogLoader
        width: container.width
        height: container.height + statusBar.height
    }

    Component {
        id: noSpaceDialogComponent
        NoSpaceDialog {}
    }
}
