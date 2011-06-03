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

    property int flashMenuRotationAngle: 0
    property bool flashMenuRotationCounterClockwise: false
    property int  flashMenuRotationAnimationDuration: 0


    width: 116
    height: parent.height

    property int value : 1

    signal flashMode (int flashValue);

    Image {
        id: flashImage
        anchors.centerIn: parent

        anchors.verticalCenter: parent.verticalCenter

        transformOrigin: Item.Center
        rotation: flashMenuRotationAngle
        Behavior on rotation { RotationAnimation { duration: flashMenuRotationAnimationDuration; direction: flashMenuRotationCounterClockwise ? RotationAnimation.Counterclockwise : RotationAnimation.Clockwise}}

        property string modename: camera.flashModel[camera.flashMode]
        source:{
            if(modename == "Auto")
                return "image://themedimage/icons/internal/camera-flash-auto"
            else if(modename == "On")
                return "image://themedimage/icons/internal/camera-flash-on"
            else if(modename == "Off")
                return "image://themedimage/icons/internal/camera-flash-off"
            else
                return ""
        }
    }

    onClicked: {
        var map = mapToItem (window, button.x + (button.width / 2), button.y);

        flashMenu.setPosition( button.x + (button.width / 2), map.y)
        flashMenu.show()
    }

    ContextMenu {
        id: flashMenu

        content: ListView {
            model: camera.flashModel
            width: 150
            height: 50 * model.length
            anchors.fill: parent
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
                        return "image://themedimage/icons/internal/camera-flash-auto"
                    else if(name == "On")
                        return "image://themedimage/icons/internal/camera-flash-on"
                    else if(name == "Off")
                        return "image://themedimage/icons/internal/camera-flash-off"
                    else
                        return ""
                }
                activeIconSource:{
                    if(name == "Auto")
                        return "image://themedimage/icons/internal/camera-flash-auto-active"
                    else if(name == "On")
                        return "image://themedimage/icons/internal/camera-flash-on-active"
                    else if(name == "Off")
                        return "image://themedimage/icons/internal/camera-flash-off-active"
                    else
                        return ""
                }

                onClicked: {
                    camera.flashMode = elementIndex;
                    flashMenu.hide();
                }

            }
        }
    }
}
