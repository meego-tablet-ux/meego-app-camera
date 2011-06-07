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

    orientationLock: 1
    property int componentsRotationAngle: 0
    property bool isCounterClockwise: false
    property bool isSensorLandscape: (sensorOrientation == 1 || sensorOrientation == 3) ? true : false
    property int rotationAnimationSpeed: 200

    property bool isInRecordingMode

    onSensorOrientationChanged: calculateRotation()
    onIsInRecordingModeChanged: calculateRotation()

    function calculateRotation(){
            //rotating this element might not be required, do nothing when in video record mode
            var angle;
            switch (sensorOrientation) {
                case 0:
                    angle = 90; break;
                case 1:
                    angle = 0; break;
                case 2:
                    angle = 270; break;
                case 3:
                    angle = 180; break;
        }
            if(isInRecordingMode)  angle = 0;
            if((angle > componentsRotationAngle && !(angle == 270 && componentsRotationAngle == 0)) || (componentsRotationAngle == 270 && angle == 0) )
                isCounterClockwise = false;
            else
                isCounterClockwise = true;
            componentsRotationAngle = angle;
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

                currentOrientation: sensorOrientation

                rotateAngle: 0
                zoom: zoomer.zoomLevel

                state: (camera.captureMode == 0? "photo" : "video");

                states: [
                    State {
                        name: "photo"
                        PropertyChanges {
                            target: camera
                            captureMode: 0
                        }
                        PropertyChanges {
                            target: window
                            isInRecordingMode: false
                        }
                    },
                    State {
                        name: "video"
                        PropertyChanges {
                            target: camera
                            captureMode: 1
                        }
                        PropertyChanges {
                            target: window
                            isInRecordingMode: true
                        }
                    }
                ]

                //onImageCaptured: { console.log ("Photo taken"); }
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

            ShutterAnimationComponent {
                id: shutterAnimation
                anchors.fill: parent
                width: parent.width
                height: parent.height
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
                rotationAngle: componentsRotationAngle
                rotationCounterClockwise: isCounterClockwise
                rotationAnimationDuration: rotationAnimationSpeed

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

                rotationAngle: componentsRotationAngle
                rotationCounterClockwise: isCounterClockwise
                rotationAnimationDuration: rotationAnimationSpeed
            }

            BottomBar {
                id: bottomBar
                x: 0
                y: parent.height - height
                rotationAngle: componentsRotationAngle
                rotationCounterClockwise: isCounterClockwise
                rotationAnimationDuration: window.rotationAnimationSpeed
            }

            PushButton {
                id: photoButton
                x: parent.width - width
                anchors.verticalCenter: parent.verticalCenter

                rotationAngle: componentsRotationAngle
                rotationCounterClockwise: isCounterClockwise
                rotationAnimationDuration: rotationAnimationSpeed
                visible: camera.state == "photo"

                iconSource: "image://themedimage/icons/toolbar/camera-photo"
                activeIconSource: "image://themedimage/icons/toolbar/camera-photo-active"
                iconScale: 1.5

                backgroundSource: "image://themedimage/images/camera/camera_record"
                activeBackgroundSource: "image://themedimage/images/camera/camera_takephoto_dn"

                onPressed: {
                    shutterAnimation.start();
                    camera.takePhoto();
                }
            }

            PushButton {
                id: recordButton
                x: parent.width - width
                anchors.verticalCenter: parent.verticalCenter

                visible: camera.state == "video"

                backgroundSource: "image://themedimage/images/camera/camera_record"
                activeBackgroundSource: "image://themedimage/images/camera/camera_takephoto_dn"

                Item {
                    id: recordingIconState
                    states: [
                        State {
                            name: "recording"
                            when: camera.recording
                            PropertyChanges {
                                target: recordButton
                                iconSource: "image://themedimage/images/camera/camera-record-icn-dn"
                                activeIconSource: "image://themedimage/images/camera/camera-record-icn-dn"
                            }
                        },
                        State {
                            name: "stopped"
                            when: !camera.recording
                            PropertyChanges {
                                target: recordButton
                                iconSource: "image://themedimage/images/camera/camera-record-icn"
                                activeIconSource: "image://themedimage/images/camera/camera-record-icn-dn"
                            }
                        }
                    ]
                }

                onClicked: {
                    if (camera.recording) {
                        camera.endRecording ();
                    } else {
                        camera.startRecording ();
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
