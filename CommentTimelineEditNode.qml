import QtQuick 2.0
import QtQuick.Shapes 1.14
import Toou2D 1.0

Item {
    id: edit_node
    property int startTime;
    property int endTime;
    property double startX;
    property double endX;
    property double timelineScalse;
    property double timeLength;
    readonly property int startHandleWidth: start_handle.width
    implicitWidth: 12
    implicitHeight: 24

    Shape {
        id: start_handle
        width: 6
        height: 18
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        ShapePath {
            strokeWidth: 0
            strokeStyle: ShapePath.SolidLine
            strokeColor: "white"
            startX: 0
            startY: 18
            PathLine { x: 6; y: 18 }
            PathLine { x: 6; y: 0 }
            PathLine { x: 0; y: 6 }
            fillColor: Qt.rgba(217, 217, 217, 0.80)
        }
    }
    Shape {
        id: end_handle
        width: 6
        height: 18
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        ShapePath {
            strokeWidth: 0
            strokeStyle: ShapePath.SolidLine
            strokeColor: "white"
            startX: 0
            startY: 18
            PathLine { x: 6; y: 18 }
            PathLine { x: 6; y: 6 }
            PathLine { x: 0; y: 0 }
            fillColor: Qt.rgba(217, 217, 217, 0.80)
        }
    }
    TRectangle {
        id: middle_bar
        width: edit_node.width -
               start_handle.width -
               end_handle.width
        anchors.left: start_handle.right
        anchors.verticalCenter: parent.verticalCenter
        color: "transparent"

        TRectangle {
            id: middle_bar_handle
            implicitWidth: 12
            implicitHeight: 12
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            radius: 6
            color: "#5767F9"
            enabled: middle_bar.width > 12 ? true : false
        }
        TRectangle {
            id: middle_bar_slider
            width: parent.width
            height: 4
            color: "#5767F9"
        }
    }
}
