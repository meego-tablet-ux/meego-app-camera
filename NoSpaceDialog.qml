/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.0
import MeeGo.Ux.Components 0.1
import MeeGo.App.Camera 0.1


ModalDialog {
    title: qsTr ("Memory Full")
    acceptButtonText: qsTr ("Open Photos")

    content: Text{
        anchors.margins: 12
        anchors.fill: parent
        wrapMode : Text.Wrap
        text: qsTr ("There is no storage space left on the device. To take more photos or video, you may want to delete some existing content.")
    }

    onAccepted: {
        qApp.launchDesktopByName ("/usr/share/applications/meego-app-photos.desktop");
    }
}
