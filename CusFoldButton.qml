import QtQuick 2.0
import Toou2D 1.0

Item {
    height: {
        return btn.bFold ? btn.height :
                           btn.height + content_loader.height;
    }
    property var iconSource: icon.source
    property alias btnColor: btn.color
    property string btnName
    property bool available: false
    property Component content
    Rectangle {
        id: btn
        property bool bFold: true
        width: parent.width
        height: parent.height
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        color: bFold ? "transparent" : "#161D31"
        radius: 5
        Image {
            id: icon
            width: 16
            height: 16
            anchors.top: parent.top
            anchors.topMargin: 13
            anchors.left: parent.left
            anchors.leftMargin: 16
        }

        Text {
            height: 20
            anchors.left: icon.right
            anchors.leftMargin: 11
            anchors.verticalCenter: icon.verticalCenter
            text: groupName
            font.family: local_font.name
            font.pixelSize: 14
            font.weight: Font.Bold
            color: "#FFFFFF"
            verticalAlignment: Text.AlignVCenter
        }
        Image {
            id: indicator
            width: 16
            height: 16
            anchors.top: parent.top
            anchors.topMargin: 13
            anchors.right: parent.right
            anchors.rightMargin: 13
            source: "qrc:/icon/arrow_right"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (btn.bFold) {
                    indicator.source = "qrc:/icon/arrow_down";
                }
                else {
                    indicator.source = "qrc:/icon/arrow_right";
                }
                btn.bFold = !btn.bFold;
            }
        }
    }

    Loader {
        id: content_loader
        width: parent.width
        height: content.height
        anchors.top: btn.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        visible: !btn.bFold
        sourceComponent: content
    }
}
