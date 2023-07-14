import QtQuick 2.0
import QtQuick.Controls 2.14

Rectangle {
    id: image_container
    color: "#000000"
    ScrollView {
        id: image_view
        anchors.fill: parent
        ScrollBar.horizontal.policy: ScrollBar.AsNeeded
        ScrollBar.vertical.policy: ScrollBar.AsNeeded
        clip: true
        contentChildren: Item {
            id: test
            width: image_view.width
            height: image_view.height
            Image {
                id: content
                x: parent.width / 2 - content.width / 2
                y: parent.height / 2 - content.height / 2
            }
        }

    }
    MouseArea {
        property point lastPoint: "0, 0"
        anchors.fill: parent
        z: 1
        acceptedButtons: Qt.LeftButton
        onPressed: {
            lastPoint = Qt.point(mouse.x, mouse.y);
        }
        onPositionChanged: {
            move (mouse.x - lastPoint.x, mouse.y - lastPoint.y)
            lastPoint = Qt.point(mouse.x, mouse.y);
        }
    }

    function move(deltaX, deltaY) {
        test.x = test.x + deltaX;
        test.y = test.y + deltaY;
        console.log(test.x, test.y);
    }

    Connections {
        target: replyParser
        onFileInfoReplyDone: {
            if (netDiskPath) {
                content.source = netDiskPath;
            }
        }
    }
    Connections {
        target: image_controller
        onScaleValueChanged: {
            content.scale = value;
        }
    }
}

