/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Labs.Components 0.1

ModalDialog {
    dialogTitle: qsTr ("Memory Full")
    leftButtonText: qsTr ("Open Photos")
    rightButtonText: qsTr ("Cancel")

    contentLoader.sourceComponent: DialogText {
        text: qsTr ("There is no storage space left on the device. To take more photos or video, you may want to delete some existing content.")
    }

    onDialogClicked: {
        if (button == 1) {
            console.log ("Open Photos");
            processLauncher.launch ("meego-qml-launcher --opengl --fullscreen --skip-app-switcher --app meego-app-photos");
        } else {
            console.log ("Cancelling");
        }
        dialogLoader.sourceComponent = undefined
    }
}
