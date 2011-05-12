/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Components 0.1

PushButton {
    id: button

    width: 116

    property int value : 1

    signal flashMode (int flashValue);

    Row {
        id: row

        x: 10
        width: parent.width - 20
        height: parent.height
        spacing: 3

        Image {
            id: flashImage

            anchors.verticalCenter: parent.verticalCenter

            source: "image://themedimage/images/camera/camera_icn_flash_dn";
        }

        Text {
            width: row.width - flashImage.width - 3
            anchors.verticalCenter: parent.verticalCenter
            text: actionMenu.model[camera.flashMode]
            color: "white"
            font { pixelSize: 28; weight: Font.Bold }
        }
    }

    onClicked: {
        var map = mapToItem (window, button.x + (button.width / 2), button.y);

        flashMenu.setPosition( button.x + (button.width / 2), map.y)
        flashMenu.show()
    }

    ContextMenu {
        id: flashMenu

        content: ActionMenu {
            id: actionMenu
            model: camera.flashModel
            onTriggered: {
                camera.flashMode = index
            }
        }
    }
}
