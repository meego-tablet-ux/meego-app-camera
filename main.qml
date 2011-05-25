/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.0

import QtMultimediaKit 1.1
import MeeGo.Components 0.1
import MeeGo.App.Camera 0.1

Window {
    id: window

    fullScreen: true
    fullContent: true

    Component.onCompleted: {
        console.log("load MainPage");
        switchBook(cameraContent);
    }

    Component {
        id: cameraContent

        AppPage {
            id: cameraPage
            anchors.fill: parent

            fullScreen: true

            Connections {
                target: window
                onIsActiveWindowChanged: {
                    if (isActiveWindow)
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

                anchors.fill: parent
                color: "black"
            }

            ViewFinder {
                id: camera

                anchors.fill: parent

                rotateAngle: {
                    switch (orientation) {
                    case 0:
                        return (camera.currentCamera == camera.cameraCount - 1) ? 90 : 270;
                    case 1:
                        return 0;
                    case 2:
                        return (camera.currentCamera == camera.cameraCount - 1) ? 270 : 90;
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
                    noSpaceDialogComponent.show()
                }

                onCameraChanged: {
                    zoomer.resetZoom ();
                }

                onMaxZoomChanged: {
                    zoomer.visible = (camera.maxZoom > 1.0);
                }
            }

            Rectangle {
                id: standbyCover

                width: parent.width
                height: parent.height + statusBar.height

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
                y: (parent.height - height) / 2
                visible: false

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

                y: parent.height - height
            }

            PushButton {
                id: photoButton
                x: parent.width - width
                anchors.verticalCenter: parent.verticalCenter

                visible: camera.state == "photo"

                source: "image://themedimage/images/camera/camera_takephoto_up"
                activeSource: "image://themedimage/images/camera/camera_takephoto_dn"

                onClicked: {
                    camera.takePhoto ();
                }
            }

            PushButton {
                id: recordButton
                x: parent.width - width
                anchors.verticalCenter: parent.verticalCenter

                visible: camera.state == "video"

                source: "image://themedimage/images/camera/camera_record"
                activeSource: source

                onClicked: {
                    if (camera.recording) {
                        camera.endRecording ();
                        source = "image://themedimage/images/camera/camera_record"
                    } else {
                        camera.startRecording ();
                        source = "image://themedimage/images/camera/camera_record_dn"
                    }
                }
            }
        }
    }

    Loader {
        id: dialogLoader
        width: parent.width
        height: parent.height + statusBar.height
    }

    NoSpaceDialog {
        id: noSpaceDialogComponent
    }
}
