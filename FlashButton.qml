/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.0
import MeeGo.Ux.Components.Common 0.1
import MeeGo.App.Camera 0.1
import MeeGo.Ux.Kernel 0.1

PushButton {
    id: button

    property int flashMenuRotationAngle: 0
    property bool flashMenuRotationCounterClockwise: false
    property int  flashMenuRotationAnimationDuration: 0


    width: 116
    height: parent.height

    property int value : 1

    SaveRestoreState {
        // saves: context menu status

        id: borromBarState
        onSaveRequired: {
            setValue("FlashButton.context", flashMenu.visible)
            sync()
        }
        Component.onCompleted: {
            if (restoreRequired) {
                var bContextVisible = value("FlashButton.context", 0)
                if(bContextVisible == "true") {
                    button.clicked(0);
                }
            }
        }
    }

    Image {
        id: flashImage
        anchors.centerIn: parent

        anchors.verticalCenter: parent.verticalCenter

        transformOrigin: Item.Center
        rotation: flashMenuRotationAngle
        Behavior on rotation { RotationAnimation { duration: flashMenuRotationAnimationDuration; direction: flashMenuRotationCounterClockwise ? RotationAnimation.Counterclockwise : RotationAnimation.Clockwise}}

        property int mode: camera.flashMode
        source:{
            if(mode == ViewFinder.Auto)
                return "image://themedimage/icons/internal/camera-flash-auto"
            else if(mode == ViewFinder.On)
                return "image://themedimage/icons/internal/camera-flash-on"
            else if(mode == ViewFinder.Off)
                return "image://themedimage/icons/internal/camera-flash-off"
            else
                return ""
        }
    }

    onClicked: {
        var map = mapToItem (window, button.x + (button.width / 2), button.y);

        flashMenu.setPosition( button.x + (button.width / 2), map.y-15)
        flashMenu.show()
    }

    ContextMenu {
        id: flashMenu

        content: ListView {
            model: camera.flashModel
            width: 150
            height: 50 * model.length
//            anchors.fill: parent
            interactive: false
            delegate: PushButton {
                width: 150
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                rotationAngle: flashMenuRotationAngle
                rotationCounterClockwise: flashMenuRotationCounterClockwise
                rotationAnimationDuration: flashMenuRotationAnimationDuration
                property int elementIndex: index
                property string name: modelData

                iconSource:{
                    if(name == "Auto")
                        return "image://themedimage/icons/internal/camera-flash-auto-menu"
                    else if(name == "On")
                        return "image://themedimage/icons/internal/camera-flash-on-menu"
                    else if(name == "Off")
                        return "image://themedimage/icons/internal/camera-flash-off-menu"
                    else
                        return ""
                }
                activeIconSource:{
                    if(name == "Auto")
                        return "image://themedimage/icons/internal/camera-flash-auto-selected"
                    else if(name == "On")
                        return "image://themedimage/icons/internal/camera-flash-on-selected"
                    else if(name == "Off")
                        return "image://themedimage/icons/internal/camera-flash-off-selected"
                    else
                        return ""
                }

                onClicked: {
                    var mode;
                    if (name == "Auto")
                        mode = ViewFinder.Auto
                    else if (name == "On")
                        mode = ViewFinder.On
                    else if (name == "Off")
                        mode = ViewFinder.Off
                    camera.flashMode = mode;
                    flashMenu.hide();
                }

            }
        }
    }
}
