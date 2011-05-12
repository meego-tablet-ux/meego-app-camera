/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Components 0.1
import MeeGo.App.Camera 0.1


ModalDialog {
    title: qsTr ("Memory Full")
    acceptButtonText: qsTr ("Open Photos")

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter

    content: Text{
        width : parent.width
        wrapMode : Text.Wrap
        text: qsTr ("There is no storage space left on the device. To take more photos or video, you may want to delete some existing content.")
    }

    Launcher {
        id: processLauncher;
    }

    onAccepted: {
        processLauncher.launch ("meego-qml-launcher --opengl --fullscreen --skip-app-switcher --app meego-app-photos");
    }
}
