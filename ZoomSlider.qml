/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7

Item {
    id: slider

    width: background.width
    height: background.height

    property real troughStart: 0
    property real troughEnd: trough.height
    property real troughSize: troughEnd - troughStart
    property real stepSize: troughSize / 10

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

    Image {
        id: trough
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        source: "image://theme/camera/camera_zoomtrack_bg_l"

        Image {
            id: scrub

//            x: (parent.width - width)
            anchors.horizontalCenter: parent.horizontalCenter
            y: parent.height - (height / 2)

            source: "image://theme/camera/camera_scrub_head_lrg"
        }

        MouseArea {
            id: mouse
            anchors.fill: parent

            drag.target: scrub
            drag.axis: Drag.YAxis
            drag.minimumY: troughStart - (scrub.height / 2)
            drag.maximumY: troughEnd - (scrub.height / 2)

            onPressed: {
                // Move the scrubhead to the drag start point
                moveScrub (mouse.y - (scrub.height / 2));
            }
        }
    }

    RepeatButton {
        id: up

        anchors.horizontalCenter: parent.horizontalCenter
        y: (trough.y / 2) - (height / 2)

        source: "image://theme/camera/camera_icn_add_up"
        activeSource: "image://theme/camera/camera_icn_add_dn"

        onClicked: {
            moveScrub (scrub.y - stepSize);
        }
    }

    RepeatButton {
        id: down

        anchors.horizontalCenter: parent.horizontalCenter
        y: trough.y + trough.height + (((parent.height - (trough.y + trough.height)) / 2) - (height / 2)) - 9 // -9 for dropshadow

        source: "image://theme/camera/camera_icn_minus_up"
        activeSource: "image://theme/camera/camera_icn_minus_dn"

        onClicked: {
            moveScrub (scrub.y + stepSize);
        }
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
                source: "image://theme/camera/camera_zoom_panel_p"
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
                source: "image://theme/camera/camera_zoom_panel_l"
            }
            StateChangeScript {
//                name: repositionScrubScript
                script: { repositionScrub (); }
            }
        }
    ]
}
