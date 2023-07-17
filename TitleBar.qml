import QtQuick 2.0


Item {
    id: titlebar
    property alias softIcon: software_icon.source
    property alias softName: software_name.text
    property alias bgColor: bg.color
    property var fontColor: "black"
    property var fontFamily: ""
    property int fontPixelSize: 14

    signal moved(var deltaX, var deltaY)

    implicitWidth: 1280
    implicitHeight: 25

    Rectangle {
        id: bg
        anchors.fill: parent
        color: "white"
    }

    // Left icon and text
    Row {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        spacing: 5

        Image {
            id: software_icon
            width: height
            height: titlebar.height
            source: ""
        }

        Text {
            id: software_name
            width: contentWidth
            height: titlebar.height
            text: qsTr("")
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: fontPixelSize
            font.family: fontFamily
            color: fontColor
        }
    }


    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        property point clickPos: "0,0"
        onPressed: {
            clickPos = Qt.point(mouse.x, mouse.y)
        }

        onPositionChanged: {
            moved(mouse.x - clickPos.x, mouse.y - clickPos.y)
        }
    }
}
