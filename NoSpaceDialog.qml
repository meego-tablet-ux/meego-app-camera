/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Components 0.1
import MeeGo.Labs.Components 0.1

ModalDialog {
    title: qsTr ("Memory Full")
    acceptButtonText: qsTr ("Open Photos")

    content: DialogText {
        text: qsTr ("There is no storage space left on the device. To take more photos or video, you may want to delete some existing content.")
    }

    onAccepted: {
        qApp.launchDesktopByName ("/usr/share/meego-ux-appgrid/applications/meego-app-photos.desktop");
    }
}
