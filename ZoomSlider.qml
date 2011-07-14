/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.0
import MeeGo.Ux.Gestures 0.1

Item {
    id: slider

    width: background.width
    height: background.height

    property real troughStart: 0
    property real troughEnd: trough.height
    property real troughSize: troughEnd - troughStart
    property real stepSize: troughSize / 10

    property int rotationAngle: 0
    property bool rotationCounterClockwise: false
    property int rotationAnimationDuration: 0

    // zoomLevel is in range [0.0, 1.0]
    // FIXME: zoomLevel should be the "model" and scrub should be the "view"
    // currently this is reversed and is wrong.
    property real zoomLevel: (troughEnd - scrub.y) / troughSize
    property real oldZL: 0.0

    state: "landscape"

    function moveScrub (newY) {
        scrub.y = Math.max (troughStart - (scrub.height / 2), Math.min (troughEnd - (scrub.height / 2), newY));
    }

    function repositionScrub () {
        var ty = (troughEnd - (oldZL * troughSize));
        console.log ("OldZL: " + oldZL.toString () + " ->" + ty.toString ());
        scrub.y = ty;
    }

    function resetZoom () {
        moveScrub (troughEnd);
    }

    Image {
        id: background
//        anchors.fill: parent
    }

    GestureArea {
        id: gestureArea
        x: 0
        y: trough.y
        width: slider.width
        height: trough.height
        property real min: troughStart - (scrub.height / 2)
        property real max: troughEnd - (scrub.height / 2)

        Tap {
            onStarted: {
                if((gesture.position.y <= gestureArea.max) && (gesture.position.y >= gestureArea.min))
                    moveScrub(gesture.position.y - (scrub.height / 2));
            }
        }

        Pan {
            onUpdated: {
                var newPos = scrub.y + gesture.delta.y
                if(newPos < gestureArea.min)
                    scrub.y = gestureArea.min
                else if(newPos > gestureArea.max)
                    scrub.y = gestureArea.max
                else
                    scrub.y = newPos
            }

        }
    }

    Image {
        id: trough
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        source: "image://themedimage/images/camera/camera_zoomtrack_bg_l"

        Image {
            id: scrub

//            x: (parent.width - width)
            anchors.horizontalCenter: parent.horizontalCenter
            y: parent.height - (height / 2)

            source: "image://themedimage/images/camera/camera_scrub_head_lrg"
        }




    }

    Image {
        id: up

        anchors.horizontalCenter: parent.horizontalCenter
        y: (trough.y / 2) - (height / 2)

        source: "image://themedimage/images/camera/camera_icn_add_up"
    }

    Image {
        id: down

        anchors.horizontalCenter: parent.horizontalCenter
        y: trough.y + trough.height + (((parent.height - (trough.y + trough.height)) / 2) - (height / 2)) - 9 // -9 for dropshadow

        source: "image://themedimage/images/camera/camera_icn_minus_up"
    }

    states: [
        State {
            name: "portrait"
            StateChangeScript {
//                name: repositionScrubScript
                script: { oldZL = zoomLevel; }
            }
            PropertyChanges {
                target: slider
                height: 666
            }
            PropertyChanges {
                target: background
                source: "image://themedimage/images/camera/camera_zoom_panel_p"
            }
            StateChangeScript {
//                name: repositionScrubScript
                script: { repositionScrub (); }
            }
        },
        State {
            name: "landscape"
            StateChangeScript {
//              name: repositionScrubScript
                script: { oldZL = zoomLevel; }
            }
            PropertyChanges {
                target: slider
                height: 442
            }
            PropertyChanges {
                target: background
                source: "image://themedimage/images/camera/camera_zoom_panel_l"
            }
            StateChangeScript {
//                name: repositionScrubScript
                script: { repositionScrub (); }
            }
        }
    ]
}
