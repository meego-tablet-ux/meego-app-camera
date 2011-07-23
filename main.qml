/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.0

import QtMultimediaKit 1.1
import MeeGo.Ux.Components.Common 0.1
import MeeGo.Ux.Kernel 0.1
import MeeGo.App.Camera 0.1
import MeeGo.Ux.Gestures 0.1

Window {
    id: window

    fullScreen: true
    fullContent: true

    Component.onCompleted: {
//        console.log("load MainPage");
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

            function cameraTakePhoto(){
                if (shutterLoader.sourceComponent == null)
                    shutterLoader.sourceComponent = shutterAnimation;
                shutterLoader.item.startClosingAnimation();

                camera.takePhoto();
            }

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

                onImageCapturedSig: {
                    shutterLoader.item.startOpeningAnimation();
                }

                rotateAngle: 0

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
                    if (loader.sourceComponent != null) {
                        loader.item.resetZoom ();
                    }
                }

                onMaxZoomChanged: {
                    if (loader.sourceComponent == null) {
                        if (camera.maxZoom <= 1.0)
                            return
                        loader.sourceComponent = zoomer
                    }
                    loader.item.visible = (camera.maxZoom > 1.0);
                }

                onFocusLocked: {
                    cameraTakePhoto()
                }

                Component.onCompleted: {
                    if (camera.maxZoom > 1.0)
                        loader.sourceComponent = zoomer
                }
            }

            Component {
                id: shutterAnimation

                ShutterAnimationComponent {
                    anchors.fill: parent
                    width: window.width
                    height: window.height
                }
            }

            Loader {
                id: shutterLoader
            }

            Rectangle {
                id: standbyCover

                width: parent.width
                height: parent.height + statusBar.height

                color: "black"
                visible: false

                GestureArea {
                    anchors.fill: parent
                    Tap {
                        onStarted: {
                            standbyCover.visible = false;
                            camera.leaveStandbyMode ();
                        }
                    }
                }
            }

            TopBar {
                id: topBar
                rotationAngle: componentsRotationAngle
                rotationCounterClockwise: isCounterClockwise
                rotationAnimationDuration: rotationAnimationSpeed
                capturedVideoPath: camera.capturedVideoLocation
                capturedPhotoPath: camera.imageLocation

            }

            Component {
                id: zoomer

                ZoomSlider {
                    x: 12
                    y: (window.height - height) / 2

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

                    onZoomLevelChanged: {
                        camera.zoom = zoomLevel;
                    }
                }

            }


            Loader {
                id: loader
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
                    if (!camera.ready)
                        return

                    if (!camera.canLockFocus)
                        cameraTakePhoto()
                    else
                        camera.startFocus()
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
                            PropertyChanges {
                                target: window
                                inhibitScreenSaver: true
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
                            PropertyChanges {
                                target: window
                                inhibitScreenSaver: false
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

            NoSpaceDialog {
                id: noSpaceDialogComponent
            }

        }
    }
}
