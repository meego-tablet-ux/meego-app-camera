/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7

Image {
    id: bottomBarBackground

    width: parent.width
    source: {
        if (orientation == 1 || orientation == 3) {
            return "image://theme/camera/camera_bottombar_l";
        } else {
            return "image://theme/camera/camera_bottombar_p";
        }
    }

    // Switch cameras
    PushButton {
        id: changeCameras
        anchors.verticalCenter: parent.verticalCenter
        source: "image://theme/camera/camera_rotate_up"
        activeSource: "image://theme/camera/camera_rotate_dn"

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

        source: "image://theme/camera/camera_bottombar_up"
        activeSource: "image://theme/camera/camera_bottombar_dn"

        onFlashMode: {
            camera.flashMode = flashValue;
        }
    }
 }
